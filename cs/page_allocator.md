---
tags: [ kernel ] 
layout: post
date: 2019-06-08
title: Linux kernel page allocation
category: cs
---

# Reference
[Driver porting: low-level memory allocation]https://lwn.net/Articles/22909/)
[The "too small to fail" memory-allocation rule](https://lwn.net/Articles/627419/)
[Revisiting "too small to fail"](https://lwn.net/Articles/723317/)

# track: buddy memory system 1963 ~ 1965
[buddy system 1965 a fast storage allocator.](http://sci-hub.tw/https://dl.acm.org/citation.cfm?doid=365628.365655) 
[Buddy memory allocation](https://en.wikipedia.org/wiki/Buddy_memory_allocation)
[buddy system variants 1977](https://dl.acm.org/citation.cfm?id=359626)
The following cited from above 1965 paper.
The oporations involved in obtaining blocks from and retm'ning thom to the free
storage lists aro vory fast, making this scheme particularly appropriate for list structure operations and for other
situations involving many sizes of blocks which are fixed in size and location. This is in fact tho storago bookkeeping
mothod used in tho Boll Telephone Laboratories Low-Level List Language'

OSIDP
Both fixed and dynamic partitioning schemes have drawbacks. A fixed partitioning
scheme limits the number of active processes and may use space inefficiently if there is
a poor match between available partition sizes and process sizes. A dynamic partition-
ing scheme is more complex to maintain and includes the overhead of compaction. An
interesting compromise is the buddy system

## Related code
free_area; page_is_buddy; PageBuddy(buddy) && page_order(buddy) 
setup_arch->x86_init.paging.pagetable_init = native_pagetable_init = paging_init ->
        sparse_init ...-> vmemmap_populate      # vmemmap
        zone_sizes_init->free_area_init_nodes -> free_area_init_node-> free_area_init_core
                zone_pcp_init # init percpu pageset with boot_pageset
                init_currently_empty_zone(zone, zone_start_pfn, size); # free_area.free_list
                memmap_init_zone # Memory map a) Set all page to reserved. MIGRATE_MOVABLE? b) Set node, zone to page->flags; set_page_links

start_kernel->mm_init
        mem_init-> memblock_free_all or free_all_bootmem # /* this will put all low memory onto the freelists */

# CPU set
[Cpusets and memory policies](https://lwn.net/Articles/717797/)
# GFP flags
__GFP_THISNODE: 9b819d204cf602eab1a53a9ec4b8d2ca51e02a1d Add __GFP_THISNODE to avoid fallback to other nodes and ignore cpuset/memory policy restrictions
__GFP_HIGHMEM in __alloc_zeroed_user_highpage??

# Alloc flags
gfp_to_alloc_flags
ALLOC_HIGH: __zone_watermark_ok(): if (alloc_flags & ALLOC_HIGH) min -= min / 2;
ALLOC_HARDER: rmqueue(): if (alloc_flags & ALLOC_HARDER) { page = __rmqueue_smallest(zone, order, MIGRATE_HIGHATOMIC);

# high-order atomic allocations
commit 0aaa29a56e4fb0fc9e24edb649e2733a672ca099
Author: Mel Gorman <mgorman@techsingularity.net>
Date:   Fri Nov 6 16:28:37 2015 -0800
    mm, page_alloc: reserve pageblocks for high-order atomic allocations on demand

# Hot and cold pages
[Hot and cold pages](https://lwn.net/Articles/14768/)
[mm, Remove cold parameter from free_hot_cold_page*](https://patchwork.kernel.org/patch/10013971/)


# Do anonymous page, zero page
tglx: commit 382a7dec462a90ad6ae01227f1e8758ae721f6ed
Author: Christoph Lameter <clameter@sgi.com>
Date:   Tue Feb 1 16:34:17 2005 -0800
    [PATCH] alloc_zeroed_user_highpage() to fix the clear_user_highpage issue
Lost tracks
commit 6fbaac38b85e4bd3936b882392e3a9b45e8acb46
Author: Linus Torvalds <torvalds@athlon.transmeta.com>
Date:   Mon Feb 4 19:14:54 2002 -0800
    v2.4.7 -> v2.4.7.1
# Obselete feature - Fair-zone allocation
[mm, page_alloc: Remove fair zone allocation policy](https://lore.kernel.org/patchwork/patch/691300/)
[Configurable fair allocation zone policy](https://lwn.net/Articles/576778/)

# Watermarks
totalreserve_pages is wmark high; check calculate_totalreserve_pages and Documentation/sysctl/vm.txt 

Check Documentation/sysctl/vm.txt for min_free_kbytes
min_free_kbytes_sysctl_handler or watermark_scale_factor_sysctl_handler or
core_initcall(init_per_zone_wmark_min) ->
        setup_per_zone_wmarks-> __setup_per_zone_wmarks
{
        firo@linux-6qg8:~> grep managed /proc/zoneinfo
                managed  3973
                managed  464142
                managed  7726451
        >>> 3973 + 464142 + 7726451
        8194566
        firo@linux-6qg8:~> cat /proc/sys/vm/min_free_kbytes
        67584
        >>> 67584 / 4 * 3973 / 8194566
        8
        # Unit of watermark is Page.    
        WMARK_MIN = page_no(min_free_kbytes) * (zone.managed_pages / \Sum of zone.managed_pages)
        WMARK_LOW = 1.25 * min or min + 1/1000 * zone.managed_pages
        WMARK_HIGH = 1.5 * min or min + 2/1000 * zone.managed_pages
}
if min_free_kbytes > 1/250*total mamanged_pages, we use 1.25 min or 1.5 min
low for normal path
min for slow path

if low is reached (i.e. slow path) wakeup __GFP_KSWAPD_RECLAIM
# vm_event_item
commit e177ea28e7eded3490174487c81e5bef8a2c4d95
Author:     Andrew Morton <akpm@zip.com.au>
AuthorDate: Thu Jul 18 21:09:17 2002 -0700
Commit:     Linus Torvalds <torvalds@home.transmeta.com>
CommitDate: Thu Jul 18 21:09:17 2002 -0700
    [PATCH] VM instrumentation
    A patch from Rik which adds some operational statitics to the VM.
    In /proc/meminfo:
    PageTables:     Amount of memory used for process pagetables
    PteChainTot:    Amount of memory allocated for pte_chain objects
    PteChainUsed:   Amount of memory currently in use for pte chains.
    In /proc/stat:
    pageallocs:     Number of pages allocated in the page allocator
    pagefrees:      Number of pages returned to the page allocator
                    (These can be used to measure the allocation rate)
    pageactiv:      Number of pages activated (moved to the active list)
    pagedeact:      Number of pages deactivated (moved to the inactive list)
    pagefault:      Total pagefaults
    majorfault:     Major pagefaults
    pagescan:       Number of pages which shrink_cache looked at
    pagesteal:      Number of pages which shrink_cache freed
    pageoutrun:     Number of calls to try_to_free_pages()
    allocstall:     Number of calls to balance_classzone()		# direct relaim do_try_to_free_pages

# Root cause for reclaim
Node: 

# Priority
Smantics in PLKA
commit 9e3b2f8cd340e13353a44c9a34caef2848131ed7
Refs: u3.4-8606-g9e3b2f8cd340
Author:     Konstantin Khlebnikov <khlebnikov@openvz.org>
AuthorDate: Tue May 29 15:06:57 2012 -0700
Commit:     Linus Torvalds <torvalds@linux-foundation.org>
CommitDate: Tue May 29 16:22:26 2012 -0700
    mm/vmscan: store "priority" in struct scan_control
DEF_PRIORITY vs NODE_RECLAIM_PRIORITY(old ZONE_RECLAIM_PRIORITY)

# Scan control: may_unmap vs may_swap
a6dc60f8975a ("vmscan: rename sc.may_swap to may_unmap")
2e2e42598908 ("vmscan,memcg: reintroduce sc->may_swap") reintroduced .may_swap for memory controller.
See RECLAIM_UNMAP for understanding may_unmap

# Node(old zone) reclaim
See zone_reclaim_mode in Documentation/admin-guide/sysctl/vm.rst
commit 9eeff2395e3cfd05c9b2e6074ff943a34b0c5c21
Refs: v2.6.16-rc1-169-g9eeff2395e3c
Author:     Christoph Lameter <clameter@sgi.com>
AuthorDate: Wed Jan 18 17:42:31 2006 -0800
Commit:     Linus Torvalds <torvalds@g5.osdl.org>
CommitDate: Wed Jan 18 19:20:17 2006 -0800
    [PATCH] Zone reclaim: Reclaim logic
[...]
commit 9eeff2395e3cfd05c9b2e6074ff943a34b0c5c21
Refs: v2.6.16-rc1-169-g9eeff2395e3c
Author:     Christoph Lameter <clameter@sgi.com>
AuthorDate: Wed Jan 18 17:42:31 2006 -0800
Commit:     Linus Torvalds <torvalds@g5.osdl.org>
CommitDate: Wed Jan 18 19:20:17 2006 -0800
    [PATCH] Zone reclaim: Reclaim logic
    Some bits for zone reclaim exists in 2.6.15 but they are not usable.  This
    patch fixes them up, removes unused code and makes zone reclaim usable.
    Zone reclaim allows the reclaiming of pages from a zone if the number of
    free pages falls below the watermarks even if other zones still have enough
    pages available.  Zone reclaim is of particular importance for NUMA
    machines.  It can be more beneficial to reclaim a page than taking the
    performance penalties that come with allocating a page on a remote zone.
    Zone reclaim is enabled if the maximum distance to another node is higher
    than RECLAIM_DISTANCE, which may be defined by an arch.  By default
    RECLAIM_DISTANCE is 20.  20 is the distance to another node in the same
    component (enclosure or motherboard) on IA64.  The meaning of the NUMA
    distance information seems to vary by arch.
## RECLAIM_WRITE and RECLAIM_SWAP
caching effect vs data locality
commit 1b2ffb7896ad46067f5b9ebf7de1891d74a4cdef
Refs: v2.6.16-rc1-635-g1b2ffb7896ad
Author:     Christoph Lameter <clameter@engr.sgi.com>
AuthorDate: Wed Feb 1 03:05:34 2006 -0800
Commit:     Linus Torvalds <torvalds@g5.osdl.org>
CommitDate: Wed Feb 1 08:53:16 2006 -0800
    [PATCH] Zone reclaim: Allow modification of zone reclaim behavior
    In some situations one may want zone_reclaim to behave differently.  For
    example a process writing large amounts of memory will spew unto other nodes
    to cache the writes if many pages in a zone become dirty.  This may impact the
    performance of processes running on other nodes.
    Allowing writes during reclaim puts a stop to that behavior and throttles the
    process by restricting the pages to the local zone.
    Similarly one may want to contain processes to local memory by enabling
    regular swap behavior during zone_reclaim.  Off node memory allocation can
    then be controlled through memory policies and cpusets.

## RECLAIM_UNMAP
commit 95bbc0c7210a7397fec1cd219f896ca95bf29e3e
Refs: v4.1-3341-g95bbc0c7210a
Author:     Zhihui Zhang <zzhsuny@gmail.com>
AuthorDate: Wed Jun 24 16:56:42 2015 -0700
Commit:     Linus Torvalds <torvalds@linux-foundation.org>
CommitDate: Wed Jun 24 17:49:42 2015 -0700
    mm: rename RECLAIM_SWAP to RECLAIM_UNMAP
    The name SWAP implies that we are dealing with anonymous pages only.  In
    fact, the original patch that introduced the min_unmapped_ratio logic
    was to fix an issue related to file pages.  Rename it to RECLAIM_UNMAP
    to match what does.
    Historically, commit a6dc60f8975a ("vmscan: rename sc.may_swap to
    may_unmap") renamed .may_swap to .may_unmap, leaving RECLAIM_SWAP
    behind.  commit 2e2e42598908 ("vmscan,memcg: reintroduce sc->may_swap")
    reintroduced .may_swap for memory controller.

## RECLAIM_SLAB(deleted)
commit 2a16e3f4b0c408b9e50297d2ec27e295d490267a
Refs: v2.6.16-rc1-636-g2a16e3f4b0c4
Author:     Christoph Lameter <clameter@engr.sgi.com>
AuthorDate: Wed Feb 1 03:05:35 2006 -0800
Commit:     Linus Torvalds <torvalds@g5.osdl.org>
CommitDate: Wed Feb 1 08:53:16 2006 -0800
    [PATCH] Reclaim slab during zone reclaim
    If large amounts of zone memory are used by empty slabs then zone_reclaim
    becomes uneffective.  This patch shakes the slab a bit.
    The problem with this patch is that the slab reclaim is not containable to a
    zone.  Thus slab reclaim may affect the whole system and be extremely slow.
    This also means that we cannot determine how many pages were freed in this
    zone.  Thus we need to go off node for at least one allocation.
    The functionality is disabled by default.
    We could modify the shrinkers to take a zone parameter but that would be quite
    invasive.  Better ideas are welcome.

## min_slab_ratio
commit 0ff38490c836dc379ff7ec45b10a15a662f4e5f6
Author: Christoph Lameter <clameter@sgi.com>
Date:   Mon Sep 25 23:31:52 2006 -0700
    [PATCH] zone_reclaim: dynamic slab reclaim

## RECLAIM_UNMAP

# Direct reclaim
do_try_to_free_pages
vm_event_item ALLOCSTALL

# Kswapd
balance_pgdat PAGEOUTRUN

# PF_SWAPWRITE - better name RECLAIMWRITE
So what does swap mean? See [Swap Migration V4: Overview](https://lore.kernel.org/linux-mm/20051025193023.6828.89649.sendpatchset@schroedinger.engr.sgi.com/#r)
[Swap Migration V5: Overview](https://lwn.net/Articles/157936/)
commit 930d915252edda7042c944ed3c30194a2f9fe163
Refs: v2.6.15-1460-g930d915252ed
Author:     Christoph Lameter <clameter@sgi.com>
AuthorDate: Sun Jan 8 01:00:47 2006 -0800
Commit:     Linus Torvalds <torvalds@g5.osdl.org>
CommitDate: Sun Jan 8 20:12:41 2006 -0800
    [PATCH] Swap Migration V5: PF_SWAPWRITE to allow writing to swap
    Add PF_SWAPWRITE to control a processes permission to write to swap.
    - Use PF_SWAPWRITE in may_write_to_queue() instead of checking for kswapd
      and pdflush
    - Set PF_SWAPWRITE flag for kswapd and pdflush

# memory cgroup
set_memcg_congestion

# Zone
## Low memory reserved
Check Documentation/sysctl/vm.txt for lowmem_reserve_ratio
lowmem_reserve_ratio_sysctl_handler or core_initcall(init_per_zone_wmark_min) ->
        setup_per_zone_lowmem_reserve
firo@linux-6qg8:~> cat /proc/zoneinfo | grep protection
        protection: (0, 1813, 31994, 31994, 31994)
        protection: (0, 0, 30181, 30181, 30181)
        protection: (0, 0, 0, 0, 0)
        protection: (0, 0, 0, 0, 0)
        protection: (0, 0, 0, 0, 0)
Check __alloc_pages_nodemask(), lowmem_reserve is used for checking if there is enough pages in current zone to which allcation fallbacks from a prefered zone.
lowmem_reserv is used for fallback allcations from a perfered zone in the zonelist.
The index of lowmme_reserv is the prefered zoneref.zone_idx
## Zone lists
struct zonelist node_zonelists[MAX_ZONELISTS];
 * [0]  : Zonelist with fallback
 * [1]  : No fallback (__GFP_THISNODE)
start_kernel->
        build_all_zonelists
or hotpulg or /proc/sys/vm/numa_zonelist_order: numa_zonelist_order_handler
  node_zonelists = {{				# Fallback zones: this zonelist including all zones from all nodes.
      _zonerefs = {{
          zone = 0xffff88107ffd5d80, # node 0
          zone_idx = 2
          zone = 0xffff88107ffd56c0, # node 0
          zone_idx = 1
          zone = 0xffff88107ffd5000, # node 0
          zone_idx = 0
          zone = 0xffff88207ffd2d80, # Node 1; fallback.
          zone_idx = 2
          zone = 0x0,
          zone_idx = 0
	...}}}
	node_zonelists[1]			# Nofallback zones
# Material
## _refcount
after __alloc_pages_nodemask _refcount=0,
post_alloc_hook: init should be 0.
but set_page_refcounted -> set_page_count(page, 1);
and Check put_page

# PFMEMALLOC
[UVM](https://www.kernel.org/doc/gorman/html/understand/understand009.html)
[Kill PF_MEMALLOC abuse](https://lore.kernel.org/patchwork/cover/178099/)
 get_page_from_freelist and __ac_get_obj
                 * page is set pfmemalloc is when ALLOC_NO_WATERMARKS was
                 * necessary to allocate the page. The expectation is
                 * that the caller is taking steps that will free more
                 * memory. The caller should avoid the page being used
                 * for !PFMEMALLOC purposes.
                if (alloc_flags & ALLOC_NO_WATERMARKS)
                        set_page_pfmemalloc(page);
[without being forced to write out dirty pages](https://stackoverflow.com/a/40899585/1025001)
[并不是所有进程都有的。kswapd，direct reclaim的process等在回收的时候会设置这个标志](http://bbs.chinaunix.net/thread-4078724-1-1.html)
check __perform_reclaim and memalloc_noreclaim_save
kswapd-> PF_MEMALLOC-> try to pageout some pages ->  allocates some page using alloc_page for pageout -> no memory in lowreserve -> goto slowpath
        /* Avoid recursion of direct reclaim */
        if (current->flags & PF_MEMALLOC)
                goto nopage;
otherwise
        __alloc_pages_direct_reclaim->__perform_reclaim->Set PF_MEMALLOC.
__gfp_pfmemalloc_flags
                if (current->flags & PF_MEMALLOC)
                        return ALLOC_NO_WATERMARKS;
commit c93bdd0e03e848555d144eb44a1f275b871a8dd5
Author: Mel Gorman <mgorman@suse.de>
Date:   Tue Jul 31 16:44:19 2012 -0700
    netvm: allow skb allocation to use PFMEMALLOC reserves

# Expected page state
page_expected_state and check_new_page, page_mapcount_reset, prep_new_page
