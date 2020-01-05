---
tags: [ kernel ]
title: memory mapping
date: 2018-08-22T21:39:41+08:00
category: cs
---

This article is talking about user space Memory mmapping; it's not limitted to mmap(2) system call.
[Understanding memory mapping](https://www.ibm.com/support/knowledgecenter/en/ssw_aix_72/com.ibm.aix.genprogc/understanding_mem_mapping.htm)
TLPI:chapter 49 and LSP: Chapter 8

# History
BSD 4.2
1990 SunOS 4.1
[A Must-read: The applications programmer gains access to the facilities of the VM system through several sets of system calls.](http://bitsavers.trailing-edge.com/pdf/sun/sunos/4.1/800-3846-10A_System_Services_Overview_199003.pdf)

# Formal causes
[What are memory mappings? - Landley](https://landley.net/writing/memory-faq.txt)

> A memory mapping is a set of page table entries describing the properties
> of a consecutive virtual address range.  Each memory mapping has a
> start address and length, permissions (such as whether the program can
> read, write, or execute from that memory), and associated resources (such
> as physical pages, swap pages, file contents, and so on).
Firo:  mmap, page fault, PFRA.

# VMA
vma's unit is PAGE_SIZE; (vm_end - vm_start) % 0x1000 == 0 is True.
## split_vma
new_below
commit 5846fc6c31162234e88bdfd91548b1cf0d2cebbd
Author: Andrew Morton <akpm@digeo.com>
Date:   Tue Sep 17 06:35:47 2002 -0700
    [PATCH] consolidate the VMA splitting code
new_below means the place where the old vma go to! Bad naming!
0 means the old will save the head part. 1 means tail part.

## File
### PAS
Protection, Shared, Private.
vm_page_prot, vm_flags
remove_mapping

### Backing dev
vm_file, vm_pgoff

## Private anonymouse mappings
Heap - malloc mmap
Anonymous Memory Mappings, LSP chapter 9

## File private mappings
Program: execve text,data,bss
Libraries
openat(AT_FDCWD, "/lib64/libc.so.6", O_RDONLY|O_CLOEXEC) = 3 
mmap(NULL, 1857568, PROT_READ, MAP_PRIVATE|MAP_DENYWRITE, 3, 0) = 0x7f27cbb02000
* onset - mmap
do_mmap -> mmap_region 
ext2: generic_file_mmap -> vma->vm_ops = generic_file_vm_ops
ext4: ext4_file_mmap -> vma->vm_ops = ext4_file_vm_ops
both: filemap_fault
* nuclus 
Write - do_cow_page
Read - do_read_page
Read & write - do_wp_page

# Shared memory mapping
[Chapter 12  Shared Memory Virtual Filesystem:](https://www.kernel.org/doc/gorman/html/understand/understand015.html)

> This is a very clean interface that is conceptually easy to understand but it does not help anonymous pages as there is no file backing. To keep this nice interface, Linux creates an artifical file-backing for anonymous pages using a RAM-based filesystem where each VMA is backed by a “file” in this filesystem. Every inode in the filesystem is placed on a linked list called shmem_inodes so that they may always be easily located. This allows the same file-based interface to be used without treating anonymous pages as a special case. 

Firo: every time you create a shared memory via mmap(2), you create a inode with same name dev/zero in the hidden shm_mnt fs; 
The name dev/zero is only a name. It has nothing related to /dev/zero in drivers/char/mem.c. And /dev/shm is only a tmpfs; it has nothing related shmemfs, but POSIX's shm_open uses /dev/shm.
## Shared anonymouse mappings
[vmscan: limit VM_EXEC protection to file pages](https://lore.kernel.org/patchwork/patch/174306/)
If someone may take advange of reclaimation code by mmap(..., VM_EXEC, SHRED|ANON), OOM may occur since the old code protect it from reclaiming by add it back to the active list. Great patch. However, program running in tmpfs will also penalized.
page_is_file_cache < !PageAnon
[ashmem](https://lwn.net/Articles/452035/)
* onset - mmap
do_mmap -> mmap_region -> vma_link -> (__shmem_file_setup) && __vma_link_file: into i_mmap interval_tree.
* nuclus - share fault
Read: do_read_fault
Write: do_shared_fault -> shmem_getpage_gfp shmem_add_to_page_cache
WP: do_wp_page -> wp_page_shared or wp_page_reuse
b)IPC using a shared file mapping
## File shared mappings - a) Memory-mapped I/O
## History
late 70s - IPC: see TLPI: Chapter 45 INTRODUCTION TO SYSTEM V IPC 
they first appear together in Columbus UNIX, a Bell UNIX for database and efficient transaction processing
1983 - IPC See TLPI or wikipedia shared mmeory.
they land together in System V that made them popular in mainstream UNIX-es, hence the name

1983 - BSD mmap with shared vs private memory mapping
BSD 4.2: The system supports sharing of data between processes by allowing pages to be mapped into memory. These mapped pages may be shared with other processes or private to the process.

1984 Jan - BSD mmap with file memory mapping support by SunOS
The mmap seems firstly implemented by [SunOS 1.1](http://bitsavers.trailing-edge.com/pdf/sun/sunos/1.1/800-1108-01E_System_Interface_Manual_for_the_Sun_Workstation_Jan84.pdf)
N.B. This call is not completely implemented In 4.2(BSD).
More sunos docs: http://bitsavers.trailing-edge.com/pdf/sun/sunos/

1988
[SunOS 4[4] introduced Unix's mmap, which permitted programs "to map files into memory."](https://en.wikipedia.org/wiki/Memory-mapped_file#History)
1989
One paper found in OSTEP: [Memory Coherence in Shared Virtual Memory Systems](https://courses.cs.washington.edu/courses/cse551/09sp/papers/memory_coherence.pdf)


# mmap
## MAP_SYNC
[Two more approaches to persistent-memory writes](https://lwn.net/Articles/731706/)
dax_iomap_pte_fault

# madvise
## MADV_SEQUENTIAL and reclaim
mm: more likely reclaim MADV_SEQUENTIAL mappings - 4917e5d0499b5ae7b26b56fccaefddf9aec9369c

## MADV_FREE
[Volatile ranges and MADV_FREE](https://lwn.net/Articles/590991/)
[Use new madvise()'s MADV_FREE on the private heap](https://bugs.python.org/issue26601)
commit 854e9ed09dedf0c19ac8640e91bcc74bc3f9e5c9
Author: Minchan Kim <minchan@kernel.org>
Date:   Fri Jan 15 16:54:53 2016 -0800
    mm: support madvise(MADV_FREE)
commit 10853a039208c4afaa322a7d802456c8dca222f4
Author: Minchan Kim <minchan@kernel.org>
Date:   Fri Jan 15 16:55:11 2016 -0800
    mm: move lazily freed pages to inactive list

commit f7ad2a6cb9f7c4040004bedee84a70a9b985583e
Author: Shaohua Li <shli@fb.com>
Date:   Wed May 3 14:52:29 2017 -0700
    mm: move MADV_FREE pages into LRU_INACTIVE_FILE list


# mprotect
## PROT_NONE
[---p PROT_NOME mapping](http://www.greenend.org.uk/rjk/tech/dataseg.html#summary)
show_vma_header_prefix
cat /proc/self/maps 
7ffff7a17000-7ffff7bcc000 r-xp 00000000 08:03 1188168                    /usr/lib64/libc-2.27.so ============> text
7ffff7bcc000-7ffff7dcc000 ---p 001b5000 08:03 1188168                    /usr/lib64/libc-2.27.so ============> PROT_NONE
7ffff7dcc000-7ffff7dd0000 r--p 001b5000 08:03 1188168                    /usr/lib64/libc-2.27.so ============> read only data
7ffff7dd0000-7ffff7dd2000 rw-p 001b9000 08:03 1188168                    /usr/lib64/libc-2.27.so ============> initialized
7ffff7dd2000-7ffff7dd6000 rw-p 00000000 00:00 0 
strace -e mmap,mprotect cat /dev/null 
mmap(NULL, 3926752, PROT_READ|PROT_EXEC, MAP_PRIVATE|MAP_DENYWRITE, 3, 0) = 0x7ffff7a17000       ===> text
mprotect(0x7ffff7bcc000, 2097152, PROT_NONE) = 0                                ======================> PROT_NONE
mmap(0x7ffff7dcc000, 24576, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x1b5000) = 0x7ffff7dcc000
mmap(0x7ffff7dd2000, 15072, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_ANONYMOUS, -1, 0) = 0x7ffff7dd2000
mprotect(0x7ffff7dcc000, 16384, PROT_READ) = 0                                          ========> read only data
/* If _PAGE_BIT_PRESENT is clear, we use these: */
/* - if the user mapped it with PROT_NONE; pte_present gives true */
[A MUST READ: Mel on PAGE_PROTNONE](https://www.kernel.org/doc/gorman/html/understand/understand006.html)
[Using mprotect(.., .., PROT_NONE) on Linux](https://volatility-labs.blogspot.com/2015/05/using-mprotect-protnone-on-linux.html)
[Linus on _PAGE_PROTNONE](https://lkml.org/lkml/1998/9/21/55)
 define _PAGE_BIT_PROTNONE      _PAGE_BIT_GLOBAL
tglx: commit 06d9f6ff137579551a2ee18661847915fe2bb812 (tag: 0.97.5)
Author: Linus Torvalds <torvalds@linuxfoundation.org>
Date:   Fri Nov 23 15:09:05 2007 -0500
    [PATCH] Linux-0.97.5 (September 12, 1992)
There isn't too much useful information.
https://www.spinics.net/lists/newbies/msg08579.html
man mprotect, PROT_NONE
userspace addr is associated with non-GLOBAL pte, so the 8th G is reused by PROT_NONE.
