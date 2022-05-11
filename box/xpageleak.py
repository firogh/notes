#!/usr/bin/env python3
#
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
import sys
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

./xpageleak.py
        Trace allocations in kernel mode and display a summary of outstanding
        allocations every 5 seconds
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
        help="the PID to trace; if not specified, trace kernel ")
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
parser.add_argument("--wa-missing-free", default=False, action="store_true",
        help="Workaround to alleviate misjudgments when free is missing")
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
parser.add_argument("--ebpf", action="store_true",
        help=argparse.SUPPRESS)

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
#include <linux/jhash.h>
#include <linux/mm.h>
#include <linux/bpf.h>
#include <uapi/linux/bpf.h>

struct alloc_info_t {
        u64 size;
        u32 order;
	u32 offset;
        u64 timestamp_ns;
        u64 stack_id;
};

struct stack_counters_t {
        u64 arqs;   // allocation reqeusts
        long  long alloced;
        u64 freed; 
	u64 total;
};

BPF_HASH(kp_alloc_stackid, u32);
BPF_HASH(stack_counters, u64, struct stack_counters_t, 10240);
BPF_HASH(page_stackid, u64, struct alloc_info_t, 3096000); // ~12GB
BPF_STACK_TRACE(stack_traces, 20480);
BPF_TABLE("percpu_hash", u64, u64, stack_alloc_counter, 10240);
BPF_TABLE("percpu_hash", u64, u64, stack_free_counter, 10240);

static inline void stack_counters_add(u64 stack_id, u64 sz) {
        struct stack_counters_t *spp, sp;
	u64 count = 0, *countp;
        
        spp = stack_counters.lookup(&stack_id);
        if (spp != 0)
            sp = *spp;
	else {
		sp.arqs = 0;
		sp.alloced = 0;
		sp.freed = 0;
		sp.total = 0;
	}

        sp.alloced += sz;
	sp.total +=sz;
        sp.arqs += 1;

        stack_counters.update(&stack_id, &sp);
	countp = stack_alloc_counter.lookup(&stack_id);
	if (countp != 0)
		count = *countp;
	count += sz;
	stack_alloc_counter.update(&stack_id, &count);
}

static inline void stack_counters_del(u64 stack_id, u64 sz) {
        struct stack_counters_t *spp, sp;
	u64 count = 0, *countp;

        spp = stack_counters.lookup(&stack_id);
        if (spp != 0) {
                sp = *spp;
                sp.freed += sz;
                sp.alloced -= sz;
		if (sp.alloced < 0) /* because alloced isn't atomatic. */
			sp.alloced = 0; 
                stack_counters.update(&stack_id, &sp);
        }
	countp = stack_free_counter.lookup(&stack_id);
	if (countp != 0)
		count = *countp;

	count += sz;
	stack_free_counter.update(&stack_id, &count);
}

static inline void add_one_page(struct alloc_info_t *info, u64 pfn)
{
	page_stackid.update(&pfn, info);
}
static inline int xpage_alloc_enter(struct pt_regs *ctx, u64 pfn, int order) {
        u32 pid32 = bpf_get_current_pid_tgid();
        struct alloc_info_t info;
	u64 *stack_id64;
        u32 i;

	if (pfn == 0)
		return 0;

	stack_id64 = kp_alloc_stackid.lookup(&pid32);
	if (stack_id64 == 0)
		return 0;
	kp_alloc_stackid.delete(&pid32);

	info.stack_id = *stack_id64;
	//info.stack_id = stack_traces.get_stackid(ctx, STACK_FLAGS);
	info.timestamp_ns = bpf_ktime_get_ns();
        info.size = 4096;
	info.order = order;
	stack_counters_add(info.stack_id, 4096 << order);
	i = 0;
	info.offset = 0;

	if (order <= 3) {
		add_one_page(&info, pfn + i);
		if ((i +1) == (1<< order))
			return 0;
		i++;

		info.offset = i;
		add_one_page(&info, pfn +i);
		if ((i +1) == (1<< order))
			return 0;
		i++;	

		info.offset = i;
		add_one_page(&info, pfn +i);
		if ((i +1) == (1<< order))
			return 0;
		i++;	

		info.offset = i;
		add_one_page(&info, pfn +i);
		if ((i +1) == (1<< order))
			return 0;
		i++;	

		info.offset = i;
		add_one_page(&info, pfn +i);
		if ((i +1) == (1<< order))
			return 0;
		i++;	

		info.offset = i;
		add_one_page(&info, pfn +i);
		if ((i +1) == (1<< order))
			return 0;
		i++;	

		info.offset = i;
		add_one_page(&info, pfn +i);
		if ((i +1) == (1<< order))
			return 0;
		i++;	

		info.offset = i;
		add_one_page(&info, pfn +i);
		if ((i +1) == (1<< order))
			return 0;
	} else {
		info.size = 4096 <<order;
		page_stackid.update(&pfn, &info);
	}
        return 0;
}

static inline int del_one_page(u64 addr)
{
	struct alloc_info_t *info;
	info = page_stackid.lookup(&addr);
	if (info == 0)
		return 0;
	page_stackid.delete(&addr);
	stack_counters_del(info->stack_id, 4096);

	return 0;
}

static inline int xpage_free_enter(struct pt_regs *ctx, void *pfn, unsigned int order) {
	struct alloc_info_t *info;
        u64 addr = (u64)pfn;
	int i = 0;

	if (order <= 3) {
		del_one_page(addr + i);
		if ((i+1) == (1<<order))
			return 0;
		i++;
		del_one_page(addr + i);
		if ((i+1) == (1<<order))
			return 0;
		i++;
		del_one_page(addr + i);
		if ((i+1) == (1<<order))
			return 0;
		i++;
		del_one_page(addr + i);
		if ((i+1) == (1<<order))
			return 0;
		i++;
		del_one_page(addr + i);
		if ((i+1) == (1<<order))
			return 0;
		i++;
		del_one_page(addr + i);
		if ((i+1) == (1<<order))
			return 0;
		i++;
		del_one_page(addr + i);
		if ((i+1) == (1<<order))
			return 0;
		i++;
		del_one_page(addr + i);
		if ((i+1) == (1<<order))
			return 0;
	} else {
		info = page_stackid.lookup(&addr);
		if (info == 0)
			return 0;
		page_stackid.delete(&addr);
	/*
		if (info->size < (4096 << order))
			bpf_trace_printk("------: %ld %ld %ld\\n",info->stack_id,info->size,4096 << order);
	*/

		stack_counters_del(info->stack_id, 4096 << order);
	}
	return 0;
}



/* https://github.com/iovisor/bpftrace/issues/101 */
int kp_xpage_alloc_enter(struct pt_regs *ctx) {
        u32 pid32 = bpf_get_current_pid_tgid();
	u64 stack_id = stack_traces.get_stackid(ctx, 0);

	kp_alloc_stackid.update(&pid32, &stack_id);

        return 0;
}
"""

bpf_source_kernel = """
TRACEPOINT_PROBE(kmem, mm_page_alloc) {
       return xpage_alloc_enter((struct pt_regs *)args, args->pfn, args->order);
}

TRACEPOINT_PROBE(kmem, mm_page_free) {
        return xpage_free_enter((struct pt_regs *)args, (void *)args->pfn, args->order);
}
"""

bpf_source += bpf_source_kernel


stack_flags = "0"
bpf_source = bpf_source.replace("STACK_FLAGS", stack_flags)

if args.ebpf:
    print(bpf_source)
    exit()

bpf = BPF(text=bpf_source)
bpf.attach_kprobe(event="__alloc_pages_nodemask", fn_name="kp_xpage_alloc_enter")

print("Attaching to kernel allocators, Ctrl+C to quit.")
def print_outstanding():
        print("[%s] Top %d stacks with outstanding allocations:" %
              (datetime.now().strftime("%H:%M:%S"), top_stacks))
        alloc_info = {}
        page_stackid = bpf["page_stackid"]
        stack_traces = bpf["stack_traces"]
        for address, info in sorted(page_stackid.items(), key=lambda a: a[1].size):
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
                                combined.append(('0x'+format(addr, '016x')+'\t').encode('utf-8') + bpf.sym(addr, pid,
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
                      (alloc.size, alloc.count,
                       b"\n\t\t".join(alloc.stack).decode("ascii")))

def print_outstanding_combined():
        stack_traces = bpf["stack_traces"]
        stacks = sorted(bpf["stack_counters"].items(),
                        key=lambda a: -a[1].alloced)
        cnt = 1
        entries = []
        for stack_id, info in stacks:
                try:
                        trace = []
                        for addr in stack_traces.walk(stack_id.value):
                                sym = bpf.sym(addr, pid,
                                                      show_module=True,
                                                      show_offset=True)
                                trace.append(str(sym))
                        trace = "\n\t\t".join(trace)
                except KeyError:
                        trace = "stack information lost"
                entry = ("\ttotoal: %d, %d bytes in %d allocations from stack\n\t\t%s" %(info.total, info.alloced, info.arqs, trace))
                entries.append(entry)
                cnt += 1
                if cnt > top_stacks:
                        break

        print("[%s] Top %d stacks with outstanding allocations:" %
              (datetime.now().strftime("%H:%M:%S"), top_stacks))

        print('\n'.join(reversed(entries)))

def xs(stack_id):
        stack_traces = bpf["stack_traces"]
        trace = []
        for addr in stack_traces.walk(stack_id):
                sym = bpf.sym(addr, pid,
                                      show_module=True,
                                      show_offset=True)
                trace.append(str(sym))
        trace = "\n\t\t".join(trace)
        print(trace)

count_so_far = 0

def print_total_outstanding_pages_from_stack_counters():
	stacks = bpf["stack_counters"].items()
	total_alloced = 0
	total_freed = 0
	total_requests = 0
	for stack_id, info in stacks:
		total_alloced += info.alloced
		total_freed += info.freed
		total_requests += info.arqs
		if info.total < info.freed:
			print("Lost counters: Stackid %d Outstanding %ld , totoal %d, freed %ld , in %d allocations"%(stack_id.value, info.alloced/4096, info.total/4096, info.freed/4096, info.arqs))
	print(datetime.now().strftime("%Y_%m_%d_%I_%M_%S_%p"))
	print(": Outstanding %ld , alloced %ld , freed %ld , in %d allocations"
			%(total_alloced /4096, (total_alloced + total_freed)/4096, total_freed/4096, total_requests))
def oxs():
        stacks = bpf["stack_counters"].items()
        total_alloced = 0
        total_freed = 0
        total_requests = 0
        for stack_id, info in stacks:
                total_alloced += info.alloced
                total_freed += info.freed
                total_requests += info.arqs
                print("sid %d Outstanding %ld , totoal %ld, freed %ld , in %d allocations"%(stack_id.value, info.alloced, info.total/4096, info.freed/4096, info.arqs))
def os():
	print_total_outstanding_pages_from_stack_counters()

def op():
	print_total_outstanding_pages_from_stack_counters()

def interactive_pageleak():
        code.interact(local=locals())

#        u64 size;
#        u32 order;
#        u32 offset;
#        u64 timestamp_ns;
#        u64 stack_id;

def dump_page_stackid(dir, file_prefix):
    pses = bpf["page_stackid"].items()
    f = open(dir+"/"+file_prefix+"_pages_stackid.log", "w+")
    for pfn, ps in pses:
        f.write("%ld %ld %ld %ld %ld %ld \n"%(pfn.value, ps.order, ps.offset, ps.size, ps.timestamp_ns, ps.stack_id))
def dump_stack_counters(dir, file_prefix):
    sps = bpf["stack_counters"].items() 
    f = open(dir+"/"+file_prefix+"_stack_counters.log", "w+")
    for sid, sp in sps:
        f.write("%ld %ld %ld %ld %ld \n"%(sid.value, sp.arqs, sp.alloced, sp.freed, sp.total))
def dump_stack_traces(dir, file_prefix):
    f = open(dir+"/"+file_prefix+"_stack_traces.log", "w+")
    stack_counters = bpf["stack_counters"].items()
    stack_traces = bpf["stack_traces"]
    for stack_id, sp in stack_counters:
        trace = ["=> %ld %ld %ld %ld %ld "%(stack_id.value, sp.arqs, sp.alloced, sp.freed, sp.total)]
        try:
                for addr in stack_traces.walk(stack_id.value):
                        sym = bpf.sym(addr, pid,
                                              show_module=True,
                                              show_offset=True)
                        trace.append(str(sym))
               # trace = "\n\t\t".join(trace)
                trace = " ".join(trace)
                trace +="\n"
        except KeyError:
                trace = "stack information lost\n"
        f.write(trace)
def xa():
	s = bpf["stack_alloc_counter"].items()
	for i,c in s:
		print("%ld "%(i.value), end="")
		for j in c:
			print("%ld "%(j), end="")
		print("")

def xf():
	s = bpf["stack_free_counter"].items()
	for i,c in s:
		print("%ld "%(i.value), end="")
		for j in c:
			print("%ld "%(j), end="")
		print("")

def save_counter_items(items, filename):
	fileobject = open(filename, "w+")
	for i,a in items:
		entry = "%ld %ld"%(i.value, sum(a))
		for j in a:
			entry += " %ld"%(j)
		fileobject.write(entry)

def save_stack_traces(stack_counters, filename):
    stack_traces = bpf["stack_traces"]
    f = open(filename, "w+")	
    for stack_id, sp in stack_counters:
        trace = ["=> %ld %ld %ld %ld %ld "%(stack_id.value, sp.arqs, sp.alloced, sp.freed, sp.total)]
        try:
                for addr in stack_traces.walk(stack_id.value):
                        sym = bpf.sym(addr, pid,
                                              show_module=True,
                                              show_offset=True)
                        trace.append(str(sym))
               # trace = "\n\t\t".join(trace)
                trace = " ".join(trace)
                trace +="\n"
        except KeyError:
                trace = "stack information lost\n"
        f.write(trace)

def save_page_stackid(items, filename):
	f = open(filename, "w+")
	for pfn, ps in items:
		f.write("%ld %ld %ld %ld %ld %ld \n"%(pfn.value, ps.order, ps.offset, ps.size, ps.timestamp_ns, ps.stack_id))

def save_debug_data_from_bpf_maps():
	tag =  datetime.now().strftime("%Y_%m_%d_%I_%M_%S_%p")
	tag = "suse_"+tag+"_"
	page_stackid_items = bpf["page_stackid"].items()
	alloc_counter_items = bpf["stack_alloc_counter"].items()
	free_counter_items = bpf["stack_free_counter"].items()
	stack_counters_items = bpf["stack_counters"].items()

	save_counter_items(alloc_counter_items, "./"+tag+"stack_alloc_counter.log")
	save_counter_items(free_counter_items, "./"+tag+"stack_free_counter.log")
	save_stack_traces(stack_counters_items, "./"+tag+"stack_traces.log")
	save_page_stackid(page_stackid_items, "./"+tag+"page_stackid.log")

def test_data():
    dump_stack_traces("./", "firo")
    dump_page_stackid("./", "firo")

def save_data():
	tag =  datetime.now().strftime("%Y_%m_%d_%I_%M_%S_%p")
	tag = "suse_"+tag
	dump_stack_traces("./", tag)
	dump_page_stackid("./", tag)
        
def uf():
    while 1:
        print(bpf.trace_fields())


while True:
        if trace_all:
                print(bpf.trace_fields())
        else:
                code.interact(local=locals())
                save_debug_data_from_bpf_maps()
                sys.stdout.flush()
                sleep(1)
                exit(0)
