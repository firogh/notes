---
tags: [ kernel ] 
layout: post
date: 2014-12-28
title:  Page frame reclaimation
category: cs
---

# GFP flags
__GFP_IO: allow disk IO
__GFP_FS: allow fs operations, depend on io.
[Avoiding memory-allocation deadlocks](https://lwn.net/Articles/594725/)
## ETC for general idea of page reclaimation from Brown
[Understanding __GFP_FS](https://lwn.net/Articles/596618/)

# Page flags
PG_reclaim:
commit 3b0db538ef6782a1e2a549c68f1605ca8d35dd7e
Author: Andrew Morton <akpm@digeo.com>
Date:   Mon Dec 2 21:31:09 2002 -0800
    [PATCH] Move reclaimable pages to the tail ofthe inactive list on
PG_swapcace means page is in the swap cache.
PG_swapbacked: stack, heap, data segment, anoymous mmap, shmem, it means page is backed by RAM or Swap. It means this page is no real file related(pagecache), reclaim this page should use swap.
PG_active: active page
PG_referenced: accessed recently
PG_lru: page is on the lru linked list
PG_mlocked: mlock()
PG_locked in generic_file_buffered_read add_to_page_cache_lru and __SetPageLocked
check mark_buffer_async_read
tglx tree
commit d58e41eec6859e9590f8f70ccdc1d58f4f6a1b84
Author: Andrew Morton <akpm@zip.com.au>
Date:   Sun May 5 01:10:37 2002 -0700
    [PATCH] Fix concurrent writepage and readpage

# Page reference results
enum page_references {
        PAGEREF_RECLAIM, 	# Try reclaim this page.
        PAGEREF_RECLAIM_CLEAN,	# Try relcaim this page, if clean.
[But you were thinking right, it is exactly what it means!  If the state is PAGEREF_RECLAIM_CLEAN, reclaim the page if it is clean:](https://lore.kernel.org/patchwork/patch/189879/)
        PAGEREF_KEEP,		# Keep this page on inactive list.
        PAGEREF_ACTIVATE,	# Move this page to active list or inevicatble.
};

# Process flags
## PF_MEMALLOC
[UVM](https://www.kernel.org/doc/gorman/html/understand/understand009.html)
[Kill PF_MEMALLOC abuse](https://lore.kernel.org/patchwork/cover/178099/)
[without being forced to write out dirty pages](https://stackoverflow.com/a/40899585/1025001)
[并不是所有进程都有的。kswapd，direct reclaim的process等在回收的时候会设置这个标志](http://bbs.chinaunix.net/thread-4078724-1-1.html)
check __perform_reclaim
## PFMEMALLOC
Example: get_page_from_freelist and __ac_get_obj
               /*
                 * page is set pfmemalloc is when ALLOC_NO_WATERMARKS was
                 * necessary to allocate the page. The expectation is
                 * that the caller is taking steps that will free more
                 * memory. The caller should avoid the page being used
                 * for !PFMEMALLOC purposes.
                 */
                if (alloc_flags & ALLOC_NO_WATERMARKS)
                        set_page_pfmemalloc(page);
commit c93bdd0e03e848555d144eb44a1f275b871a8dd5
Author: Mel Gorman <mgorman@suse.de>
Date:   Tue Jul 31 16:44:19 2012 -0700
    netvm: allow skb allocation to use PFMEMALLOC reserves
# Occasions
* Periodically Keep a halthy avaliable free pages and in interrupt context handler cannot reclaim pages.
kswapd_shrink_node
* No enough memory for a large memory alloc.
get_page_from_freelist->node_claim
try_to_free_mem_cgroup_pages
try_to_free_pages
* Manually initiate, ondemand
hibernate_preallocate_memory->shrink_all_memory
drop_caches

# Reclaimable pages
all pages of a User Mode process are reclaimable except locked.
Lokced userspace pages: 
1. Temporarily locked pages: PG_locked, 
2. Memory locked pages: VM_LOCKED, [Misunderstanding mlock](https://eklitzke.org/mlock-and-mlockall)
## Swapbacked pages
1. Private
1.1 malloc memory map
1.2 Dirty file private mapping - data, bbs segments
2. Shared - tmpfs: 
2.1 Anonymous shared mapping memory between P&C; 
2.2 System v IPC shared memory.
## Filebacked pages
1. Private: Clean file private mapping - text segment
2. Shared: Shared file mapping memory.

# LRU-2Q
page.lru and struct lruvec 
New page is inserted to head, PG_reclaim page is rotated to the tail.
## Move LRU page reclaim from zones to node
[1st RFC](https://lkml.org/lkml/2015/6/8/298)
## shrink_active_list
lru_to_page(head) (list_entry((head)->prev, struct page, lru))
## active list and inactive list
active list: working set; Am of LRU 2Q; how often
[Better active/inactive list balancing](https://lwn.net/Articles/495543/)
mark_page_accessed and called sub-functions.
active -> inactive head - list_add(&page->lru, &l_inactive); in shrink_active_list
inactive -> active head - __activate_page <- mark_page_accessed <- pagecache_get_page or generic_file_buffered_read
inactive -> inactive tail - rotate_reclaimable_page

# Second chance
commit dfc8d636cdb95f7b792d5ba8c9f3b295809c125d
Author: Johannes Weiner <hannes@cmpxchg.org>
Date:   Fri Mar 5 13:42:19 2010 -0800
    vmscan: factor out page reference checks
commit 645747462435d84c6c6a64269ed49cc3015f753d
Author: Johannes Weiner <hannes@cmpxchg.org>
Date:   Fri Mar 5 13:42:22 2010 -0800
    vmscan: detect mapped file pages used only once


[Second Chance Page Replacement Algorithm](https://www.youtube.com/watch?v=eHK749r5RGs)
[Second Chance Page Replacement Algorithms and Clock Page Replacement Algorithms](https://www.youtube.com/watch?v=DFmsm0J8joY)
core: page_check_references() in shrink_page_list().
shrink_active_list for VM_EXEC page.
core: page_referenced in shrink_active_list
mark_page_accessed
{
        inactive,referenced          ->      active,unreferenced
        {
                if !PageLRU - on the pagevec
                        mm: activate !PageLRU pages on mark_page_accessed if page is on local pagevec - 059285a25f30c13ed4f5d91cecd6094b9b20bb7b
                        SetPageActive
                else
                        activate_page
        }
        {in,}active,unreferenced        ->      {in,}active,referenced
        {
                SetPageReferenced
        }
}
#### active list -> 
if pte with PAGE_ACCESSED - referenced in page_referenced() && VM_EXEC -> keep it in active list - tail -> head - rotated
else
de-activate it to inactive list head
#### inactive list
page from tail page_check_references
{
        3 passes: 
        1. hardware pte; use and clear pte; might get reference; second chance.
        2. software flag: -> RECLAIM Clean page and clear, since dirty page is a good signal that the page was used recently because the flusher threads
                                clean pages periodically.
        3. reclaim
        hardware accessed - if accessed, won't reclaim
        {
                PAGEREF_ACTIVATE: incline to activate swapbacked page
                        PageSwapBacked
                        referenced_page 
                        referenced_ptes > 1
                        vm_flags & VM_EXEC
                PAGEREF_KEEP
        } 
        else if ?? software accessed - && dirty won't reclaim
        {
                vmscan: detect mapped file pages used only once - 645747462435d84c6c6a64269ed49cc3015f753d
                vmscan,tmpfs: treat used once pages on tmpfs as used once - 2e30244a7cc1ff09013a1238d415b4076406388e
                # From above commits: When a page has PG_referenced, shrink_page_list() discards it only if it
                # is not dirty. ... PG_dirty is a good signal that the page was used recently because
                # the flusher threads clean pages periodically.  In addition, page writeback
                # is costlier than simple page discard.
                #  Firo: not incline to reclaim dirty page.
                #  Firo: PG_referenced -> PAGEREF_RECLAIM_CLEAN
                file-backed -> PAGEREF_RECLAIM_CLEAN
                swapbacked -> PAGEREF_RECLAIM
        } else
                PAGEREF_RECLAIM
}

### Working set
mm: thrash detection-based file cache sizing - a528910e12ec7ee203095eb1711468a66b9b60b0
[Not merged - mm: refault distance-based file cache sizing](https://lkml.org/lkml/2012/5/1/51)
workingset_eviction() and workingset_refault()
* WSS
https://github.com/brendangregg/wss
http://www.10tiao.com/html/606/201807/2664605543/1.html
 
### Clock ??
[ClockProApproximation](https://linux-mm.org/ClockProApproximation)
[A CLOCK-Pro page replacement implementation](https://lwn.net/Articles/147879/)
[CLOCK-Pro: An Effective Improvement of the CLOCK Replacement](https://www.usenix.org/legacy/publications/library/proceedings/usenix05/tech/general/full_papers/jiang/jiang_html/html.html)
### Other notes on Replacement polices
replacement policies:
https://www.kernel.org/doc/gorman/html/understand/understand013.html
https://linux-mm.org/PageReplacementDesign
https://www.kernel.org/doc/gorman/html/understand/understand013.html
[PageReplacementDesign](https://linux-mm.org/PageReplacementDesign)
https://www.cnblogs.com/tolimit/p/5447448.html

# Lumpy reclaim
[mm: vmscan: Remove lumpy reclaim](https://lkml.org/lkml/2012/3/28/325)
[mm: vmscan: Remove dead code related to lumpy reclaim waiting on pages under writeback](https://lore.kernel.org/patchwork/patch/262301/)


# Steps of reclaim
## WriteBack
1. if page is alreday marked reclaim, skip it this time.
2. if not makred reclaim, normal writeback, and IO isn't permitted, then mark it as reclaim and will be pended at tail of LRU after writeback complete.
3. Cgroup cases. Just wait for the complte of writeback.
## Add to swap cache
## Unmap pte.
## Dirty page
Leave kswapd to deal with IO, mark it reclaim and skip it.
## pageout
including shared memory

#ETC Leave questions open
* activate writeback pages
mm: vmscan: move dirty pages out of the way until they're flushed - c55e8d035b28b2867e68b0e2d0eee2c0f1016b43
Fixing writeback from direct reclaim https://lwn.net/Articles/396561/


# Shrink slab caches
## double slab pressure
tglx tree
commit b65bbded3935b896d55cb6b3e420a085d3089368
Author: Andrew Morton <akpm@digeo.com>
Date:   Wed Sep 25 07:20:18 2002 -0700
    [PATCH] slab reclaim balancing

# SUSE page cache limit
Check box/pagecache-limit

# LQO
## mapping is NULL?
e2be15f6c3eecedfbe1550cca8d72c5057abbbd2
+               /* Treat this page as congested if underlying BDI is */
+               mapping = page_mapping(page);
+               if (mapping && bdi_write_congested(mapping->backing_dev_info))

