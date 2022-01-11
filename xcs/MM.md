---
tags: [ kernel ]
layout: post
date: 2014-12-28
title: Linux memory management
category: cs
---

# Reference
[Kernel Recipes 2017 - 20 years of Linux virtual memory - Andrea Arcangeli](https://www.youtube.com/watch?v=pZghXbeCH5s)
Dragon book 9th chapter 8 main memory
[A must-read: memory FAQ by Landley](https://landley.net/writing/memory-faq.txt)
[An Evolutionary Study of Linux Memory Management for Fun and Profit](https://www.usenix.org/system/files/conference/atc16/atc16_paper-huang.pdf)
[Kernel developers MM documentation](https://www.kernel.org/doc/html/v4.18/vm/index.html)
[kernel.org Memory Management](https://www.kernel.org/doc/html/latest/admin-guide/mm/index.html)
https://linux-mm.org/
[2. Memory management](http://bitsavers.trailing-edge.com/pdf/sun/sunos/1.1/800-1108-01E_System_Interface_Manual_for_the_Sun_Workstation_Jan84.pdf)

# Memory partitioning
Check OSIDP 7.2
## Page vs variable-sized memory allocations
By variable-sized, we are not taking historical segementation.
Placement algorithm: Best-fit, first-fit. Within variable-szied memory allocations, it's very difficult to track free memories compared with page-sized.

## 1961 segmentation
Fist appears in Burroughs B5500
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

# Zones
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
Page allocator.
## Overcommit
https://www.kernel.org/doc/Documentation/vm/overcommit-accounting
[理解LINUX的MEMORY OVERCOMMIT](http://linuxperf.com/?p=102)
[Virtual memory settings in Linux - The Problem with Overcommit](http://engineering.pivotal.io/post/virtual_memory_settings_in_linux_-_the_problem_with_overcommit/)
### MAP_NORESERVE
history: commit 56307bd2d705fd2b982d89956e92d5ed142ee69d
Refs: <2.1.2>
Author:     Linus Torvalds <torvalds@linuxfoundation.org>
    Import 2.1.2
MAP_NORESERVE  0x10000         /* don't check for reservations */
        /* Private writable mapping? Check memory availability.. */
        if ((vma->vm_flags & (VM_SHARED | VM_WRITE)) == VM_WRITE) {
-               if (!vm_enough_memory(len >> PAGE_SHIFT)) {
+               if (!(flags & MAP_NORESERVE) &&
+                   !vm_enough_memory(len >> PAGE_SHIFT)) {
                        kfree(vma);
                        return -ENOMEM;
### Reserved vs committed
Improve Native Memory Tracking to report the actual RSS usage https://bugs.openjdk.java.net/browse/JDK-8249666

### Committed_AS address_space
Documentation/filesystems/proc.rst
https://www.kernel.org/doc/Documentation/vm/overcommit-accounting

# Garbage collection
[Linus Torvalds on Garbage Collection (2002)](https://news.ycombinator.com/item?id=2473932)
[Linus on refrence counting](https://yarchive.net/comp/linux/reference_counting.html)
[Paul E. McKenney: Overview of Linux-Kernel Reference Counting](http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.145.3800&rep=rep1&type=pdf)
Book: The Garbage Collection Handbook
Book: Japanese GC book.
https://www.linuxidc.com/Linux/2015-01/111565.htm
## Reference counting
[Introducing lockrefs](https://lwn.net/Articles/565734/)

# Fragmentations
buddy system, memory compaction
## Anti fragmentation
[The What, The Why and the Where To of Anti-Fragmentation](https://www.kernel.org/doc/ols/2006/ols2006v1-pages-369-384.pdf)
