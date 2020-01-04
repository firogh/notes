---
tags: [ kernel ]
layout: post
date: 2014-12-28
title: Linux memory management
category: cs
---

# Reference
Dragon book 9th chapter 8 main memory
[A must-read: memory FAQ by Landley](https://landley.net/writing/memory-faq.txt)
[An Evolutionary Study of Linux Memory Management for Fun and Profit](https://www.usenix.org/system/files/conference/atc16/atc16_paper-huang.pdf)
[Kernel developers MM documentation](https://www.kernel.org/doc/html/v4.18/vm/index.html)
[kernel.org Memory Management](https://www.kernel.org/doc/html/latest/admin-guide/mm/index.html)
https://linux-mm.org/
[2. Memory management](http://bitsavers.trailing-edge.com/pdf/sun/sunos/1.1/800-1108-01E_System_Interface_Manual_for_the_Sun_Workstation_Jan84.pdf)


# History
## 1959 paging
[Paging](https://en.wikipedia.org/wiki/Paging) according to [History of VM](https://en.wikipedia.org/wiki/Virtual_memory) is developed at 1959.

## 1961 segmentation
Fist appears in Burroughs B5500

## 1963 ~ 1965  buddy system
[Buddy memory allocation](https://en.wikipedia.org/wiki/Buddy_memory_allocation)

## 1956 ~ 1962 VM
[BEFORE MEMORY WAS VIRTUAL](http://denninginstitute.com/pjd/PUBS/bvm.pdf)
[Memory part 3: Virtual Memory](https://lwn.net/Articles/253361/)

# Memory partitioning
Check OSIDP 7.2
## Page vs variable-sized memory allocations
By variable-sized, we are not taking historical segementation.
Placement algorithm: Best-fit, first-fit. Within variable-szied memory allocations, it's very difficult to track free memories compared with page-sized.

## Segmentation
OSIDP
The difference, compared to dynamic partitioning, is that with seg-
mentation a program may occupy more than one partition, and these partitions
need not be contiguous. Segmentation eliminates internal fragmentation but, like
dynamic partitioning, it suffers from external fragmentation. However, because a
process is broken up into a number of smaller pieces, the external fragmentation
should be less.

## 1964 fixed-sized partitioning
[Partitioned allocation](https://en.wikipedia.org/wiki/Memory_management_(operating_systems)#Partitioned_allocation)
Dragon OS book 8.3.2 Memory Allocation
OSIDP: 7.2 MEMORY PARTITIONING
fixed-sized partitions.
variable-partition scheme or Segmentation
### Describing pages: memory map, the 'mem_map' array
[Memory: the flat, the discontiguous, and the sparse](https://lwn.net/Articles/789304/)
Firo: All these 3 types are [Flat memory model](https://en.wikipedia.org/wiki/Flat_memory_model)
Firo: I think kernel developers are abusing the terminology 'memory model'. [Physical Memory Model](https://www.kernel.org/doc/html/latest/vm/memory-model.html)
Firo: the full name should be memory map/layout model.
[sparsemem memory model](https://lwn.net/Articles/134804/) [PATCH](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/commit/?id=d41dee369bff3b9dcb6328d4d822926c28cc2594)
[DISCONTIGMEM, !NUMA, and SLUB](https://lwn.net/Articles/439472/)
8170e6bed465b4b0c7687f93e9948aca4358a33b x86, 64bit: Use a PF handler to materialize early mappings on demand
zone_mem_map => node_mem_map
commit a0140c1d85637ee5f4ea7c78f066e3611a6a79dc
Author: KAMEZAWA Hiroyuki <kamezawa.hiroyu@jp.fujitsu.com>
Date:   Mon Mar 27 01:15:55 2006 -0800
    [PATCH] remove zone_mem_map

Related code: pfn_to_page
#### Zones
ULK 2: 7.1.2 Memory Zones; LDD: Memory zones; LKD3: Zones; UVM: Chapter 2  Describing Physical Memory
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

[Zone-lock and mmap_sem scalability](https://lwn.net/Articles/753269/)

# Memory allocation
[Dynamic Storage Allocation: A Survey and Critical Review](https://compas.cs.stonybrook.edu/~nhonarmand/courses/sp17/cse506/papers/dsa.pdf)
## track: buddy memory system
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

### Related code
free_area; page_is_buddy; PageBuddy(buddy) && page_order(buddy) 
setup_arch->x86_init.paging.pagetable_init = native_pagetable_init = paging_init ->
        sparse_init ...-> vmemmap_populate      # vmemmap
        zone_sizes_init->free_area_init_nodes -> free_area_init_node-> free_area_init_core
                zone_pcp_init # init percpu pageset with boot_pageset
                init_currently_empty_zone(zone, zone_start_pfn, size); # free_area.free_list
                memmap_init_zone # Memory map a) Set all page to reserved. MIGRATE_MOVABLE? b) Set node, zone to page->flags; set_page_links

start_kernel->mm_init
        mem_init-> memblock_free_all or free_all_bootmem # /* this will put all low memory onto the freelists */

## obtain/alloc and return/free memory
page allocator
page reclamation, memory compaction

# Lift the burden of loading program from application
paging
# Decopule addresses and memory locations
VM, memory mapping.

# Fragmentations
buddy system, memory compaction
## Mobility
PLKA - 3.5.2
[Making kernel pages movable](https://lwn.net/Articles/650917/)
Split the free lists for movable and unmovable allocations - b2a0ac8875a0a3b9f0739b60526f8c5977d2200f

Related code:
set_pageblock_migratetype

find_suitable_fallback
        [MIGRATE_UNMOVABLE]   = { MIGRATE_RECLAIMABLE, MIGRATE_MOVABLE,   MIGRATE_TYPES },
        [MIGRATE_RECLAIMABLE] = { MIGRATE_UNMOVABLE,   MIGRATE_MOVABLE,   MIGRATE_TYPES },
        [MIGRATE_MOVABLE]     = { MIGRATE_RECLAIMABLE, MIGRATE_UNMOVABLE, MIGRATE_TYPES },

MIGRATE_RECLAIMABLE? 
commit e12ba74d8ff3e2f73a583500d7095e406df4d093
Refs: v2.6.23-4359-ge12ba74d8ff3
Author:     Mel Gorman <mel@csn.ul.ie>
AuthorDate: Tue Oct 16 01:25:52 2007 -0700
Commit:     Linus Torvalds <torvalds@woody.linux-foundation.org>
CommitDate: Tue Oct 16 09:43:00 2007 -0700
    Group short-lived and reclaimable kernel allocations
[treewide: remove GFP_TEMPORARY allocation flag](https://lore.kernel.org/patchwork/patch/814997/)

LQO:
MIGRATE_HIGHATOMIC

## Memory compaction
[Memory compaction issues](https://lwn.net/Articles/591998/)
[Memory compaction](https://lwn.net/Articles/368869/)
https://kernelnewbies.org/Linux_2_6_35#Memory_compaction
[firo-must: The hard work behind large physical memory allocations in the kernel](https://linuxplumbersconf.org/event/2/contributions/65/attachments/15/171/slides-expanded.pdf)


## Anti fragmentation
[The What, The Why and the Where To of Anti-Fragmentation](https://www.kernel.org/doc/ols/2006/ols2006v1-pages-369-384.pdf)

