---
tags: [ kernel ] 
layout: post
date: 2019-06-08
title: Linux kernel page allocation
category: cs
---

# GFP
[Memory Allocation Guide](https://www.kernel.org/doc/html/latest/core-api/memory-allocation.html)
[Memory Allocation Controls](https://www.kernel.org/doc/html/latest/core-api/mm-api.html#memory-allocation-controls)
Also see include/linux/gfp.h
## Removed GFP flags
__GFP_WAIT: mm, page_alloc: Rename __GFP_WAIT to __GFP_RECLAIM
## GFP_ZONE_TABLE and GFP_ZONE_BAD
commit b70d94ee438b3fd9c15c7691d7a932a135c18101
Refs: v2.6.30-5489-gb70d94ee438b
Author:     Christoph Lameter <cl@linux.com>
AuthorDate: Tue Jun 16 15:32:46 2009 -0700
    page-allocator: use integer fields lookup for gfp_zone and check for errors in flags passed to the page allocator
+ * GFP_ZONE_TABLE is a word size bitstring that is used for looking up the
+ * zone to use given the lowest 4 bits of gfp_t. Entries are ZONE_SHIFT long
+ * and there are 16 of them to cover all possible combinations of
+ * __GFP_DMA, __GFP_DMA32, __GFP_MOVABLE and __GFP_HIGHMEM
+ * The zone fallback order is MOVABLE=>HIGHMEM=>NORMAL=>DMA32=>DMA.
+ * But GFP_MOVABLE is not only a zone specifier but also an allocation
+ * policy. Therefore __GFP_MOVABLE plus another zone selector is valid.
+ * Only 1bit of the lowest 3 bit (DMA,DMA32,HIGHMEM) can be set to "1".
+ *       bit       result
+ *       0x0    => NORMAL
+ *       0x1    => DMA or NORMAL
+ *       0x2    => HIGHMEM or NORMAL
+ *       0x3    => BAD (DMA+HIGHMEM)
+ *       0x4    => DMA32 or DMA or NORMAL
+ *       0x5    => BAD (DMA+DMA32)
+ *       0x6    => BAD (HIGHMEM+DMA32)
+ *       0x7    => BAD (HIGHMEM+DMA32+DMA)
+ *       0x8    => NORMAL (MOVABLE+0)
+ *       0x9    => DMA or NORMAL (MOVABLE+DMA)
+ *       0xa    => MOVABLE (Movable is valid only if HIGHMEM is set too)
+ *       0xb    => BAD (MOVABLE+HIGHMEM+DMA)
+ *       0xc    => DMA32 (MOVABLE+HIGHMEM+DMA32)
+ *       0xd    => BAD (MOVABLE+DMA32+DMA)
+ *       0xe    => BAD (MOVABLE+DMA32+HIGHMEM)
+ *       0xf    => BAD (MOVABLE+DMA32+HIGHMEM+DMA)

# Alloc flags
gfp_to_alloc_flags
ALLOC_HIGH: __zone_watermark_ok(): if (alloc_flags & ALLOC_HIGH) min -= min / 2;
ALLOC_HARDER: rmqueue(): if (alloc_flags & ALLOC_HARDER) { page = __rmqueue_smallest(zone, order, MIGRATE_HIGHATOMIC);

# PF_MEMALLOC
[Mel's book on PF_MEMALLOC](https://www.kernel.org/doc/gorman/html/understand/understand009.html)
[Kill PF_MEMALLOC abuse](https://lore.kernel.org/patchwork/cover/178099/)
 get_page_from_freelist and __ac_get_obj
                 * page is set pfmemalloc is when ALLOC_NO_WATERMARKS was
                 * necessary to allocate the page. The expectation is
                 * that the caller is taking steps that will free more
                 * memory. The caller should avoid the page being used
                 * for !PFMEMALLOC purposes.
                if (alloc_flags & ALLOC_NO_WATERMARKS)
                        set_page_pfmemalloc(page);
## Users of PF_MEMALLOC
kswapd and __alloc_pages_direct_reclaim->__perform_reclaim->Set PF_MEMALLOC.
commit c93bdd0e03e848555d144eb44a1f275b871a8dd5
Author: Mel Gorman <mgorman@suse.de>
Date:   Tue Jul 31 16:44:19 2012 -0700
    netvm: allow skb allocation to use PFMEMALLOC reserves

# PF_SWAPWRITE - swapwrite originally means swap space but now stands for kswapd or zone reclaim and migration?
[Swap Migration V4: Overview](https://lore.kernel.org/linux-mm/20051025193023.6828.89649.sendpatchset@schroedinger.engr.sgi.com/#r)
[Swap Migration V5: Overview](https://lwn.net/Articles/157936/)
commit 930d915252edda7042c944ed3c30194a2f9fe163
Refs: v2.6.15-1460-g930d915252ed
Author:     Christoph Lameter <clameter@sgi.com>
AuthorDate: Sun Jan 8 01:00:47 2006 -0800
    [PATCH] Swap Migration V5: PF_SWAPWRITE to allow writing to swap
    Add PF_SWAPWRITE to control a processes permission to write to swap.
    - Use PF_SWAPWRITE in may_write_to_queue() instead of checking for kswapd and pdflush
    - Set PF_SWAPWRITE flag for kswapd and pdflush
## Firo 
The origianl migrations code [swap_pages](https://lore.kernel.org/linux-mm/20051025193039.6828.74991.sendpatchset@schroedinger.engr.sgi.com/)
seems I can remove it from migration code since it's not used during migrating pages.
Could I remove it completely.

# Zone lists
struct zonelist node_zonelists[MAX_ZONELISTS];
 * [0]  : Zonelist with fallback
 * [1]  : No fallback (__GFP_THISNODE)
start_kernel -> build_all_zonelists
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

# LQO
[Driver porting: low-level memory allocation]https://lwn.net/Articles/22909/)
[The "too small to fail" memory-allocation rule](https://lwn.net/Articles/627419/)
[Revisiting "too small to fail"](https://lwn.net/Articles/723317/)

# High-order atomic allocations
commit 0aaa29a56e4fb0fc9e24edb649e2733a672ca099
Author: Mel Gorman <mgorman@techsingularity.net>
Date:   Fri Nov 6 16:28:37 2015 -0800
    mm, page_alloc: reserve pageblocks for high-order atomic allocations on demand

# Hot and cold pages, pcp list
[Hot and cold pages](https://lwn.net/Articles/14768/)
[mm, Remove cold parameter from free_hot_cold_page*](https://patchwork.kernel.org/patch/10013971/)

# Fair-zone allocation - obsoleted but see __GFP_WRITE
[mm, page_alloc: Remove fair zone allocation policy](https://lore.kernel.org/patchwork/patch/691300/)
[Configurable fair allocation zone policy](https://lwn.net/Articles/576778/)

# Compaction and reclamation
Direct reclaim: do_try_to_free_pages vm_event_item ALLOCSTALL
Kswapd: balance_pgdat PAGEOUTRUN

# Buddy memory system 1963 ~ 1965
[buddy system 1965 a fast storage allocator.](http://sci-hub.tw/https://dl.acm.org/citation.cfm?doid=365628.365655) 
[Buddy memory allocation](https://en.wikipedia.org/wiki/Buddy_memory_allocation)
[buddy system variants 1977](https://dl.acm.org/citation.cfm?id=359626)
The following cited from above 1965 paper.
The oporations involved in obtaining blocks from and retm'ning thom to the free
storage lists aro vory fast, making this scheme particularly appropriate for list structure operations and for other
situations involving many sizes of blocks which are fixed in size and location. This is in fact tho storago bookkeeping
mothod used in tho Boll Telephone Laboratories Low-Level List Language'
## OSIDP
Both fixed and dynamic partitioning schemes have drawbacks. A fixed partitioning
scheme limits the number of active processes and may use space inefficiently if there is
a poor match between available partition sizes and process sizes. A dynamic partition-
ing scheme is more complex to maintain and includes the overhead of compaction. An
interesting compromise is the buddy system
## Translations
free_area; page_is_buddy; PageBuddy(buddy) && page_order(buddy) 
setup_arch->x86_init.paging.pagetable_init = native_pagetable_init
        sparse_init vmemmap_populate      # vmemmap
        zone_sizes_init free_area_init_core zone_pcp_init 
        memmap_init_zone # Memory map a) Set all page to reserved. MIGRATE_MOVABLE? b) Set node, zone to page->flags; set_page_links
### Buddy init
mem_init-> memblock_free_all or free_all_bootmem # /* this will put all low memory onto the freelists */
