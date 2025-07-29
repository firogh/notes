#!/usr/bin/env python3
#
# memleak   Trace and display outstanding allocations to detect
#           memory leaks in user-mode processes and the kernel.
#
# USAGE: memleak [-h] [-p PID] [-t] [-a] [-o OLDER] [-c COMMAND]
#                [--combined-only] [-s SAMPLE_RATE] [-T TOP] [-z MIN_SIZE]
#                [-Z MAX_SIZE] [-O OBJ]
#                [interval] [count]
#
# Licensed under the Apache License, Version 2.0 (the "License")
# Copyright (C) 2016 Sasha Goldshtein.

from bcc import BPF
from time import sleep
from datetime import datetime
import resource
import argparse
import subprocess
import os
import code

class Allocation(object):
    def __init__(self, stack, size):
        self.stack = stack
        self.count = 1
        self.size = size

    def update(self, size):
        self.count += 1
        self.size += size

def run_command_get_output(command):
        p = subprocess.Popen(command.split(),
                stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        return iter(p.stdout.readline, b'')

def run_command_get_pid(command):
        p = subprocess.Popen(command.split())
        return p.pid

examples = """
EXAMPLES:

./memleak -p $(pidof allocs)
        Trace allocations and display a summary of "leaked" (outstanding)
        allocations every 5 seconds
./memleak -p $(pidof allocs) -t
        Trace allocations and display each individual allocator function call
./memleak -ap $(pidof allocs) 10
        Trace allocations and display allocated addresses, sizes, and stacks
        every 10 seconds for outstanding allocations
./memleak -c "./allocs"
        Run the specified command and trace its allocations
./memleak
        Trace allocations in kernel mode and display a summary of outstanding
        allocations every 5 seconds
./memleak -o 60000
        Trace allocations in kernel mode and display a summary of outstanding
        allocations that are at least one minute (60 seconds) old
./memleak -s 5
        Trace roughly every 5th allocation, to reduce overhead
"""

description = """
Trace outstanding memory allocations that weren't freed.
Supports both user-mode allocations made with libc functions and kernel-mode
allocations made with kmalloc/kmem_cache_alloc/get_free_pages and corresponding
memory release functions.
"""

parser = argparse.ArgumentParser(description=description,
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=examples)
parser.add_argument("-p", "--pid", type=int, default=-1,
        help="the PID to trace; if not specified, trace kernel allocs")
parser.add_argument("-t", "--trace", action="store_true",
        help="print trace messages for each alloc/free call")
parser.add_argument("interval", nargs="?", default=5, type=int,
        help="interval in seconds to print outstanding allocations")
parser.add_argument("count", nargs="?", type=int,
        help="number of times to print the report before exiting")
parser.add_argument("-a", "--show-allocs", default=False, action="store_true",
        help="show allocation addresses and sizes as well as call stacks")
parser.add_argument("-o", "--older", default=500, type=int,
        help="prune allocations younger than this age in milliseconds")
parser.add_argument("-c", "--command",
        help="execute and trace the specified command")
parser.add_argument("--combined-only", default=False, action="store_true",
        help="show combined allocation statistics only")
parser.add_argument("-s", "--sample-rate", default=1, type=int,
        help="sample every N-th allocation to decrease the overhead")
parser.add_argument("-T", "--top", type=int, default=10,
        help="display only this many top allocating stacks (by size)")
parser.add_argument("-z", "--min-size", type=int,
        help="capture only allocations larger than this size")
parser.add_argument("-Z", "--max-size", type=int,
        help="capture only allocations smaller than this size")
parser.add_argument("-O", "--obj", type=str, default="c",
        help="attach to allocator functions in the specified object")

args = parser.parse_args()

pid = args.pid
command = args.command
kernel_trace = (pid == -1 and command is None)
trace_all = args.trace
interval = args.interval
min_age_ns = 1e6 * args.older
sample_every_n = args.sample_rate
num_prints = args.count
top_stacks = args.top
min_size = args.min_size
max_size = args.max_size
obj = args.obj

if min_size is not None and max_size is not None and min_size > max_size:
        print("min_size (-z) can't be greater than max_size (-Z)")
        exit(1)

if command is not None:
        print("Executing '%s' and tracing the resulting process." % command)
        pid = run_command_get_pid(command)

bpf_source = """
#include <uapi/linux/ptrace.h>

struct alloc_info_t {
        u64 size;
        u64 timestamp_ns;
        int stack_id;
};

struct alloc_data_t {
        int size;
        int stack_id;
};

struct combined_alloc_info_t {
        u64 total_size;
        u64 number_of_allocs;
};

BPF_TABLE("hash", u64, struct alloc_info_t, allocs, 10);
BPF_TABLE("hash", u64, struct combined_alloc_info_t, combined_allocs, 10);

BPF_STACK_TRACE(stack_traces, 40240)
BPF_TABLE("hash", u64, struct alloc_data_t, obj_stack, 400000);
// BPF_TABLE("percpu_array", int, int, nested_level, 10);
// BPF_TABLE("percpu_array", int, struct alloc_data_t, nested_stacks, 1024);
BPF_HASH(sizes, u64, struct alloc_data_t);

static inline void update_statistics_add(u64 stack_id, u64 sz) {
        struct combined_alloc_info_t *existing_cinfo;
        struct combined_alloc_info_t cinfo = {0};

        existing_cinfo = combined_allocs.lookup(&stack_id);
        if (existing_cinfo != 0)
                cinfo = *existing_cinfo;

        cinfo.total_size += sz;
        cinfo.number_of_allocs += 1;

        combined_allocs.update(&stack_id, &cinfo);
}

static inline void update_statistics_del(u64 stack_id, u64 sz) {
        struct combined_alloc_info_t *existing_cinfo;
        struct combined_alloc_info_t cinfo = {0};

        existing_cinfo = combined_allocs.lookup(&stack_id);
        if (existing_cinfo != 0)
                cinfo = *existing_cinfo;

        if (sz >= cinfo.total_size)
                cinfo.total_size = 0;
        else
                cinfo.total_size -= sz;

        if (cinfo.number_of_allocs > 0)
                cinfo.number_of_allocs -= 1;

        combined_allocs.update(&stack_id, &cinfo);
}

"""

bpf_source_kernel = """

#define bpf_probe_read_kernel bpf_probe_read

/* https://github.com/iovisor/bpftrace/issues/101 */
static inline int kp_alloc_core(struct pt_regs *ctx, int len, int ki)
{
    unsigned char *kc;
    unsigned int size = len;
    int stack_id;
    struct alloc_data_t d;
    u64 task;

    if (size ==  0) {
        bpf_probe_read_kernel(&kc, sizeof(kc), &ctx->di);
        bpf_probe_read_kernel(&size, sizeof(size), (void *)(kc + 0x14) );
        len = size;
    } else {
        if ((256 < size) && (size <=512))
            size = 512;
    }

    /* we are ready to do something */
    if (size != 512)
        return 0;

    //bpf_trace_printk("Found %d, %d %d\\n", ki, size, stack_id);
    task =  bpf_get_current_task();
    stack_id = stack_traces.get_stackid(ctx, 0);
    d.size = len;
    d.stack_id = stack_id;

    sizes.update(&task, &d);
    return 0;
}

static inline int kretp_alloc_core(struct pt_regs *ctx, int ki)
{
    int *idx = 0, v, id;
    u32 zero = 0;
    u64 obj;
    u64 task;
    struct alloc_data_t *dp,d;
    int stack_id;

    task =  bpf_get_current_task();

    dp = sizes.lookup(&task);

    if (dp == 0)
        return 0;
    d =  *dp;

    sizes.delete(&task);

    bpf_probe_read_kernel(&obj, sizeof(obj), &ctx->ax);

    //stack_id = stack_traces.get_stackid(ctx, 0);
    //d->stack_id =  stack_id;
    obj_stack.update(&obj, &d);
    // bpf_trace_printk("Found ret %d %d %lx\\n", ki, stack_id, obj);

    return 0;
}

static inline int kp_free_core(struct  pt_regs *ctx, u64 obj)
{
    struct alloc_data_t *dp =  obj_stack.lookup(&obj);
        if (dp == 0)
                return 0;

        //bpf_trace_printk("Found free %d %lx\\n", *stack_id, obj);

        obj_stack.delete(&obj);
        //update_statistics_del(*stack_id, 512);

        return 0;
}

int kp_kmem_cache_alloc_trace(struct pt_regs *ctx) {

    return kp_alloc_core(ctx, 0, 1);
}

int kretp_kmem_cache_alloc_trace(struct pt_regs *ctx) {
    return kretp_alloc_core(ctx, 2);
}

int kp_kmem_cache_free(struct pt_regs *ctx) {
    u64 obj;
    
    bpf_probe_read_kernel(&obj, sizeof(obj), &ctx->si);
    return kp_free_core(ctx, obj);
}
int kp_kfree(struct pt_regs *ctx) {
    u64 obj;
    
    bpf_probe_read_kernel(&obj, sizeof(obj), &ctx->di);
    return kp_free_core(ctx, obj);
}

int kp___kmalloc(struct pt_regs *ctx) {
    int size = 0;
    bpf_probe_read_kernel(&size, sizeof(size), &ctx->di);
    return kp_alloc_core(ctx, size, 3);
}
int kretp___kmalloc(struct pt_regs *ctx) {
    return kretp_alloc_core(ctx, 4);
}

int kp___kmalloc_node(struct pt_regs *ctx) {
    int size = 0;
    bpf_probe_read_kernel(&size, sizeof(size), &ctx->di);
    return kp_alloc_core(ctx, size, 30);
}

int kretp___kmalloc_node(struct pt_regs *ctx) {
    return kretp_alloc_core(ctx, 40);
}

int kp___kmalloc_node_track_caller(struct pt_regs *ctx) {
    int size = 0;
    bpf_probe_read_kernel(&size, sizeof(size), &ctx->di);
    return kp_alloc_core(ctx, size, 31);
}
int kretp___kmalloc_node_track_caller(struct pt_regs *ctx) {
    return kretp_alloc_core(ctx, 41);
}

int kp___kmalloc_track_caller(struct pt_regs *ctx) {
    int size = 0;
    bpf_probe_read_kernel(&size, sizeof(size), &ctx->di);
    return kp_alloc_core(ctx, size, 5);
}
int kretp___kmalloc_track_caller(struct pt_regs *ctx) {
    return kretp_alloc_core(ctx, 6);
}
/*
 __do_kmalloc inlined:  __kmalloc 
 kmem_cache_alloc_trace

 __kmalloc_track_caller for kstrdup

TRACEPOINT_PROBE(kmem, kmalloc) {
        gen_alloc_enter((struct pt_regs *)args, args->bytes_alloc);
        return gen_alloc_exit2((struct pt_regs *)args, (size_t)args->ptr);
}
*/

int kp_kmem_cache_alloc_node_trace(struct pt_regs *ctx) {

    return kp_alloc_core(ctx, 0, 7);
}

int kretp_kmem_cache_alloc_node_trace(struct pt_regs *ctx) {
    return kretp_alloc_core(ctx, 8);
}
/*
  kmem_cache_alloc_node_trace

TRACEPOINT_PROBE(kmem, kmalloc_node) {
        gen_alloc_enter((struct pt_regs *)args, args->bytes_alloc);
        return gen_alloc_exit2((struct pt_regs *)args, (size_t)args->ptr);
}
*/
/*
    kfree

TRACEPOINT_PROBE(kmem, kfree) {
        return kp_free_core((struct pt_regs *)args, (u64)args->ptr);
}

*/
int kp_kmem_cache_alloc(struct pt_regs *ctx) {

    return kp_alloc_core(ctx, 0, 9);
}

int kretp_kmem_cache_alloc(struct pt_regs *ctx) {
    return kretp_alloc_core(ctx, 10);
}

/*
    kmem_cache_alloc
TRACEPOINT_PROBE(kmem, kmem_cache_alloc) {
        gen_alloc_enter((struct pt_regs *)args, args->bytes_alloc);
        return gen_alloc_exit2((struct pt_regs *)args, (size_t)args->ptr);
}

*/
int kp_kmem_cache_alloc_node(struct pt_regs *ctx) {

    return kp_alloc_core(ctx, 0, 11);
}

int kretp_kmem_cache_alloc_node(struct pt_regs *ctx) {
    return kretp_alloc_core(ctx, 12);
}

/* kmem_cache_alloc_node
TRACEPOINT_PROBE(kmem, kmem_cache_alloc_node) {
        gen_alloc_enter((struct pt_regs *)args, args->bytes_alloc);
        return gen_alloc_exit2((struct pt_regs *)args, (size_t)args->ptr);
}
*/
/*
kmem_cache_free
TRACEPOINT_PROBE(kmem, kmem_cache_free) {
        return kp_free_core((struct pt_regs *)args, (u64) args->ptr);
}
*/

"""

if kernel_trace:
        bpf_source += bpf_source_kernel

bpf_source = bpf_source.replace("SHOULD_PRINT", "1" if trace_all else "0")
bpf_source = bpf_source.replace("SAMPLE_EVERY_N", str(sample_every_n))
bpf_source = bpf_source.replace("PAGE_SIZE", str(resource.getpagesize()))

size_filter = ""
if min_size is not None and max_size is not None:
        size_filter = "if (size < %d || size > %d) return 0;" % \
                      (min_size, max_size)
elif min_size is not None:
        size_filter = "if (size < %d) return 0;" % min_size
elif max_size is not None:
        size_filter = "if (size > %d) return 0;" % max_size
bpf_source = bpf_source.replace("SIZE_FILTER", size_filter)

stack_flags = "BPF_F_REUSE_STACKID"
if not kernel_trace:
        stack_flags += "|BPF_F_USER_STACK"
bpf_source = bpf_source.replace("STACK_FLAGS", stack_flags)

bpf = BPF(text=bpf_source)
bpf.attach_kretprobe(event="__kmalloc_track_caller", fn_name="kretp___kmalloc_track_caller")
bpf.attach_kprobe(event="__kmalloc_track_caller", fn_name="kp___kmalloc_track_caller")
bpf.attach_kretprobe(event="kmem_cache_alloc_node_trace", fn_name="kretp_kmem_cache_alloc_node_trace")
bpf.attach_kprobe(event="kmem_cache_alloc_node_trace", fn_name="kp_kmem_cache_alloc_node_trace")
bpf.attach_kretprobe(event="kmem_cache_alloc", fn_name="kretp_kmem_cache_alloc")
bpf.attach_kprobe(event="kmem_cache_alloc", fn_name="kp_kmem_cache_alloc")
bpf.attach_kretprobe(event="kmem_cache_alloc_node", fn_name="kretp_kmem_cache_alloc_node")
bpf.attach_kprobe(event="kmem_cache_alloc_node", fn_name="kp_kmem_cache_alloc_node")

bpf.attach_kretprobe(event="kmem_cache_alloc_trace", fn_name="kretp_kmem_cache_alloc_trace")
bpf.attach_kprobe(event="kmem_cache_alloc_trace", fn_name="kp_kmem_cache_alloc_trace")

bpf.attach_kretprobe(event="__kmalloc", fn_name="kretp___kmalloc")
bpf.attach_kprobe(event="__kmalloc", fn_name="kp___kmalloc")

bpf.attach_kretprobe(event="__kmalloc_node", fn_name="kretp___kmalloc_node")
bpf.attach_kprobe(event="__kmalloc_node", fn_name="kp___kmalloc_node")

bpf.attach_kretprobe(event="__kmalloc_node_track_caller", fn_name="kretp___kmalloc_node_track_caller")
bpf.attach_kprobe(event="__kmalloc_node_track_caller", fn_name="kp___kmalloc_node_track_caller")




bpf.attach_kprobe(event="kfree", fn_name="kp_kfree")
bpf.attach_kprobe(event="kmem_cache_free", fn_name="kp_kmem_cache_free")

print("Attaching to kernel allocators, Ctrl+d to quit.")

def print_outstanding():
        print("[%s] Top %d stacks with outstanding allocations:" %
              (datetime.now().strftime("%H:%M:%S"), top_stacks))
        alloc_info = {}
        allocs = bpf["allocs"]
        stack_traces = bpf["stack_traces"]
        for address, info in sorted(allocs.items(), key=lambda a: a[1].size):
                if BPF.monotonic_time() - min_age_ns < info.timestamp_ns:
                        continue
                if info.stack_id < 0:
                        continue
                if info.stack_id in alloc_info:
                        alloc_info[info.stack_id].update(info.size)
                else:
                        stack = list(stack_traces.walk(info.stack_id))
                        combined = []
                        for addr in stack:
                                combined.append(bpf.sym(addr, pid,
                                        show_module=True, show_offset=True))
                        alloc_info[info.stack_id] = Allocation(combined,
                                                               info.size)
                if args.show_allocs:
                        print("\taddr = %x size = %s" %
                              (address.value, info.size))
        to_show = sorted(alloc_info.values(),
                         key=lambda a: a.size)[-top_stacks:]
        for alloc in to_show:
                print("\t%d bytes in %d allocations from stack\n\t\t%s" %
                      (alloc.size, alloc.count, b"\n\t\t".join(alloc.stack)))

def dst_x(stack_id):
    stack_traces = bpf["stack_traces"]
    trace = "=> id %ld "%(stack_id)
    try:
        for addr in stack_traces.walk(stack_id):
            sym = bpf.sym(addr, pid,show_module=True,show_offset=True)
            #trace +=  "".join(str(sym)) + "\n"
            trace +=  "".join(str(sym))
    except KeyError:
            trace = "stack information lost\n"
    print(trace)

def print_outstanding_x():
        print("[%s] Top %d stacks with outstanding kmalloc-512 allocations:" %
              (datetime.now().strftime("%Y-%m-%d %H:%M:%S"), top_stacks))
        allocs = bpf["obj_stack"]
        stack_counts = {}
        request_size = {}
        obj_size = 512
        for obj, data in allocs.items():
            #id = stack_id.value
            id = data.stack_id
            # obj_size = data.size
            #print("stack_id  %d %lx\n"%(id, obj.value))
            if id not in stack_counts:
                stack_counts[id] = 0
                request_size[id] = 0
            stack_counts[id] = stack_counts[id] + obj_size
            request_size[id] = request_size[id] + data.size

        i = 0
        for stack_id, counts in sorted(stack_counts.items(), key=lambda a: a[1], reverse=True):
                if stack_id < 0:
                        continue
                print("stack_id %d, outstanding memory %d B, rquest allocations %d B:"%(stack_id, counts, request_size[stack_id]))
                dst_x(stack_id)
                i = i + 1
                if i >= top_stacks:
                    break

def print_outstanding_combined():
        stack_traces = bpf["stack_traces"]
        stacks = sorted(bpf["combined_allocs"].items(),
                        key=lambda a: -a[1].total_size)
        cnt = 1
        entries = []
        for stack_id, info in stacks:
                try:
                        trace = []
                        for addr in stack_traces.walk(stack_id.value):
                                sym = bpf.sym(addr, pid,
                                                      show_module=True,
                                                      show_offset=True)
                                trace.append(sym)
                        trace = "\n\t\t".join(trace)
                except KeyError:
                        trace = "stack information lost"

                entry = ("\t%d bytes in %d allocations from stack\n\t\t%s" %
                         (info.total_size, info.number_of_allocs, trace))
                entries.append(entry)

                cnt += 1
                if cnt > top_stacks:
                        break

        print("[%s] Top %d stacks with outstanding allocations:" %
              (datetime.now().strftime("%H:%M:%S"), top_stacks))

        print('\n'.join(reversed(entries)))

count_so_far = 0
def dump_this_stack_trace(stack_id):
    stack_traces = bpf["stack_traces"]
    # trace = ["=> id %ld "%(stack_id)]
    trace = "=> id %ld "%(stack_id)
    try:
        for addr in stack_traces.walk(stack_id):
            sym = bpf.sym(addr, pid,show_module=True,show_offset=True)
            #trace.append(str(sym))
            # trace = "\n\t\t".join(trace)
            # trace = " ".join(trace)
            trace +="\n"
    except KeyError:
            trace = "stack information lost\n"
    print(trace)

def dst(stack_id):
    stack_traces = bpf["stack_traces"]
    trace = "=> id %ld "%(stack_id)
    try:
        for addr in stack_traces.walk(stack_id):
            sym = bpf.sym(addr, pid,show_module=True,show_offset=True)
            trace +=  "".join(str(sym)) + "\n"
    except KeyError:
            trace = "stack information lost\n"
    print(trace)

def dtf():
    try:
        while True:
            print(bpf.trace_fields())
    except KeyboardInterrupt:
        return

def save_kmalloc512_stackid(filename):
    f = open(filename, "w+")
    objs = bpf["obj_stack"].items()
    for obj,data in objs:
        id = data.stack_id
        size = data.size
        addr = obj.value
        f.write("%x %d %ld \n"%(addr, size, id))
    f.close()

def save_stack_traces(filename):
    stack_traces = bpf["stack_traces"]
    traces = stack_traces.items()
    f = open(filename, "w+")
    for stack_id, sp in traces:
        id = stack_id.value
        trace = "=> id %ld "%(id)
        try:
            for addr in stack_traces.walk(id):
                    sym = bpf.sym(addr, pid,
                                          show_module=True,
                                          show_offset=True)
                    # trace += "".join(str(sym))
                    trace += str(sym)
           # trace = "\n\t\t".join(trace)
           # trace = " ".join(trace)
            trace +="\n"
        except KeyError:
            trace = "stack information lost\n"
        f.write(trace)
    f.close()

def save_slabinfo(filename, st, ss, es):
    f = open(filename, "w+")
    st += "\n"
    f.write(st)
    f.write(ss)
    f.write(es)
    f.close()

def save_data(st, ss, es):
    tag =  datetime.now().strftime("%Y_%m_%d_%I_%M_%S_%p")
    tag = "suse_"+tag+"_"

    save_stack_traces("./"+tag+"stack_traces.log")
    save_kmalloc512_stackid("./"+tag+"kmalloc512_stackid.log")
    save_slabinfo("./"+tag+"slabinfo.log", st, ss, es)


def dump_data():
    tag =  datetime.now().strftime("%Y_%m_%d_%I_%M_%S_%p")
    tag = "suse_"+tag
    dump_stack_traces("./", tag)
    dump_page_stackid("./", tag)

def read_slabinfo():
    with open("/proc/slabinfo", 'r') as content_file:
        buffer = content_file.read()
    return buffer

start_slabinfo = ""
start_time = ""
start_kmalloc512 =""
k512 = 0

def get_k512():
    with open("/proc/slabinfo", 'r') as file:
        for line in file:
            if line.startswith("kmalloc-512"):
                return line

def stat():
    curr = get_k512()
    curr = curr.split()
    delta = int(curr[1]) - k512
    print("Outstanding kmalloc-512 %d B %d KB : curr %s since %d %s"%(delta * 512, delta/2, curr[1], k512, start_time))
    print_outstanding_x()

# print("Start debug %d\n"%(trace_all))
end_slabinfo = ""
#start_time = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
#start_slabinfo = read_slabinfo()
#save_data(start_time, start_slabinfo, end_slabinfo)

def test_save():
    save_data(start_time, start_slabinfo, end_slabinfo)

while True:
        try:
            if trace_all:
                        print(bpf.trace_fields())
                        #code.interact(local=locals())
            else:
                    try:
                            #sleep(interval)
                            start_slabinfo = read_slabinfo()
                            start_time = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
                            start_kmalloc512 = get_k512()
                            tmp =  start_kmalloc512.split()
                            k512 = int(tmp[1])
                            code.interact(local=locals())
                            end_slabinfo = read_slabinfo()
                            save_data(start_time, start_slabinfo, end_slabinfo)
                            sleep(1)
                            exit()
                    except KeyboardInterrupt:
                            exit()

                    print_outstanding_x()

        except KeyboardInterrupt:
            code.interact(local=locals())
