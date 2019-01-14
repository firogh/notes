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


# Reverse mapping
unmapping at once all page table entries of a shared pages
## page.index
1. file memory mapped page: index is offset in a file
2. Anonymous page: linear_page_index in __page_set_anon_rmap()__
## Anonymous Pages
Page + VMA => ptes
### Stack/heap/private anonymous mapping
page.index is VFN. 
do_anonymous_page -> page_add_new_anon_rmap
add vma to anon_vma: 1. fork or 2. do_anonymous_page(2.1 forked but parent don't use vma. 2.2 exec)
in do_mmap, for anonymous mapping, MAP_PRIVATE: pgoff = addr >> PAGE_SHIFT; addr will be vm_start in mmap_region
vm_pgoff is pgoff, vm_start >> PAGE_SHIFT, in mmap_region 
## Mapped pages 
### File mapped pages
Page and file are fixed. VMA is dynamic.
Page + VMA = Page + VMA.vm_start = Page's offset in VMA + VMA.vm_start = Virtual Address => ptes 
Page's offset in VMA = Page's offset in file - VMA's offset in file = page.index - vma.vm_pgoff
### Mapped pages of tmpfs - anonymous shared mapping.
shmem_add_to_page_cache
pgoff is 0; ignored; in do_mmap MAP_SHARED;
page.index is vmf->pgoff in shmem_fault; vmf->pgoff is linear_page_index(vma, address)in handle_mm_fault; pgoff is offset in vma; 
## vm_pgoff vs index
anonymous private mapping: vm_start >> PAGE_SHIFT vs VFN 
anonymous shared mapping: 0,ignored vs page's offset in vma
file mapping: vma's offset in file vs page's offset of file
不知道理解的是否相同:
对于anonymous mapping page:
Private: page.index 是page 在所属进程地址空间的虚拟地址>> PAGE_SHIT.
Shared: page.index 是page 在所属的VMA内的offset: (page's addr - vm_start)>>PAGE_SHIT.
## anon page: 
rbtree-based Interval tree
[逆向映射的演进](http://www.wowotech.net/memory_management/reverse_mapping.html)
exec : exec -> do_execveat_common-> bprm_mm_init setup_arg_pages
do_anonymous_page -> anon_vma_prepare & page_add_new_anon_rmap
fork workflow:attach to parent; alloc anon_vma for ownself.
anon_vma_fork() -> anon_vma_chain_link: node: avc, key: avmc.vma->start/last_pgoff page.mapping=anon_vma.rb_root
rmap: add exclusively owned pages to the newest anon_vma - e8a03feb54ca7f1768bbdc2b491f9ef654e6d01d
nuclus:
rmap_walk_anon()
## file page:
rbtree-based interval tree - INTERVAL_TREE_DEFINE in mm/interval_tree.c
__vma_adjust -> __vma_link_file: node: vma.shared.rb, key: vma.start_pgoff and vma.last_pgoff  address_space->i_mmap
address_space is premise
rmap_walk_file(page.mapping=address_space.i_mmap 

# Page flags
PG_swapcace means page is in the swap cache.
PG_swapbacked: stack, heap, data segment, anoymous mmap, shmem, it means page is backed by RAM or Swap. It means this page is no real file related(pagecache), reclaim this page should use swap.
PG_active: active page
PG_referenced: accessed recently
PG_lru: page is on the lru linked list
PG_mlocked: mlock()
# struct page
## page.lru
lruvec, buddy system, slab, isolate list
