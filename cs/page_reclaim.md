---
tags: [ kernel ] 
layout: post
date: 2017-04-06
title: Linux page reclaim
category: cs
---
# Reference
[The case of the overly anonymous anon_vma][1]
[1]: https://lwn.net/Articles/383162/

When or why do we wnant to reclaim pages?
	Periodically Keep a halthy avaliable free pages.
	No enough memory for a large memory alloc.
	Manually initate
What pages do we want to reclaim?
	All user space pages, except some pages.
	Do not reclaim kernel page in order to make things simple.
Where to store the pages?
	There may be many processes which use the same pages.
	We put list of pages in a global site.
How to organize the pages.
	LRU
	When to add the page to LRU
How to reclaim in deatils.
	For memory-mapped file page, write back.
	For private and anonymous, swap.
	For memroy-mapped file page without modification.
	How to distinguish kernel page from user space page?
How to modify the process's pte to let it know the page has been reclaimed.
	Reverse mapping.


# Anonymous reverse mappinng
Where do reverse mappings come from?
	syscall: mmap(ANONYMOUS | PRIVATE)
	Proceass address space: Heap, stack.
mm: change anon_vma linking to fix multi-process server scalability issue
5beb49305251e5669852ed541e8e2f2f7696c53e
When we fork a child process, it will associate multiple anon_vmas with a VMA.
	
