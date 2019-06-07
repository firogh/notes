---
tags: [ kernel ]
layout: post
date: 2014-12-28
title: Linux memory management
category: cs
---

# Ref
[An Evolutionary Study of Linux Memory Management for Fun and Profit](https://www.usenix.org/system/files/conference/atc16/atc16_paper-huang.pdf)

# memory organization
## Memory mode
flat mem -> uma
discontig -> NUMA
sparse -> Hotplug + NUMA
# E820 memory map
Kerenl setup header?: detect_memory() Load BIOS memory map into boot_params.e820_map
setup_arch-> 
{
	setup_memory_map -> default_machine_specific_memory_setup // Save e820 memory map into struct vriable e820 from boot_params.e820_map.
	max_pfn = e820_end_of_ram_pfn(); // max_pfn  BIOS-e820: mem 0x0000000100000000-0x00000003227fffff usable and last_pfn = 0x322800(12840MB), so last_pfn is invalid address, use it with <.
	mtrr update max_pfn, see [Processor supplementary capability](https://en.wikipedia.org/wiki/Processor_supplementary_capability)
	trim_low_memory_range // reserve 64k
	max_low_pfn = e820_end_of_low_ram_pfn(); //end of block below 4GB
}

# Memblock and bootmem
setup_arch->
	memblock_x86_fill// copy e820 to memblock, reconstructs direct memory mapping and setups the direct mapping of the physical memory at PAGE_OFFSET
memblock the [implementations](https://0xax.gitbooks.io/linux-insides/content/mm/linux-mm-1.html) of memblock is quite simple. static initialization with variable memblock.
bootmem is discarded by [ARM](https://lkml.org/lkml/2015/12/21/333) and x86
a little history in e820_register_active_region about memblock [replaced by lmb](https://lkml.org/lkml/2010/7/13/68)
## What is the meaning of the following notes?
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

# Zones and Nodes
setup_arch->x86_init.paging.pagetable_init = native_pagetable_init = paging_init -> zone_sizes_init->free_area_init_nodes
{
	free_area_init_node-> 
		calculate_node_totalpages
		
		alloc_node_mem_map// mem_map for FLAT, but not for us because we use sparsemem
		free_area_init_core
		{
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
## Zone lists
zone list is for Node not for zone. NUMA system has two zone lists:
Check MAX_ZONELISTS
 * [0]  : Zonelist with fallback
 * [1]  : No fallback (__GFP_THISNODE)
start_kernel->
	build_all_zonelists
or hotpulg or /proc/sys/vm/numa_zonelist_order: numa_zonelist_order_handler
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
          zone = 0xffff88207ffd2d80, # Node 1; fallback.
          zone_idx = 2
        }, {
          zone = 0x0, 
          zone_idx = 0
# Watermarks
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

# LQO
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
