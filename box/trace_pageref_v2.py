#!/usr/bin/env python3

from bcc import BPF
from time import sleep
import struct
import json
import sys
import argparse
import code
from datetime import datetime

program = """
#include <linux/mm.h>
#include <linux/bpf.h>

struct pfn_stack_entry {
    u64 pfn;
    u64 stack_id;
    u64 mapping;
    u64 ts;
    unsigned long flags;
    int val;
    int count;
    int ret;
    int type;
};

BPF_STACK_TRACE(stack_traces, 10240);
BPF_HASH(stack_counts, u64, u64, 10240);
BPF_RINGBUF_OUTPUT(events, 1 << 16);

static int trace_page_ref(struct pt_regs *ctx, u64 pfn, unsigned long flags, 
        void *mapping, int count, int val, int ret, int type ) {
    struct pfn_stack_entry *entry;
    u64 stack_id = stack_traces.get_stackid(ctx, BPF_F_REUSE_STACKID);

    u64 *old = stack_counts.lookup(&stack_id);
    u64 new = 1;
    if (old) {
        new = *old + 1;
    }
    stack_counts.update(&stack_id, &new);
    entry = events.ringbuf_reserve(sizeof(*entry));
    if (!entry)
        return 0;
    
    entry->pfn = pfn;
    entry->stack_id = stack_id;
    entry->count = count;
    entry->flags = flags;
    entry->ts = bpf_ktime_get_ns();
    entry->mapping = (u64) mapping;
    entry->val = val;
    entry->ret = ret;
    entry->type = type;

    events.ringbuf_submit(entry, 0);
    return 0;
}
"""

bpf_source_kernel = """
/*
static int trace_page_ref(struct pt_regs *ctx, u64 pfn, unsigned long flags, 
        void *mapping, int count, int val, int ret, int type ) {
    */
 
TRACEPOINT_PROBE(page_ref, page_ref_mod) {
        return trace_page_ref((struct pt_regs *)args, args->pfn, 
                    args->flags, args->mapping, args->count, args->val, args->count, 0);
}
TRACEPOINT_PROBE(page_ref, page_ref_mod_unless) {
        if (!args->ret)
            return 0;
        return trace_page_ref((struct pt_regs *)args, args->pfn, 
                    args->flags, args->mapping, args->count, args->val, args->count, 1);
}

TRACEPOINT_PROBE(page_ref, page_ref_set) {
        return trace_page_ref((struct pt_regs *)args, args->pfn, 
                    args->flags, args->mapping, args->count, args->val, args->count, 2);
}

TRACEPOINT_PROBE(page_ref, page_ref_mod_and_return) {
        return trace_page_ref((struct pt_regs *)args, args->pfn, 
                    args->flags, args->mapping, args->count, args->val, args->ret, 3);
}


TRACEPOINT_PROBE(page_ref, page_ref_mod_and_test) {
        return trace_page_ref((struct pt_regs *)args, args->pfn, 
                    args->flags, args->mapping, args->count, args->val, args->ret, 4);
}
/*


*/
"""

program += bpf_source_kernel
b = BPF(text=program)

print("Tracing page_ref:* → PFN → Unlimited Stack Traces... Press Ctrl+C to stop.")

total_pfn = 0
pfn_events = {}
pfn_state = {}
possible_leak_pfn = [] # collection of possible leaked pfns

def handle_event(cpu, data, size):
    event = b["events"].event(data)
    pfn = event.pfn
    global total_pfn

    if pfn not in pfn_events:
        pfn_events[pfn] = []

    if pfn not in pfn_state:
        pfn_state[pfn] = 0

    if event.count == 0:
        if pfn_events[pfn]:
            total_pfn = total_pfn - 1
        # print(f"del {pfn}")
        pfn_state[pfn] = 0 # 0 stands for unknown state
        pfn_events[pfn] = [] # Release irrelant events
        if pfn in possible_leak_pfn:
            possible_leak_pfn.remove(pfn)
    else:
        if not pfn_events[pfn]:
            pfn_state[pfn] = 0 # 0 stands for unknown state
            new_allocated = event.count - event.val
            if new_allocated == 0:
                total_pfn = total_pfn + 1
                # print(f"new {pfn}")
                pfn_events[pfn].append(event)
        else:
            pfn_events[pfn].append(event)

    if pfn_state[pfn] == 0:
        if event.mapping != 0 and event.count != 0:
            pfn_state[pfn] =1 # once used as pagecache
    elif pfn_state[pfn] == 1:
        if event.mapping == 0 and event.count > 1:
            pfn_state[pfn] = 2 # possiblely leaked
            possible_leak_pfn.append(pfn)

b["events"].open_ring_buffer(handle_event)
bpf = b

pid = -1
def xs(bpf, stack_id):
        stack_traces = bpf["stack_traces"]
        trace = []
        for addr in stack_traces.walk(stack_id):
                sym = bpf.sym(addr, pid,
                                      show_module=True,
                                      show_offset=True)
                trace.append(str(sym))
        trace = "\n\t\t".join(trace)
        print(trace)

def save_data():
    tag =  datetime.now().strftime("%Y_%m_%d_%I_%M_%S_%p")
    tag = "suse_"+tag
    dump_stack_traces("./", tag)
    dump_page_stackid("./", tag)

examples = """
EXAMPLES:

./trace_pageref
"""

description = """
Trace possiblely leaked page refefences.
"""

parser = argparse.ArgumentParser(description=description,
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=examples)

parser.add_argument("-i", "--interval", default=5, type=int,
        help="interval in seconds to print the number of possiblely leaked page refefences")

parser.add_argument("-l", "--limit", default=1000, type=int,
        help="exit once possibly leaked page refereces reaches this limit")

args = parser.parse_args()
leak_limit = args.limit
interval = args.interval

def leak_stat():
    print(f"Number of possiblely leaked pages:{len(possible_leak_pfn)}, total tracked pfns {total_pfn} / {len(pfn_state)}")

def save_stack_traces(stack_counters, filename):
    stack_traces = bpf["stack_traces"]
    f = open(filename, "w+")
    for stack_id, count in stack_counters:
        trace = ["=> %ld %ld "%(stack_id.value, count.value)]
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
"""
struct pfn_stack_entry {
    u64 pfn;
    u64 stack_id;
    int val;
    int count;
    void *mapping;
    unsigned long flags;
    int ret;
    int type;
};"""

def save_events(items, filename):
    f = open(filename, "w+")
    for pfn, events in items:
        for e in events:
            mapping = e.mapping
            if e.mapping is None:
                #code.interact(local=locals())
                mapping = 0
            #f.write(f"{pfn} {e.stack_id} {e.val} {e.count} {e.mapping} {e.flags} {e.ret} {e.type} \n")
            f.write("%d %d %d %d %d %x %x %d %d \n"%(pfn, e.stack_id, e.val, e.count, e.ts, e.mapping, e.flags, e.ret, e.type))

def save_leak(filename):
    f = open(filename, "w+")
    for pfn in possible_leak_pfn:
        f.write(f"{pfn} \n")

ts_tag = "suse_"+datetime.now().strftime("%Y_%m_%d_%I_%M_%S_%p")+"_"
def save_data():
    tag = ts_tag
    stack_counters_items = bpf["stack_counts"].items()
    save_stack_traces(stack_counters_items, "./"+tag+"stack_traces.log")
    events_items = pfn_events.items()
    save_events(events_items, "./"+tag+"pageref_events.log")
    save_leak("./"+tag+"possible_leak_page.log")

def copy_file_with_timestamp(source, destination):
    with open(source, 'r') as file:
        my_file = file.read()
    with open(destination, 'a') as file:
        ts = "=> "+datetime.now().strftime("%Y_%m_%d_%I_%M_%S_%p")+"\n"
        file.write(ts)
        file.write(my_file)

def save_meminfo():
    meminfo = ts_tag+"meminfo.log"
    copy_file_with_timestamp("/proc/meminfo", meminfo)

interval = 2
try:
    save_meminfo()
    while True:
        b.ring_buffer_poll()
        print(f"Number of possiblely leaked pages:{len(possible_leak_pfn)}, tracked {total_pfn} / total {len(pfn_state)}")
        sleep(interval)
except KeyboardInterrupt:
    print("Interrupted...")

print("Please wait...")
print("Saving data...")
#code.interact(local=locals())
#save_debug_data_from_bpf_maps()
save_meminfo()
save_data()
sys.stdout.flush()
print("Done...")
#sleep(1)
exit(0)
