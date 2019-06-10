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
user/kernel mode, kerne space/userspace address, good/bad area

# Kernel address
vmalloc,spurious_fault, kprobes_fault
## Bad area
[fixup_exception](https://www.kernel.org/doc/Documentation/x86/exception-tables.txt)
[Kernel-Level Exception Handling](https://www.linuxjournal.com/article/2135)
SIGSEGV 
kernel oops 

# Userspace address
## Bad area
PF_USER ? SIGSEGV
## Stack
premise: grows down: address > 65536 + 32 * sizeof(unsigned long) + rsp
expand_stack(): expand vma of stack; + good area workflow.
## Private anonymous page
Check 'man madvise' for private anonymous page.
Stack and Heap - mmap, brk: do_anonymous_page 
Library Private read & write: do_wp_page, 
Library Private write: cow page, do_cow_page
### do_cow_page
1. get page and cow:  __do_fault-> vma->vm_ops->fault = filemap_fault -> page cache ? page_cache_read add to lru
2. add page to anon lru list: finish_fault->alloc_set_pte-> page_add_new_anon_rmap -> __SetPageSwapBacked
### Page, rmap, pte
page: alloc_zeroed_user_highpage_movable
page_add_new_anon_rmap
set_pte_at
## Private file page
Libray Private  read: do_read_page, pte prot is PAGE_COPY check protection_map.
## Shared file page
do_shared_page
## Shared anonymous page
do_shared_page
## Swap page
do_swap_page

# Read and write pages - Copy-on-write technique.
## do_read_page
[map_pages](https://lwn.net/Articles/588802/)

# Synchronization
## down_read(&mm->mmap_sem)?
linux-tglx
commit b50661029222940e24d2fba7c982ac0774a38c78
Author: Andi Kleen <ak@muc.de>
Date:   Thu Sep 16 22:00:12 2004 -0700

    [PATCH] x86-64: avoid deadlock in page fault handler
    
    Avoid deadlock when kernel fault happens inside mmap sem.
Check ULKv3 Page 380.
https://lkml.org/lkml/2004/5/19/108
https://lkml.org/lkml/2013/5/13/418
## IRQ
commit 891cffbd6bcba26409869c19c07ecd4bfc0c2460
Author: Linus Torvalds <torvalds@linux-foundation.org>
Date:   Sun Oct 12 13:16:12 2008 -0700

    x86/mm: do not trigger a kernel warning if user-space disables interrupts and generates a page fault
