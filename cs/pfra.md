---
tags: [ kernel ] 
layout: post
date: 2014-12-28
title:  Page frame reclaimation
category: cs
---

# Move LRU page reclaim from zones to node
[1st RFC](https://lkml.org/lkml/2015/6/8/298)

# Lumpy reclaim
[mm: vmscan: Remove lumpy reclaim](https://lkml.org/lkml/2012/3/28/325)
[mm: vmscan: Remove dead code related to lumpy reclaim waiting on pages under writeback](https://lore.kernel.org/patchwork/patch/262301/)

# Recalimable pages
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
1. Private
1.1 Clean file private mapping - text segment
2. Shared
2.1 Shared file mapping memory.

# PF_MEMALLOC
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
    
    Change the skb allocation API to indicate RX usage and use this to fall
    back to the PFMEMALLOC reserve when needed.  SKBs allocated from the
    reserve are tagged in skb->pfmemalloc.  If an SKB is allocated from the
    reserve and the socket is later found to be unrelated to page reclaim, the
    packet is dropped so that the memory remains available for page reclaim.
    Network protocols are expected to recover from this packet loss.

# PFRA
## Leave questions open
* I saw we always shrink active list from the head, what about the tail entries, if the nr_to_scan isn't enough to reach the end of active list?
Seems this problem is apporached by isolate_lru_pages with for-loop and list_add insert and move_active_pages_to_lru ( lru - LRU_ACTIVE).
* activate writeback pages
mm: vmscan: move dirty pages out of the way until they're flushed - c55e8d035b28b2867e68b0e2d0eee2c0f1016b43

## Organization
LRU-2Q
### active list and inactive list
active list: working set; Am of LRU 2Q; how often
[Better active/inactive list balancing](https://lwn.net/Articles/495543/)
mark_page_accessed and called sub-functions.
active -> inactive head - list_add(&page->lru, &l_inactive); in shrink_active_list
inactive -> active head - __activate_page <- mark_page_accessed <- pagecache_get_page or generic_file_buffered_read
inactive -> inactive tail - rotate_reclaimable_page
## Occasions
* Periodically Keep a halthy avaliable free pages.
kswapd_shrink_node
* No enough memory for a large memory alloc.
get_page_from_freelist->node_claim
try_to_free_mem_cgroup_pages
try_to_free_pages
* Manually initiate, ondemand
hibernate_preallocate_memory->shrink_all_memory
drop_caches
## Policy
OOM
### Second chance
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

# SUSE page cache limit
Check box/pagecache-limit

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

# Page flags
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
# struct page
## page.lru
lruvec, buddy system, slab, isolate list

# ETC
commit dfc8d636cdb95f7b792d5ba8c9f3b295809c125d
Author: Johannes Weiner <hannes@cmpxchg.org>
Date:   Fri Mar 5 13:42:19 2010 -0800

    vmscan: factor out page reference checks
    
    The used-once mapped file page detection patchset.

## double slab pressure
tglx tree
commit b65bbded3935b896d55cb6b3e420a085d3089368
Author: Andrew Morton <akpm@digeo.com>
Date:   Wed Sep 25 07:20:18 2002 -0700

    [PATCH] slab reclaim balancing
