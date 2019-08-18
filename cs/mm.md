---
tags: [ kernel ]
layout: post
date: 2014-12-28
title: Linux memory management
category: cs
---

# Ref
[An Evolutionary Study of Linux Memory Management for Fun and Profit](https://www.usenix.org/system/files/conference/atc16/atc16_paper-huang.pdf)

# VM
[BEFORE MEMORY WAS VIRTUAL](http://denninginstitute.com/pjd/PUBS/bvm.pdf)
[Memory part 3: Virtual Memory](https://lwn.net/Articles/253361/)

# Zones and Nodes
ULK 2: 7.1.2 Memory Zones
LDD: Memory zones
LKD3: Zones
UVM: Chapter 2  Describing Physical Memory
history: commit 4d1ceac2980133513fec009b18514f026ff58afa (tag: 2.3.23pre5)
Author: Linus Torvalds <torvalds@linuxfoundation.org>
Date:   Fri Nov 23 15:27:55 2007 -0500
    Import 2.3.23pre5
+#define MEM_TYPE_DMA           0
+#define MEM_TYPE_NORMAL                1
+#define MEM_TYPE_HIGH          2

history: commit cb75bc5a9d9a433f6b5c8015a0f43c1b4d0fad79 (tag: 2.3.27pre6)
Author: Linus Torvalds <torvalds@linuxfoundation.org>
Date:   Fri Nov 23 15:28:33 2007 -0500
    Import 2.3.27pre6
+ *  Reshaped it to be a zoned allocator, Ingo Molnar, Red Hat, 1999
-#define MEM_TYPE_DMA           0
-#define MEM_TYPE_NORMAL                1
+#define ZONE_DMA               0
+#define ZONE_NORMAL            1
-static struct free_area_struct free_area[NR_MEM_TYPES][NR_MEM_LISTS];
+typedef struct zone_struct {

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
## mm_init
Most of the reset the memory initialization is done in this function.
start_kernel->
	mm_init
{
	# ??
	# /* this will put all low memory onto the freelists */
	mem_init-> memblock_free_all or free_all_bootmem
}

# Watermarks
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

# Low memory reserved
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
