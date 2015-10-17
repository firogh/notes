---
date: 2015-05-11
title: 如何使用RCU in linux kernel
category: howto
---

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

