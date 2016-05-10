---
tags: [ cs ] 
title: Algorithms and data structues for kernel hacking
date: 2015-05-27T12:42:12+08:00 
category: cs
---

# Reference
[Core algorithms deployed](http://cstheory.stackexchange.com/questions/19759/core-algorithms-deployed#)
Algorithms: Design Techniques and Analysis
# Process
The Algorithm Design Manual 2nd Edition


# 世界本源, the origin of the world, L'Origine du monde
being(4), abyss(0)
material(1) change, quality, quantity, relation, space, time
process(2), tansfer, truth, signifier(3), bijection, Causality, structure

# 排序算法的分析
排序的结果全序.
在确定排序的思想/算法后, 排序的过程是从无序to 有序的过程.
我们排序的过程等价成一次算术运算.
每一次, 在特定的算法下, 数据有序度增加or不增加, 但不减少.
我们把有序度看成一个quantity.那么此时排序的过程等价于有序度增加的过程.
怎么定义有序度set内元素构成=<就是一度粗有序度. 严谨的连续元素构成=<关系.
比如1 2 3 4 5 , bold order:4 + 3+ 2 +1; 严谨有序度:４

也就是一个算法, 对如3 2 5 4 1. bo:2 + 2 ; 严谨有序度:0
bo: 2+2 to 10
严谨: 0 to 4
就算完成.此时我们没做一次排序, 都在做加法运算.只是每一步转换成一个变量了.
这似乎对我们理解sorting起不了多大的帮助.
直觉应该没错.确实不应该关心过程, 而应该关系变化本身, process不等于变化本身.
只是process是变化的动力, 却不是原因.变化的因果并不是恒定不变的.process成了变化的
结果了, 因为要这样所以才有了process, 不是这个process才怎么怎么了.
这样我们知道从raw set to ordered set, 这个过程, 一定可以有多个sort algorithms来完成.
那么选取哪个sort algorithm就是有改变的形式决定的.
所以我们依然在探讨结构上的变化. 而不是纯粹的process过程.
比如先确定边界元素的有序度, 从一边向另一边逐渐演化. 这就是selection sort.
注意!我们通过等价变换causlity, 原来近乎process主导的问题变为了, quantity structure渐变的问题了.
原来是伸手不见五指的变, 现在是看的见有形的变.
再看其他的算法, 比如我们想确定由中间quantity发起向两侧的有序变化, 就是快排.快排为什么快?
但是我们发现, 这种有中间发起的有序度变多的过程, 有两个疑点.
1. 这个所谓中间值是我们随机指定的, 几乎不可能是真正意义上的"中间值".
2. 也是疑惑最大, 这种中起的有序度变化, 严格有序度不像边沿变化那样连续, 但是关于所谓中间值的bold order
确是丝毫没有耗损的! 也就是bold order是非常细腻的, 而Strict order, 是非常粗野的.
我们可以武断地下个定论, 满足严格有序的算法效率差!后面会给出严谨的证明.
我们继续, 另外务必注意, 我们这里没有任何关于已知算法的信息.
已知的各种算法是就是符号系统里面的signifier, 在我们分析的过程中纯粹的就是个名字而已.
更可怜的是, 这个名字是我们从signified推出来的.
我们现在已经推出了冒泡, 选择, 快排 三种算法了.
另外还有merge sort, heap sort, insertion sort, shell sort.
我只是看了merge sort之后的名字了, 具体的算法, 内容我都忘了(刻意不去想).











