---
tags: [ kernel ]
layout: post
date: 2014-12-28
title: Linux memory management
category: cs
---

# Reference
[An Evolutionary Study of Linux Memory Management for Fun and Profit](https://www.usenix.org/system/files/conference/atc16/atc16_paper-huang.pdf)
[Kernel developers MM documentation](https://www.kernel.org/doc/html/v4.18/vm/index.html)
[kernel.org Memory Management](https://www.kernel.org/doc/html/latest/admin-guide/mm/index.html)
https://linux-mm.org/

# History
Dragon book 9th chapter 8 main memory
## Unknow old era for fixed/variable-sized partitioning
[Partitioned allocation](https://en.wikipedia.org/wiki/Memory_management_(operating_systems)#Partitioned_allocation)
Dragon OS book 8.3.2 Memory Allocation
OSIDP: 7.2 MEMORY PARTITIONING
fixed-sized partitions.
variable-partition scheme or Segmentation

## 1959 paging
[Paging](https://en.wikipedia.org/wiki/Paging) according to [History of VM](https://en.wikipedia.org/wiki/Virtual_memory) is developed at 1959.

## 1961 segmentation
Fist appears in Burroughs B5500

## 1963 ~ 1965  buddy system
[Buddy memory allocation](https://en.wikipedia.org/wiki/Buddy_memory_allocation)

## 1956 ~ 1962 VM
[BEFORE MEMORY WAS VIRTUAL](http://denninginstitute.com/pjd/PUBS/bvm.pdf)
[Memory part 3: Virtual Memory](https://lwn.net/Articles/253361/)

# Purposes
## keeping track of physical memory, 
Buddy system, why?
## obtain/alloc and return/free memory
page allocator
### for user space application =>
VM, paging, page tables, page fault, memory mapping.
### for mitigating fragmenatations
buddy system, memory compaction
### for maintain healthy memory
page reclamation, memory compaction

# Formal cause
[Dynamic Storage Allocation: A Survey and Critical Review]
## Memory partitioning
Check OSIDP 7.2
### Page vs variable-sized memory allocations
By variable-sized, we are not taking historical segementation.
Placement algorithm: Best-fit, first-fit. Within variable-szied memory allocations, it's very difficult to track free memories compared with page-sized.

### Segmentation
OSIDP
The difference, compared to dynamic partitioning, is that with seg-
mentation a program may occupy more than one partition, and these partitions
need not be contiguous. Segmentation eliminates internal fragmentation but, like
dynamic partitioning, it suffers from external fragmentation. However, because a
process is broken up into a number of smaller pieces, the external fragmentation
should be less.

### [buddy system 1965](http://sci-hub.tw/https://dl.acm.org/citation.cfm?doid=365628.365655) [Buddy memory allocation](https://en.wikipedia.org/wiki/Buddy_memory_allocation)
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

## Fragmentations

## Zones
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
### Zone lists
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
# Material
## Onset
setup_arch->x86_init.paging.pagetable_init = native_pagetable_init = paging_init -> zone_sizes_init->free_area_init_nodes
	free_area_init_node-> 
		calculate_node_totalpages
		alloc_node_mem_map// mem_map for FLAT, but not for us because we use sparsemem
		free_area_init_core
			zone->managed_pages = zone->present_pages - memmap_pages - DMA?dma_reserve:0
			// init percpu pageset with boot_pageset
			zone_pcp_init 
			// free_area.free_list
			init_currently_empty_zone(zone, zone_start_pfn, size);
			// Set all page to reserved. MIGRATE_MOVABLE?
			// Set node, zone to page->flags; set_page_links
			memmap_init_zone
### Buddy system
start_kernel->mm_init
	# /* this will put all low memory onto the freelists */
	mem_init-> memblock_free_all or free_all_bootmem
