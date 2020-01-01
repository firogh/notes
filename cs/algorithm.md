---
tags: [ cs ] 
title: Algorithms and data structues
date: 2015-05-27T12:42:12+08:00 
category: cs
---
# Reference
[Core algorithms deployed](http://cstheory.stackexchange.com/questions/19759/core-algorithms-deployed#)
Algorithms: Design Techniques and Analysis
The Algorithm Design Manual 2nd Edition

# Leetcode
## 1p3c
http://www.1point3acres.com/bbs/thread-270312-1-1.html
最终我觉得像word search12， word break12，word ladder12，LIS，sort color，LRU，insert & delete in O1，rob house123，234sum这种题要达到闭眼秒杀的程度，min/max heap，bucket sort，topological sort，binary pre/in/post/level 遍历，combination/permutation这种东西要做梦都梦到
最后祝lz好运，加油~ 
## Why do we have to solve leetcode problems?
尤其是在北美，Google，Facebook，Microsoft，Amazon 等等大公司，无一不考刷题，以算法面试为主。而无论是北美留学生，还是工作几年的上班族，想进大公司，唯一的出路就是刷题。
[Leetcode 分类顺序表第二版](https://cspiration.com/leetcodeClassification)

# Computational complexity theory
http://bigocheatsheet.com/

# Core conceptions
* representations
[Linked vs sequential](https://www.geeksforgeeks.org/binary-tree-array-implementation/)
* ADT vs data structure
ADT is a data type defined by its behavior. 
[Any type that does not specify an implementation is an abstract data type.](https://en.wikipedia.org/wiki/Data_type#Abstract_data_types)

# Linked list
## Static linked list
Reprented in an array.
## Internal vs external liked
Sometimes, SLUB put freelist in object

## kernel doubly linked list operations
### add
* kernel version
next->prev = new;
new->next = next;
new->prev = prev;
prev->next = new;
### delete
* kernel version
next->prev = prev;
WRITE_ONCE(prev->next, next);
entry->next = LIST_POISON1;
entry->prev = LIST_POISON2;
# BL list
kernel: add bl_list - 4e35e6070b1ceed89c3bba2af4216c286fb1dafd

# Double linked list

# Associative array
[Essentials: Brian Kernighan on Associative Arrays - Computerphile](https://www.youtube.com/watch?v=qTZJLJ3Gm6Q)
vs indexed array
## Associativity
* trade-off
a) Checking more places takes more power and chip area, 
b) and potentially more time. On the other hand, caches with more associativity suffer fewer misses
fully associative - the best miss rates, but practical only for a small number of entries
N-way set associative cache: 8 is a common choice for later implementations
direct-mapped cache - if two locations map to the same entry, they may continually knock each other out. anti-fragmantion worsens this case.

# Judy array
http://judy.sourceforge.net/

# Hashtable
[A generic hash table](https://lwn.net/Articles/510202/)
hash function

# BST
Pre-order
In-order traversal
Post-order
postfix and prefix and sort


# Graph

# Depth first sarch
DAG

# Interval tree in kernel
anonymous page: anon_vma_interval_tree_insert

# Trie
Trie is prefix tree.
Trees only store keys.
[Trie Data Structure](https://www.youtube.com/watch?v=AXjmTQ8LEoI)
[Trie with numeric key](https://www.youtube.com/watch?v=MC-iQHFdEDI)
* terms very confused
[Radix tree vs Trie, check radix meaning](https://stackoverflow.com/questions/14708134/what-is-the-difference-between-trie-and-radix-trie-data-structures)
Patricia is compact trie or Patricia is radix = 2 trie?

# Radix tree in kernel not wikipedia
page cache: page_cache_tree_insert
Wikipedia: Radix tree looks like a compact trie.
Kernel: Radix tree was more like a Multi-level index associative arrya or judy array.
[Trees I: Radix trees](https://lwn.net/Articles/175432/)
[Enhancing the Linux Radix Tree](http://events17.linuxfoundation.org/sites/events/files/slides/LinuxConNA2016%20-%20Radix%20Tree.pdf)
[The design and implementation of the XArray](https://www.youtube.com/watch?v=v0C9_Fp-co4)
[A multi-order radix tree](https://lwn.net/Articles/688130/)
radix_tree_init_maxnodes(): height is 11 in kernel?
__radix_tree_create() add one page__
radix_tree_lookup_slot: find one page

# Search
Data property: unique key, indexed
[Search data structure](https://en.wikipedia.org/wiki/Search_data_structure)
Sequencial array: binary search
Associative array
BST
Hashtable
## Which algorithm?
[Advantages of BST over Hash Table](https://www.geeksforgeeks.org/advantages-of-bst-over-hash-table/)
1. Can get all keys in sorted order  by just doing in-order traversal of BST
2. Doing order statistics, finding closest lower and greater elements, doing range  queries  are easy to do with BSTs.
3. BSTs are easy to implement compared to hashing.
4. With Self Balancing BSTs, all operations are guarnateed to work in O(logN) time.

# Replacement polices
Pseudo-LRU

# LRU
[Leetcode 146 LRU cache](https://leetcode.com/problems/lru-cache/description/)
Order items by access times
## Pseudo LRU/2 - Second chance and queue
type: Reclaim
Order items by enqueueing sequence
dcache
## Second chance and [2Q](https://pdfs.semanticscholar.org/d62d/e5f995164fff50f5ce61c0113f6bc9f04225.pdf)
Page reclaim algorithm
type: Reclaim

# Ring buffer or Circular buffer

# Redblack tree
gap between linar node can be optimized by argument rb tree. O(n) -> O(log n)
mm: augment vma rbtree with rb_subtree_gap d37371870ceb1d2165397dc36114725b6dca946c
[Rank-Balanced Trees](http://sidsen.azurewebsites.net//papers/rb-trees-talg.pdf)
https://leetcode.com/problems/count-of-smaller-numbers-after-self/description/
http://yuex.in/post/2017/08/red-black-tree-in-action.html
[Left-Leaning Red-Black Trees](https://www.cs.princeton.edu/~rs/talks/LLRB/08Dagstuhl/RedBlack.pdf)
## History
AVL tree, B-tree, symmetric binary B-tree or 2–3–4 tree, red–black tree
[2-3-4 Trees and RedBlack Trees](https://www.cs.purdue.edu/homes/ayg/CS251/slides/chap13b.pdf)
