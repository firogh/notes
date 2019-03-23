---
tags: [ kernel ] 
title: The Journey to RCU
date: 2015-05-24T09:52:12+08:00 
category: cs
---

# RCU
## Causality
Read Copy Update HOWTO介绍了当初RCU开发的动机:
## Structure
[What is RCU?](http://www.rdrop.com/users/paulmck/RCU/whatisRCU.html)

# Classic RCU
commit c17ef45342cc033fdf7bdd5b28615e0090f8d2e7
Author: Paul E. McKenney <paulmck@linux.vnet.ibm.com>
Date:   Tue Jun 23 17:12:47 2009 -0700
    rcu: Remove Classic RCU
[Brief Overview of Classic RCU Implementation](https://lwn.net/Articles/305782/#Brief%20Overview%20of%20Classic%20RCU%20Implementation)
[Linux2.6.11版本：classic RCU的实现](http://www.wowotech.net/kernel_synchronization/linux2-6-11-RCU.html)

# Grace period
Documentation/RCU/rcu.txt
[URCU: any period of time during which each reader thread resides in at least one quiescent state is called a grace period.](https://lwn.net/Articles/573424/)
[Start a New Grace Period](https://lwn.net/Articles/305782/#Start%20a%20new%20grace%20period.)
[... after each CPU has passed through at least one quiescent state, the RCU grace period ends.](https://lwn.net/Articles/305782/#Brief%20Overview%20of%20Classic%20RCU%20Implementation)

# Quiescent state
[... after all the CPUs in the system have gone through at least one "quiescent" state (such as context switch, idle loop, or user code)](http://lse.sourceforge.net/locking/rcu/HOWTO/descrip.html)
[URCU: Any line of code not in an RCU read-side critical section is termed a quiescent state](https://lwn.net/Articles/573424/)
[The rcu and rcu_bh flavors of RCU have different sets of quiescent states.](https://lwn.net/Articles/305782/#Pass%20through%20a%20quiescent%20state.)

# granularity of waiting
[The great advantage of RCU ... without having to explicitly track each and every one of them](https://lwn.net/Articles/262464/#Wait%20For%20Pre-Existing%20RCU%20Readers%20to%20Complete) 
In RCU's case, the things waited on are called "RCU read-side critical sections". ditto.

# Preemptiable RCU
[The design of preemptible read-copy-update](https://lwn.net/Articles/253651/)
[Realtime RCU](http://www.rdrop.com/users/paulmck/RCU/realtimeRCU.2005.04.23a.pdf)

# SRCU 
[Sleepable RCU](https://lwn.net/Articles/202847/)

# RCU: The Bloatwatch Edition
[RCU: The Bloatwatch Edition](https://lwn.net/Articles/323929/)

# Hierarchical RCU / Tree RCU
[Hierarchical RCU](https://lwn.net/Articles/305782)
[Tree RCU Grace Period Memory Ordering Components ](https://www.kernel.org/doc/Documentation/RCU/Design/Memory-Ordering/Tree-RCU-Memory-Ordering.html)


# RCU API
[RCU part 3: the RCU API, 2008 edition](https://lwn.net/Articles/264090/)
[The RCU API, 2010 Edition](https://lwn.net/Articles/418853/)
[The RCU API, 2014 Edition](https://lwn.net/Articles/609904/)
[The RCU API, 2019 edition](https://lwn.net/Articles/777036/)

# RCU stalls
[Decoding Those Inscrutable RCU CPU Stall Warnings](https://www.youtube.com/watch?v=23_GOr8Sz-E)
update_process_times->rcu_check_callbacks->rcu_pending->__rcu_pending->check_cpu_stall->print_other_cpu_stall
Documentation/RCU/stallwarn.txt


# Reference
<iframe frameborder="no" border="0" marginwidth="0" marginheight="0" width=330 height=86 src="http://music.163.com/outchain/player?type=2&id=444737&auto=1&height=66"></iframe>
[官网Read-Copy Update Mutual Exclusion](http://lse.sourceforge.net/locking/rcupdate.html)
[Read Copy Update HOWTO](http://lse.sourceforge.net/locking/rcu/HOWTO/index.html)
[Read-Copy Update Mutual-Exclusion in Linux](http://lse.sourceforge.net/locking/rcu/rcupdate_doc.html)
[Thanh Do's notes Read-copy update. In Ottawa Linux Symposium, July 2001](http://pages.cs.wisc.edu/~thanhdo/qual-notes/sync/sync2-rcu.txt)


# How to use RCU
list rcu
[What is RCU? Part 2: Usage](https://lwn.net/Articles/263130/)
[RCU Usage In the Linux Kernel: One Decade Later](http://www2.rdrop.com/users/paulmck/techreports/RCUUsage.2013.02.24a.pdf)

# What is RCU
[What is RCU, Really?](http://www.rdrop.com/~paulmck/RCU/whatisRCU.html)
[What is RCU, Fundamentally?](https://lwn.net/Articles/262464/)
[RCU part 3: the RCU API](http://lwn.net/Articles/264090/)
RCU supports concurrency between a single updater and multiple readers!
## Design pattern
* Publish-Subscribe 
For updater before synchnization_rcu(); similar to softirq rcu
* Observer 
For updater to wait For Pre-Existing RCU Readers to Complete
Maintain Multiple Versions of Recently Updated Objects

# Positivism Implementions
## Classic RCU
Classic RCU requires that read-side critical sections obey the same rules 
obeyed by the critical sections of pure spinlocks: 
blocking or sleeping of any sort is strictly prohibited.
## Preemtible RCU
[The design of preemptible read-copy-update](http://lwn.net/Articles/253651/)
stage 的切换是由rcu_read_unlock 推动的!

## Tree RCU 
For hundreds and thounds CPU cores

#FAQ
* Difference with primitive and atomic
* Grace period is synchroinze_rcu or a softirq justment
* __rcu sparse will warn you if you access that pointer without the services of one of the variants of rcu_dereference().
# Below is how to use RCU!
# Reference
[RCU Usage In the Linux Kernel: One Decade Later](http://www2.rdrop.com/users/paulmck/techreports/RCUUsage.2013.02.24a.pdf)
# Introduction
本文内容并非我原创, 实际上是翻译加上个人理解Paul E. McKenney的paper.Credit to Paul and others.
本文不关心RCU的实现. 
本文的核心目标就是了解如何使用RCU.
所有使用RCU代码, 都可以分为4部分:
RCU read critical section: 就是rcu_read_lock()和rcu_read_unlock()之间的代码.
update: 这部分代码在synchronize_rcu/call_rcu之前执行. 比如给pointer = new_data;
synchronize: 这部分代码是等待调用synchronize/call_rcu时的所有Rcu read critical section 执行完.
free: 这部分是在上面synchronize/call_rcu确认所有对就数据引用的RCU read critical section都执行完了, 安全释放内存.
Paul的paper总结了5种用法, 并且附上了代码, 便于理解.
理解的过程实质上是用RCU的外衣再次理解这个5种方法!

# Wait for Completion
内核中有[完成量completion](https://lwn.net/Articles/23993/)的基础设施
上面的RCU read critical section和synchronize可以构成completion的语义.
为什么要用RCU而不是内核自身提供的completion函数呢?
Paul的文中举的例子实在NMI中要作这样的事情, 显然基于waitqueue的completion是毫无用处的.
所以这里的RCU, 应该叫做Completion RCU-version.

# Reference Counting
传统的引用计数就是往结构体里面内嵌一个atomic,加加减减.
但RCU实现的引用计数把实际工作转移到softirq-rcu部分去了.
Paul给了一个例子, 我认为是及其不合适的.因为他忽略了softirq部分随着smp的核数增多
同样也会带来性能的损耗,而不是almost zero in his paper.
引用计数和完成量的区别, 除了语义上的差异, 看起来没有太大的区分.FIXME.

# Type Safe Memory
 
# Publish-Subscribe

# Read-Write Lock Alternative 

