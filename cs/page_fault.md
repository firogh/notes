---
tags: [ kernel ] 
layout: post
date: 2017-04-06
title: Page fault
category: cs
---

# three cases where page fault might happen
the first is if the VA is simply not mapped at all into VAS of the process that's requesting it.
Another case is if a process has in sufficient permissions for this address. so you're trying to write into something that's read-only for example.
the third is if the VA is valid but it's swapped out.
## Use case
not mapped
lazy allocation
swapping

#Page faults
Page fault: If a process accesses a part of virtual address space not yet associated with a page in memory


* Segemnt fault
Segment fault: address is not in VMA, namely invalid address! Check bad_area() in x86.
https://www.kernel.org/doc/Documentation/x86/exception-tables.txt
Null pointer: probably, mostly catched in below 0x08048000.

* Demanding page 
vmalloc_fault(), 进程从用户态进入内核态不会引起CR3的改变.页表同步!
expand_stack()/handle_mm_fault()

* vmalloc fault

## mm fault handler
arch/powerpc/mm/fault.c
deatils of trap is in bad_page_fault

sigbus 
__handle_mm_fault
{
        hugetlb_fault CONFIG_HUGETLB_PAGE
        handle_pte_fault-> 
        {
                do_linear_fault -> __do_fault
                {
                        vma->vm_ops->fault(vma, &vmf);
                }
                do_anonymous_page
                {
                        check_stack_guard_page
                }
                do_nonlinear_fault
                do_swap_page
                do_wp_page
                {
                        vma->vm_ops->page_mkwrite(vma, &vmf); //??
                }
}

