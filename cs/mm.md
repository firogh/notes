---
tags: [ kernel ] 
layout: post
date: 2014-12-28
title: Linux memory management
category: cs
---
# MM
## memory track
crash> enum zone_type
enum zone_type {
  ZONE_DMA = 0
  ZONE_DMA32 = 1
  ZONE_NORMAL = 2
  ZONE_MOVABLE = 3
  ZONE_DEVICE = 4
  __MAX_NR_ZONES = 5
};
  node_zonelists = {{
      _zonerefs = {{
          zone = 0xffff88107ffd5d80, # node 0
          zone_idx = 2
        }, {
          zone = 0xffff88107ffd56c0, # node 0
          zone_idx = 1
        }, {
          zone = 0xffff88107ffd5000, # node 0
          zone_idx = 0
        }, {
          zone = 0xffff88207ffd2d80,  # belongs to Node 1
          zone_idx = 2
        }, {
          zone = 0x0, 
          zone_idx = 0
## Memory allocation
__GFP_THISNODE: 9b819d204cf602eab1a53a9ec4b8d2ca51e02a1d - Add __GFP_THISNODE to avoid fallback to other nodes and ignore cpuset/memory policy restrictions
## Memory compaction: 
if buddy algorithm failed to allocate large memory, it will trigger memory compaction. Check /proc/buddyinfo; /proc/sys/vm/extfrag_threshold  compact 0 < > 1000 swap old page
[Memory compaction](https://lwn.net/Articles/368869/)
https://www.zhihu.com/question/59053036
## Page migration
Split the free lists for movable and unmovable allocations
MIGRATE_RECLAIMABLE: 
commit e12ba74d8ff3e2f73a583500d7095e406df4d093
Author: Mel Gorman <mel@csn.ul.ie>
    Group short-lived and reclaimable kernel allocations
## Page reclaim
### Page swap
PG_swapcace means page is in the swap cache.
PG_swapbacked means page is backed by RAM or Swap. It means this page is no real file related(pagecache), reclaim this page should use swap.

## Memory leaks
/proc/meminfo
Committed_AS and __vm_enough_memory

## Optimization Cache
### Hot and cold pages
[Hot and cold pages](https://lwn.net/Articles/14768/)
per_cpu_pageset

### Buffer cache
"Buffers" represent how much portion of RAM is dedicated to cache disk blocks
1. Open block device directly open("/dev/sdb"...) -> blkdev_open
2. Read metadata,including indirect blocks, bitmap,  sb_getblk->... -> grow_dev_page
meminfo_proc_show

# LQO
Watermark
MIGRATE_HIGHATOMIC
Fair-zone allocation: obesete in 4.x+ kernel
What about other cpu when panic
## BUG

# Reference
[Memory – Part 2: Understanding Process memory](https://techtalk.intersec.com/2013/07/memory-part-2-understanding-process-memory/)

# Page flags
PG_active: 表示此页当前是否活跃，当放到或者准备放到活动lru链表时，被置位
PG_referenced: 表示此页最近是否被访问，每次页面访问都会被置位
PG_lru: 表示此页是处于lru链表中的
PG_mlocked: 表示此页被mlock()锁在内存中，禁止换出和释放
PG_swapbacked: 表示此页依靠swap，可能是进程的匿名页(堆、栈、数据段)，匿名mmap共享内存映射，shmem共享内存映射

# struct page
* page.lru
1. lruvec
2. buddy system
3. slab
4. isolate list
* page.index
1. file memory mapped page: index is offset in a file
2. Anonymous page: linear_page_index in __page_set_anon_rmap()__

# TLB
spurious_fault:a stale TLB entry

# Kprobes
kprobes_fault

# Leave questions open
kmmio probe in do_page_fault
* pte
1216         if (error_code & PF_WRITE) {
1217                 /* write, present and write, not present: */
1218                 if (unlikely(!(vma->vm_flags & VM_WRITE)))
* madvise
* pte_present ?
* bug?
mm/memory.c: do_anonymous_page: page_add_new_anon_rmap(page, vma, vmf->address, false);
* userfd
https://www.youtube.com/watch?v=xhOBc5L_Hkk
* Special sections in Linux binaries
https://lwn.net/Articles/531148/
* COW: for malloc page 
are the ptes set during fork for child process?
check copy_one_pte is_cow_mapping in copy_page_range in dup_mmap
VM_WIPEONFORK
MADV_WIPEONFORK
* DAX
* __shmem_file_setup dev/zero
* http://pzemtsov.github.io/2016/11/06/bug-story-alignment-on-x86.html


# Data synchronization

# process address space
[flexible-mmap-2.6.7-D5][0] 
[Reorganizing the address space][1] 
[Anatomy of a Program in Memory][2] 
 
[0]: https://lwn.net/Articles/90311/ 
[1]: https://lwn.net/Articles/91829/ 
[2]: http://duartes.org/gustavo/blog/post/anatomy-of-a-program-in-memory/

# Reference
[BEFORE MEMORY WAS VIRTUAL](http://denninginstitute.com/pjd/PUBS/bvm.pdf)
[Memory part 3: Virtual Memory][1]

[1]: https://lwn.net/Articles/253361/

# Addresses
the VA is at fixed offset from the PA. They're shifted by exactly the page offset. so if your page offset is 3 GBs that means 3 GBs in VM corresponds to 0 in PM
kernel Virutal addresses are used for two different things 
this first is noncontiguous memory mappings; this is often used for large buffer on small memory system.
the other thing they're used for  is memory mapped i/o.

VM area struct represents a mapping 

# SLAB 
[The Slab Allocator: An Object-Caching Kernel Memory Allocator](https://people.eecs.berkeley.edu/~kubitron/cs194-24/hand-outs/bonwick_slab.pdf)
[Text version](https://www.usenix.org/legacy/publications/library/proceedings/bos94/full_papers/bonwick.a)
https://mp.weixin.qq.com/s/ragFsK_AJivOGjR47tAhHw
https://events.static.linuxfound.org/images/stories/pdf/klf2012_kim.pdf
type: resource
[The slab allocator has three principle aims:](https://www.kernel.org/doc/gorman/html/understand/understand011.html)
[Re: When to use kmem_cache_alloc](https://lkml.org/lkml/2000/8/7/65)
shrink attr
/sys/kernel/slab/iint_cache/shrink
SLAB_ATTR(shrink);
array_cache.entry: cache hotness.
## slub
[SLAUOB: Kernel memory allocator design and philosophy](https://www.youtube.com/watch?v=h0VMLXavx30)
[SLUB fastpath improvements and potential booster shots through bulk alloc and free](https://www.youtube.com/watch?v=s0lZzP1jOzI)
Documentation/vm/slub.txt
[SLUB: The unqueued slab allocator V6](https://lwn.net/Articles/229096/)
[The SLUB allocator](https://lwn.net/Articles/229984/)
SLUB core - 81819f0fc8285a2a5a921c019e3e3d7b6169d225
https://events.static.linuxfound.org/sites/events/files/slides/slaballocators.pdf
[linux内存源码分析 - SLUB分配器概述](https://www.cnblogs.com/tolimit/p/4654109.html)
http://www.wowotech.net/memory_management/247.html
Freeing a full slab will make it present in per cpu partial of node partial.
[Per cpu free list](https://lwn.net/Articles/454474/)
[slub: reap free slabs periodically](https://lore.kernel.org/patchwork/patch/687700/)
### Freelist vs Freelist
26:00
[关于page同时被多CPU使用](http://kouucocu.lofter.com/post/1cdb8c4b_50f6319)
## SLUB debug
[SLUB DEBUG原理](http://www.wowotech.net/memory_management/427.html)
?? Human index

# Page frame reclaiming
Free memory is low.
Occassions
Periodical: kswapd
Ondemand: not enough memory for alloc pages
Mannual trigger: drop_caches
## What pages are unreclaimable
All kernel pages are unreclaimable.
Free pages: incldued in buddy systme lists.
Reserved pages: PG_reserved
Lokced userspace pages: 
1. Temporarily locked pages: PG_locked, 
2. Memory locked pages: VM_LOCKED, [Misunderstanding mlock](https://eklitzke.org/mlock-and-mlockall)
## Recalimable pages
all pages of a User Mode process are reclaimable except locked.
Swapbacked pages
1. Private
1.1 malloc memory map
1.2 Dirty file private mapping - data, bbs segments
2. Shared - tmpfs: 
2.1 Anonymous shared mapping memory between P&C; 
2.2 System v IPC shared memory.
Filebacked pages
1. Private
1.1 Clean file private mapping - text segment
2. Shared
2.1 Shared file mapping memory.
### Rules adopted by the PFRA
1. Free the “harmless” pages first
2. LRU 2Q
3. OOM
# Page cache and i_mmap key
index
vm_pgoff

## Reclaim
### WriteBack
1. if page is alreday marked reclaim, skip it this time.
2. if not makred reclaim, normal writeback, and IO isn't permitted, then mark it as reclaim and will be pended at tail of LRU after writeback complete.
3. Cgroup cases. Just wait for the complte of writeback.
### Add to swap cache
### Unmap pte.
### Dirty page
Leave kswapd to deal with IO, mark it reclaim and skip it.
### pageout
including shared memory
### Reverse mapping
unmapping at once all page table entries of a shared pages
#### Anonymous Pages
Page + VMA => ptes
##### Stack/heap/private anonymous mapping
page.index is VFN. 
do_anonymous_page -> page_add_new_anon_rmap
add vma to anon_vma: 1. fork or 2. do_anonymous_page(2.1 forked but parent don't use vma. 2.2 exec)
in do_mmap, for anonymous mapping, MAP_PRIVATE: pgoff = addr >> PAGE_SHIFT; addr will be vm_start in mmap_region
vm_pgoff is pgoff, vm_start >> PAGE_SHIFT, in mmap_region 
#### Mapped pages
##### File mapped pages
Page and file are fixed. VMA is dynamic.
Page + VMA = Page + VMA.vm_start = Page's offset in VMA + VMA.vm_start = Virtual Address => ptes 
Page's offset in VMA = Page's offset in file - VMA's offset in file = page.index - vma.vm_pgoff
##### Mapped pages of tmpfs - anonymous shared mapping.
shmem_add_to_page_cache
pgoff is 0; ignored; in do_mmap MAP_SHARED;
page.index is vmf->pgoff in shmem_fault; vmf->pgoff is linear_page_index(vma, address)in handle_mm_fault; pgoff is offset in vma; 
#### vm_pgoff vs index
anonymous private mapping: vm_start >> PAGE_SHIFT vs VFN 
anonymous shared mapping: 0,ignored vs page's offset in vma
file mapping: vma's offset in file vs page's offset of file
不知道理解的是否相同:
对于anonymous mapping page:
Private: page.index 是page 在所属进程地址空间的虚拟地址>> PAGE_SHIT.
Shared: page.index 是page 在所属的VMA内的offset: (page's addr - vm_start)>>PAGE_SHIT.

# Writeback 
bdi_wq = alloc_workqueue("writeback", WQ_MEM_RECLAIM | WQ_FREEZABLE 
root        40  0.0  0.0      0     0 ?        I<   10:08   0:00 [writeback]
bdi_list
bdi_register_owner(bdi, disk_to_dev(disk));
# Contents
Logic gates: SRAM, DRAM
What is data/contrl/addr bus?
What problems we will meet in memory management?
Memory allocation and release. Bootmem and Buddy system is pretty good. Can we eliminate Bootmem?
How to distribute these memory to processes? Virtual memory.
How to translate linear address to physical address? Page table.
Exchange data between primary memory and second memory. Paging.

What are the pitfall of manuplate directly physcial memory
# Memory pagge cache and buffer cache.
page cache for memory, buffer cache for fs(block size is dependent on filesystem).
address_pace->page_tree: why radix tree, read ahead, 
do_page_fault->read_page

For buffer cache: buffer cache is only a wrapper of page for fs operations. 
buffer_head is temporary data released in a deeper function than the function allocing the buffer_head.
__block_write_full_page
block_read_full_page

I can not cover every corner of kernel, so If need, I will learn it.
[The coherency problem, fs-writeback](https://lwn.net/Kernel/Index/#Memory_management-Writeback)
[Flushing out pdflush](https://lwn.net/Articles/326552/)

[Swapping](https://lwn.net/Kernel/Index/#Memory_management-Swapping)
User space process: anonymous mapping(stack,heap,mmap), IPC-share memory(anonymous?), private mapping

[Shrink cache](https://lwn.net/Kernel/Index/#Memory_management-Shrinkers)
LRU cache


# page
[An introduction to compound pages](https://lwn.net/Articles/619514/)
# Memroy mangement
## GFP flags
__GFP_IO: allow disk IO
__GFP_FS: allow fs operations, depend on io.
more details in lwn, lkd

# Virtual memory
vm的提出是为了解决。easy to use。
1. decoupling physical memory 符号集合。programmer 不需要关注底层细节。 任务转给操作系统。
2. VM相对物理内存增加了表达能力， 有了更多表达符号。着减少了swap or 不必要的页表抖动。
## Vmalloc
may sleep.
### Hwo Vmalloc works? 
Work in HIGHMEM and NORMALMEM
The skeleton is rbtree, root is global variable vmap_area_root.rb_node.
struct vm_struct likes struct address_space, functionlly;
struct vmap_area likes struct vm_area_struct.
map_vm_area 页表映射
the page in ZONE_NORMAL will not use directly mapping pfn address! It use VMALLOC address! 

##Process virtual memory
* struct vm_area_struct: The intervals of legal address are called *memory areas* is permitted to access.
* struct address_space: To establish an association between the regions of the vm and the places where the related data are located.
i_mmap: how many processes opened this file.
https://lkml.org/lkml/2012/8/7/46
* sturct mm_struct: how many files(vm_area_struct) does this process opened.

* Memory mappings
syscall remap_file_pages Nolinear mappings is deprecated, since Linux 3.16

* link 
a virtual address and physical address. --page tale
a memory region of a process and its virtual page addresses. --vm_area_struct
a region of file(one physical) and all virtual address spaces(many virtual) into which the region is mapped. address_space->i_mmap.
a physical page and the processes that share the page(used in swap case) 

#Physical memory
* NUMA/UMA pg_data_t: My PC is UMA, numatop, numastat, numactl
* ZONE(DMA/NORMAL/HIGHMEM) struct zone:
* struct page is the basic unit of kernel mm knowns as page frame.
	The goal of strcut page is to describe physical memory, not the data contained therein.
* The buddy system is per-zone struct free_area
* Physical address is connected to Virtual address by pfn = page - mem_map;
## page allocator
alloc_pages()
##Page/buffer cache
struct address_space->page_tree
##Page writeback
data_synchronization.log

#FAQ
* Where is Per-CPU variable?
static Per-CPU in .data(?) below high_memory!
runtime Per-CPU, it's GFP_KERNEL in pcpu_create_chunk()

* Memory mode
flat mem -> uma
discontig -> NUMA
sparse -> Hotplug + NUMA

* When does kernel alloc these struct pages in x86_64?
http://lwn.net/Articles/229670/
vmemmap silimar to memmap

* When kmap_atomic() BUG_ON effect?

* How cpu resolve address below high_memory?
Cpu-spicific!
x86 used page table to all address!
Mips cpu can be aware of this address!

* How to deal with useless page? : > /home/firo/bigdata

* pfmemalloc -- skb 表示申请了紧急内存!
page free

* Compound pages
18fa11efc279c20af5eefff2bbe814ca067
https://www.spinics.net/lists/newbies/msg41159.html
https://lwn.net/Articles/619514/
# Memory initialization onset:  
先从bios 拿信息 main -> detect_memory save in boot_params.e820_map
之后real -> protected -> long mode
启动 protected? mode. [What does protected mode mean](http://www.delorie.com/djgpp/doc/ug/basics/protected.html)
setup_arch
setup_memory_map -> default_machine_specific_memory_setup // Save into struct e820map e820; from boot_params.e820_map. That's all.
max_pfn = e820_end_of_ram_pfn(); // max_pfn  BIOS-e820: mem 0x0000000100000000-0x00000003227fffff usable and last_pfn = 0x322800(12840MB), so last_pfn is invalid address, use it with <.
mtrr update max_pfn, see [Processor supplementary capability](https://en.wikipedia.org/wiki/Processor_supplementary_capability)
trim_low_memory_range // reserve 64k
max_low_pfn = e820_end_of_low_ram_pfn(); //4GB以下的end of block
memblock_x86_fill// copy e820 to memblock, reconstructs direct memory mapping and setups the direct mapping of the physical memory at PAGE_OFFSET
early_trap_pf_init //  X86_TRAP_PF, page_fault) => do_page_fault
init_mem_mapping //set page table and cr3.
initmem_init ; NUMA init
x86_init.paging.pagetable_init();= paging_init //x86_64 ->zone_sizes_init->...free_area_init_core
a little history e820_register_active_region replaced by lmb [replaced by](https://lkml.org/lkml/2010/7/13/68) memblock
reserve_initrd ; // RAMDISK
总结下, 内存初始化需要的基础.
1. e820 get memory region.
2. set PF trap do_page_fault.
3. set page table and cr3.
这就完了. 之后开始开始加工.
# Setting up arch specific and creating memblock
## Arch specific x86_64
setup_arch->x86_init.paging.pagetable_init = native_pagetable_init = paging_init -> 
## Memblock
memblock the [implementations](https://0xax.gitbooks.io/linux-insides/content/mm/linux-mm-1.html) of memblock is quite simple. static initialization with variable memblock.
bootmem is discarded by [ARM](https://lkml.org/lkml/2015/12/21/333) and x86

# Using memblock to prepare for buddy system
## Zones and free_area.free_list
paging_init->zone_sizes_init.
{
	free_area_init_node-> 
## prsent_pages
		calculate_node_totalpages
### mem_map/page array:
		// mem_map for FLAT, but not for us because we use sparsemem
		alloc_node_mem_map
		free_area_init_core
		{
### managed_pages
			zone->managed_pages = zone->present_pages - memmap_pages - DMA?dma_reserve:0
			// init percpu pageset with boot_pageset
			zone_pcp_init 
			// free_area.free_list
			init_currently_empty_zone(zone, zone_start_pfn, size);
			// Set all page to reserved. MIGRATE_MOVABLE?
			// Set node, zone to page->flags; set_page_links
			memmap_init_zone 

		}
}
## Set fallback for every zone and init PCP
build_all_zonelists // dmesg
### Precondition

## Sparse 
paging_init->sparse_init

# Mirgate memory from memblock to buddy system
## Preconditions
mem_map/page array
## memblock (constantly Y for x86)
memblock_free_late->__memblock_free_late->__free_pages_bootmem
## bootmem (discarded by x86)
memblock_free_late->free_bootmem_late->__free_pages_bootmem
free_all_bootmem->free_all_bootmem_core->__free_pages_bootmem
## nobootmem
free_bootmem_late->__free_pages_bootmem
free_all_bootmem->free_low_memory_core_early->__free_memory_core->*__free_pages_memory*->__free_pages_bootmem->__free_pages_boot_core
## free bootmem core/earyly
mm_init->mem_init->free_all_bootmem
## free bootmem late
start_kernel->efi_free_boot_services->free_bootmem_late->__free_pages_bootmem

# Zone watermarks 
core_initcall(init_per_zone_wmark_min)

build_all_zonelists: Just init zones, nothing else. But we have vm_total_pages/zone->managed_pages initialized in free_all_bootmem();.
page_alloc_init // drain percpu pageset when cpu dead or dead frozen for CPU hotplug

# Madv
## MADV_SEQUENTIAL and reclaim
mm: more likely reclaim MADV_SEQUENTIAL mappings - 4917e5d0499b5ae7b26b56fccaefddf9aec9369c
