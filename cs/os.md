---
tags: [ cs ] 
title: Operating system
date: 2015-02-27T15:46:14+08:00 
category: cs
---

# Signal 
* struct signal_sturct: 
The "struct signal_struct" is the random *leftovers* from all the other stuff.
http://thread.gmane.org/gmane.linux.kernel/512831/focus=513990
* sigpending
Store blocked signal info
* Non-mask signal
SIGKILL, SIGSTOP
##Generate signal
__send_signal();
##Process siganl
* SIGKILL (may be some other)
process in _send_signal()-> complete_signal() tsk->state |= TASK_WAKEKILL 
http://lwn.net/Articles/288056/
http://www.ibm.com/developerworks/library/l-task-killable/
* others
each time a switch is made from kernel mode to user mode, 
arch-specific: entry.S -> do_siganl()
{ 
	get_signal_deliver()
	{
		if fatal -> do_greoup_exit()->...__cleanup_sighand()
	}

	handle_signal() -> k->u(hanle)-sigreturn->k
}


# mm fault handler

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
# kernel panic 3.10.62
kernel/panic.c
general protection fault and page fault
1498 errorentry general_protection do_general_protection
1499 errorentry page_fault do_page_fault
static const struct stacktrace_ops print_trace_ops = { 
        .stack                  = print_trace_stack,
        .address                = print_trace_address,
        .walk_stack             = print_context_stack,
};
no_context->
{
	show_fault_oops->
	__die->
	{
		print_modules
		show_regs
		{
			printk(KERN_DEFAULT "Stack:\n");
			show_stack_log_lvl->
			{
				show_trace_log_lvl->
				{
					printk("%sCall Trace:\n", log_lvl);
					// arch/x86/kernel/dumpstack_64.c
					dump_trace->&print_trace_ops
				}
			}
			printk(KERN_DEFAULT "Code: ");
		}
	}
}
# Process and thread
# Kernel mapping
tgid_base_stuff show_map_vma

# GDB mapping
t->to_info_proc=procfs_info_proc
linux_info_proc not used in core dump
linux_core_info_proc_mappings this is correct

in kernel 
fill_files_note

_IO_new_fopen
_IO_new_file_init
libio/fileops.c _IO_file_open
& _IO_file_jumps_maybe_mmap -> _IO_default_uflow -> _IO_file_underflow_maybe_mmap

libio/fileops.c: decide_maybe_mmap
We use the file in read-only mode.  This could mean we can
     mmap the file and use it without any copying.  But not all
     file descriptors are for mmap-able objects and on 32-bit
     machines we don't want to map files which are too large since
     this would require too much virtual memory.  */
 __mmap64 (NULL, st.st_size, PROT_READ, MAP_SHARED, fp->_fileno, 0);

_IO_file_jumps_maybe_mmap 
  JUMP_INIT_DUMMY,
  JUMP_INIT(finish, _IO_file_finish),
  JUMP_INIT(overflow, _IO_file_overflow),
  JUMP_INIT(underflow, _IO_file_underflow_maybe_mmap),
  JUMP_INIT(uflow, _IO_default_uflow),
  JUMP_INIT(pbackfail, _IO_default_pbackfail),
  JUMP_INIT(xsputn, _IO_new_file_xsputn),
  JUMP_INIT(xsgetn, _IO_file_xsgetn_maybe_mmap),
  JUMP_INIT(seekoff, _IO_file_seekoff_maybe_mmap),
  JUMP_INIT(seekpos, _IO_default_seekpos),
  JUMP_INIT(setbuf, (_IO_setbuf_t) _IO_file_setbuf_mmap),
  JUMP_INIT(sync, _IO_new_file_sync), 
  JUMP_INIT(doallocate, _IO_file_doallocate),
  JUMP_INIT(read, _IO_file_read), 
  JUMP_INIT(write, _IO_new_file_write),
  JUMP_INIT(seek, _IO_file_seek),
  JUMP_INIT(close, _IO_file_close),
  JUMP_INIT(stat, _IO_file_stat),
  JUMP_INIT(showmanyc, _IO_default_showmanyc),
  JUMP_INIT(imbue, _IO_default_imbue)

_IO_file_jumps_mmap

_IO_file_jumps


[我为什么不再做PL人](http://www.yinwang.org/blog-cn/2016/03/31/no-longer-pl)

# AI
[从机器学习谈起](http://www.cnblogs.com/subconscious/p/4107357.html)

# Resources of OS design
[BrokenThorn Entertainment Operating System Development Series](http://www.brokenthorn.com/Resources/OSDevIndex.html)
[Operating Systems: Three Easy Pieces](http://pages.cs.wisc.edu/~remzi/OSTEP/)

# Unix/Linux programming
[The Art of Unix Programming](http://www.catb.org/esr/writings/taoup/html/)

# Quantaties
[Latency Numbers Every Programmer Should Know](http://people.eecs.berkeley.edu/~rcs/research/interactive_latency.html)

# Prof
[Mort Yao 程序与证明](http://www.tuicool.com/articles/YRZvem)

# Compile and link
[Pre-defined Compiler Macros](https://sourceforge.net/p/predef/wiki/Architectures/)
[Gnu binutils docs](https://sourceware.org/binutils/docs/)
[Welcome to GCC Wiki](https://gcc.gnu.org/wiki/HomePage)

# Distributed system
[CMU 15-640/440](https://www.andrew.cmu.edu/course/15-440-s13/index/syllabus_index.html)
[A Distributed Systems Reading List](http://dancres.github.io/Pages/)
[The Game of Distributed Systems Programming. Which Level Are You?](https://blog.incubaid.com/2012/03/28/the-game-of-distributed-systems-programming-which-level-are-you/)
[From the Ground Up: Reasoning About Distributed Systems in the Real World](http://bravenewgeek.com/from-the-ground-up-reasoning-about-distributed-systems-in-the-real-world/)
[WHAT WE TALK ABOUT WHEN WE TALK ABOUT DISTRIBUTED SYSTEMS](http://videlalvaro.github.io/2015/12/learning-about-distributed-systems.html)

# Hack
[Phrack](http://phrack.org/index.html)
[Obtain sys_call_table on amd64 ](https://www.exploit-db.com/papers/13146/)

# Cognition
[How does knowledge get locked up in people's heads?](http://jvns.ca/blog/2016/04/30/building-expertise-at-work/)

# Process
[How to Design Programs, Second Edition](http://www.ccs.neu.edu/home/matthias/HtDP2e/index.html)

# Programming language
[Learn Type Theory](http://jozefg.bitbucket.org/posts/2015-08-14-learn-tt.html)

# Functional language
[Functors, Applicatives, And Monads In Pictures](http://adit.io/posts/2013-04-17-functors,_applicatives,_and_monads_in_pictures.html) 

# Programming
[Programmer Competency Matrix](http://sijinjoseph.com/programmer-competency-matrix/)
[FB coding style](http://www.freebsd.org/cgi/man.cgi?query=style&sektion=9)
[GNU Coding Standards](http://www.gnu.org/prep/standards/standards.html)
[Google testing blog](http://googletesting.blogspot.com/)

# Network
[RAW PACKET FORMATS](http://www.securitywizardry.com/index.php/tools/packet-headers.html)
[从 IPv4 到 IPv6](http://wangcong.org/2012/10/15/-e4-bb-8e-ipv4-e5-88-b0-ipv6/)
[What’s wrong with IPv4 and Why we are moving to IPv6](http://www.tecmint.com/ipv4-and-ipv6-comparison/)

[Improving Linux networking performance](https://lwn.net/Articles/629155/)

# Concurrency
[Scalability Techniques for Practical Synchronization Primitives](http://queue.acm.org/detail.cfm?id=2698990)

# English
[英语口音纠正课程](http://www.tudou.com/programs/view/S9cfVXLvThM/?bid=03&pid=02&resourceId=100311153_03_05_02)
[英语思维漫谈](https://www.douban.com/note/144818819/#!/i!/ckDefault)
[Pronunciation workshop](http://v.youku.com/v_show/id_XNDYyMjcyNDI0.html?from=y1.2-1-87.3.3-2.1-1-1-2-0)
[Conceptualizing English Language Systems](https://www.tu-chemnitz.de/phil/english/sections/ling/course/Conceptualising/Annabell/Question_tags.htm)

# Memory 
[Recycling techniques](http://www.memorymanagement.org/mmref/recycle.html)

# Kernel
[走近Linux内核](http://wangcong.org/2007/03/09/-e8-b5-b0-e8-bf-91linux-e5-86-85-e6-a0-b8/)
[为什么Linux内核不允许在中断中休眠](http://wangcong.org/2012/06/01/-e4-b8-ba-e4-bb-80-e4-b9-88linux-e5-86-85-e6-a0-b8-e4-b8-8d-e5-85-81-e8-ae-b8-e5-9c-a8-e4-b8-ad-e6-96-ad-e4-b8-ad-e4-bc-91-e7-9c-a0-ef-bc-9f/)
[Linux Kernel Reading Guide](http://www.linux.org/threads/linux-kernel-reading-guide.5384/)
[Unreliable Guide To Locking](https://www.kernel.org/pub/linux/kernel/people/rusty/kernel-locking/index.html)
[Speeding up kernel development with QEMU](https://lwn.net/Articles/660404/)
[UBC: An Efficient Unified I/O and Memory Caching Subsystem for NetBSD](https://www.usenix.org/legacy/event/usenix2000/freenix/full_papers/silvers/silvers_html/)
[VFS notes](http://www.fieldses.org/~bfields/kernel/vfs.txt)
# KVM
[KVM源代码分析3:CPU虚拟化](http://www.oenhan.com/kvm-src-3-cpu)
# Human, too human
[Iyo Yoshimi 吉見衣世](https://www.flickr.com/photos/deathhell/8702618520/in/photostream/)
