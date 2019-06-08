---
tags: [ kernel ] 
layout: post
date: 2017-04-06
title: Page fault
category: cs
---

# Page Fault
* page-fault exceptions error code
v3a: 4.7 - Figure 4-12. Page-Fault Error Code
P, W/R, U/S, I/D, RSVD
user/kernel mode, kerne space/userspace addr, good/bad area
## kernel mode
### Userspace address
Ondemanding page
COW - do_wp_page
### kernel address
vmalloc
### An invalide address - nocontext
* fixup_exception
[Kernel-Level Exception Handling](https://www.linuxjournal.com/article/2135)
https://www.kernel.org/doc/Documentation/x86/exception-tables.txt
ex_handler
SIGSEGV the task since it passed the invalid address to kernel
* kernel oops 
the address wasn't from user space. It should be a kernel bug.
kproxy module error
## User mode
### Bad area
PF_USER ? SIGSEGV
## do_page_fault() - nuclus
### kernel address: 
vmalloc,spurious_fault, kprobes_fault, PF_USER ? bad_are_nosemaphore SIGSEGV, no_context
### user space address: 
#### Bad area 
1) !find_vma
2) !Permission
#### stack
premise: grows down: address > 65536 + 32 * sizeof(unsigned long) + rsp
expand_stack(): expand vma of stack; + good area workflow.
#### good area
handle_mm_fault && error_handle
## handle_mm_fault - good area
FAULT_FLAG_WRITE        0x01    /* Fault was a write access */
FAULT_FLAG_MKWRITE      0x02    /* Fault was mkwrite of existing pte */
FAULT_FLAG_ALLOW_RETRY  0x04    /* Retry fault if blocking */
FAULT_FLAG_RETRY_NOWAIT 0x08    /* Don't drop mmap_sem and wait when retrying */
FAULT_FLAG_KILLABLE     0x10    /* The fault task is in SIGKILL killable region */
FAULT_FLAG_TRIED        0x20    /* Second try */
FAULT_FLAG_USER         0x40    /* The fault originated in userspace */
FAULT_FLAG_REMOTE       0x80    /* faulting for non current tsk/mm */
FAULT_FLAG_INSTRUCTION  0x100   /* The fault was during an instruction fetch */
### do_anonymous_page
page: alloc_zeroed_user_highpage_movable
page_add_new_anon_rmap
set_pte_at
### do_fault
* normal read - do_read_fault
* file private mappings - do_cow_fault
* shared(file or anonymous mappings) - do_shared_fault
### do_swap_page
### do_wp_page
true cow
## DBG: Kernel oops
[kernel oops tracing](https://www.kernel.org/doc/Documentation/oops-tracing.txt)
[Debugging a kernel crash found by syzkaller](http://vegardno.blogspot.com/2016/08/sync-debug.html?m=1)

## LQO
* why down_read in page_fault
http://wangcong.org/2012/06/01/-e4-b8-ba-e4-bb-80-e4-b9-88linux-e5-86-85-e6-a0-b8-e4-b8-8d-e5-85-81-e8-ae-b8-e5-9c-a8-e4-b8-ad-e6-96-ad-e4-b8-ad-e4-bc-91-e7-9c-a0-ef-bc-9f/
* enable IRQ in page fault
for user mode
commit 891cffbd6bcba26409869c19c07ecd4bfc0c2460
Author: Linus Torvalds <torvalds@linux-foundation.org>
Date:   Sun Oct 12 13:16:12 2008 -0700

    x86/mm: do not trigger a kernel warning if user-space disables interrupts and generates a page fault
for kernel mode
简直精髓
* What about down_read(&mm->mmap_sem)?
linux-tglx
commit b50661029222940e24d2fba7c982ac0774a38c78
Author: Andi Kleen <ak@muc.de>
Date:   Thu Sep 16 22:00:12 2004 -0700

    [PATCH] x86-64: avoid deadlock in page fault handler
    
    Avoid deadlock when kernel fault happens inside mmap sem.
Check ULKv3 Page 380.
https://lkml.org/lkml/2004/5/19/108
https://lkml.org/lkml/2013/5/13/418

# memory mappings
TLPI:chapter 49
LSP: Chapter 8
rmap: anonymous page: asyn: page fault
rmap: file page: syn vma_link_file
## Anonymouse private mappings
anonymous page
* onset
malloc exploit it to alloc memory 
* nuclus
do_anonymous_page
## File private mappings -text and iniliazed data. 
双空, anonymous page
* onset - mmap
do_mmap -> mmap_region -> call_mmap->generic_file_mmap->vma->vm_ops = & generic_file_vm_ops
* nuclus -> page fault -> do_cow_page
1. get page and cow:  __do_fault-> vma->vm_ops->fault = filemap_fault -> page cache ? page_cache_read add to lru
2. add page to anon lru list: finish_fault->alloc_set_pte-> page_add_new_anon_rmap -> __SetPageSwapBacked
### Example text and initalized data
[??---p PROT_NOME mapping](http://www.greenend.org.uk/rjk/tech/dataseg.html#summary)
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
## Anonymouse shared mappings - Parent and child share memory
[vmscan: limit VM_EXEC protection to file pages](https://lore.kernel.org/patchwork/patch/174306/)
[ashmem](https://lwn.net/Articles/452035/)
file page: i_mmap
* onset - mmap
do_mmap -> mmap_region -> vma_link -> (__shmem_file_setup) && __vma_link_file: into i_mmap interval_tree.
* nuclus - share fault
do_shared_fault
shmem_getpage_gfp
mapping = inode->i_mapping
shmem_add_to_page_cache
* dirty a shared page since it's write fault
do_shared_fault->fault_dirty_shared_page
* what about read fault?
seems read won't dirty the shared page.
* Swaping out a anonymouse shared page
pageout->shmem_writepage - Move the page from the page cache to the swap cache; and swp_to_radix_entry
[map_pages](https://lwn.net/Articles/588802/)
### Write protect for shared page
do_wp_page -> wp_page_reuse
[PATCH] mm: tracking shared dirty pages - d08b3851da41d0ee60851f2c75b118e1f7a5fc89
## File shared mappings - a) Memory-mapped I/O, b)IPC using a shared file mapping
file page: i_mmap

# COW: for malloc page
are the ptes set during fork for child process?
check copy_one_pte is_cow_mapping in copy_page_range in dup_mmap

# Summary
mapping:  anon & private, anon 
from: anonymous page, file-backing 
to: swap area, file on the disk
rmap:
lru:
cache: swap cache, page cache
read:
write:
backend: swap area, disk 
PageAnon:		f&p, a&p 
!page_is_file_cache:	f&p, a&p, a&s

## fork

## dfc8d636cdb95f7b792d5ba8c9f3b295809c125d

# MAP_SYNC
https://lwn.net/Articles/731706/
dax_iomap_pte_fault
