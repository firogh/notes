---
tags: [ kernel ] 
layout: post
date: 2014-12-28
title: Linux memory management
category: cs
---

# Reference
[BEFORE MEMORY WAS VIRTUAL](http://denninginstitute.com/pjd/PUBS/bvm.pdf)
[Memory part 3: Virtual Memory][1]

[1]: https://lwn.net/Articles/253361/

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
data synchronization, the flush threads, pdflush
##Page swap
The available RAM memory in a computer is never enough to meet user needs or to always satisfy memory-intensive applications.


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

* compound pages
18fa11efc279c20af5eefff2bbe814ca067
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

