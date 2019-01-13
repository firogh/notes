---
tags: [ kernel ] 
layout: post
date: 2014-12-28
title: Linux memory management
category: cs
---
# memory organization
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

## Memory mode
flat mem -> uma
discontig -> NUMA
sparse -> Hotplug + NUMA
## Memory initialization onset:  
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
## Arch specific x86_64
setup_arch->x86_init.paging.pagetable_init = native_pagetable_init = paging_init -> 
## Memblock
memblock the [implementations](https://0xax.gitbooks.io/linux-insides/content/mm/linux-mm-1.html) of memblock is quite simple. static initialization with variable memblock.
bootmem is discarded by [ARM](https://lkml.org/lkml/2015/12/21/333) and x86
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
## Memory initialization from memblock to buddy system
### memblock (constantly Y for x86)
memblock_free_late->__memblock_free_late->__free_pages_bootmem
### bootmem (discarded by x86)
memblock_free_late->free_bootmem_late->__free_pages_bootmem
free_all_bootmem->free_all_bootmem_core->__free_pages_bootmem
### nobootmem
free_bootmem_late->__free_pages_bootmem
free_all_bootmem->free_low_memory_core_early->__free_memory_core->*__free_pages_memory*->__free_pages_bootmem->__free_pages_boot_core
### free bootmem core/earyly
mm_init->mem_init->free_all_bootmem
### free bootmem late
start_kernel->efi_free_boot_services->free_bootmem_late->__free_pages_bootmem
## Sparse 
paging_init->sparse_init
# Page migration
Split the free lists for movable and unmovable allocations
MIGRATE_RECLAIMABLE: 
commit e12ba74d8ff3e2f73a583500d7095e406df4d093
Author: Mel Gorman <mel@csn.ul.ie>
    Group short-lived and reclaimable kernel allocations

# ETC
## Buffer cache of command 'free'
"Buffers" represent how much portion of RAM is dedicated to cache disk blocks
1. Open block device directly open("/dev/sdb"...) -> blkdev_open
2. Read metadata,including indirect blocks, bitmap,  sb_getblk->... -> grow_dev_page
meminfo_proc_show

# LQO
[An introduction to compound pages](https://lwn.net/Articles/619514/)
Compound pages
18fa11efc279c20af5eefff2bbe814ca067
https://www.spinics.net/lists/newbies/msg41159.html
https://lwn.net/Articles/619514/
Watermark
MIGRATE_HIGHATOMIC
Fair-zone allocation: obesete in 4.x+ kernel
What about other cpu when panic
kmmio probe in do_page_fault
userfd https://www.youtube.com/watch?v=xhOBc5L_Hkk
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
## BUG

# Reference
[Memory – Part 2: Understanding Process memory](https://techtalk.intersec.com/2013/07/memory-part-2-understanding-process-memory/)
# VM
[flexible-mmap-2.6.7-D5](https://lwn.net/Articles/90311/)
[Reorganizing the address space](https://lwn.net/Articles/91829/)
[Anatomy of a Program in Memory](http://duartes.org/gustavo/blog/post/anatomy-of-a-program-in-memory/)
[BEFORE MEMORY WAS VIRTUAL](http://denninginstitute.com/pjd/PUBS/bvm.pdf)
[Memory part 3: Virtual Memory](https://lwn.net/Articles/253361/)

# Hardware memory
Logic gates: SRAM, DRAM
What is data/contrl/address bus?

# Interface
## MADV_SEQUENTIAL and reclaim
mm: more likely reclaim MADV_SEQUENTIAL mappings - 4917e5d0499b5ae7b26b56fccaefddf9aec9369c

# Linux tips
## Page flags
PG_active: active page
PG_referenced: accessed recently
PG_lru: page is on the lru linked list
PG_mlocked: mlock()
PG_swapbacked: stack, heap, data segment, anoymous mmap, shmem
## struct page
### page.lru
lruvec, buddy system, slab, isolate list
### page.index
1. file memory mapped page: index is offset in a file
2. Anonymous page: linear_page_index in __page_set_anon_rmap()__

# Pages can be isolated and migrated
Pages on LRU lists
Pages marked with PageMovable “flag”
Currently just zsmalloc (used by zram and zswap) and virtio balloon pages
[Making kernel pages movable](https://lwn.net/Articles/650917/)
