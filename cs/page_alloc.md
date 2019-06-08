---
tags: [ kernel ] 
layout: post
date: 2019-06-08
title: Linux kernel page allocation
category: cs
---

# GFP flags
__GFP_THISNODE:
9b819d204cf602eab1a53a9ec4b8d2ca51e02a1d - Add __GFP_THISNODE to avoid fallback to other nodes and ignore cpuset/memory policy restrictions
__GFP_IO: allow disk IO
__GFP_FS: allow fs operations, depend on io.
[Avoiding memory-allocation deadlocks](https://lwn.net/Articles/594725/)
[Understanding __GFP_FS](https://lwn.net/Articles/596618/)

# Hot and cold pages
[Hot and cold pages](https://lwn.net/Articles/14768/)
[mm, Remove cold parameter from free_hot_cold_page*](https://patchwork.kernel.org/patch/10013971/)

# Compound pages
[An introduction to compound pages](https://lwn.net/Articles/619514/)
libfc, fcoe: fixes for highmem skb linearize panics:18fa11efc279c20af5eefff2bbe814ca067e51ae
[what is "compound_page()" all about?](https://www.spinics.net/lists/newbies/msg41159.html)


# Obselete feature - Fair-zone allocation
[mm, page_alloc: Remove fair zone allocation policy](https://lore.kernel.org/patchwork/patch/691300/)
[Configurable fair allocation zone policy](https://lwn.net/Articles/576778/)