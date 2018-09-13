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
anonymous page
* onset - mmap
do_mmap -> mmap_region -> call_mmap->generic_file_mmap->vma->vm_ops = & generic_file_vm_ops
* nuclus -> page fault -> do_cow_page
1. get page and cow:  __do_fault-> vma->vm_ops->fault = filemap_fault -> page cache ? page_cache_read add to lru
2. add page to anon lru list: finish_fault-> alloc_set_pte-> page_add_new_anon_rmap -> __SetPageSwapBacked
### Example text and initalized data
[??---p PROT_NOME mapping](http://www.greenend.org.uk/rjk/tech/dataseg.html#summary)
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
file page: i_mmap
* onset - mmap
do_mmap -> mmap_region -> vma_link -> (__shmem_file_setup) && __vma_link_file: into i_mmap interval_tree.
* nuclus - share fault
do_shared_fault
## File shared mappings - a) Memory-mapped I/O, b)IPC using a shared file mapping
file page: i_mmap
