---
tags: [ kernel ] 
layout: post
date: 2019-06-08
title: Linux kernel page allocation
category: cs
---

# GFP flags
__GFP_THISNODE: 9b819d204cf602eab1a53a9ec4b8d2ca51e02a1d - Add __GFP_THISNODE to avoid fallback to other nodes and ignore cpuset/memory policy restrictions
__GFP_HIGHMEM in __alloc_zeroed_user_highpage??

# Do anonymous page, zero page
tglx tree
commit 382a7dec462a90ad6ae01227f1e8758ae721f6ed
Author: Christoph Lameter <clameter@sgi.com>
Date:   Tue Feb 1 16:34:17 2005 -0800
    [PATCH] alloc_zeroed_user_highpage() to fix the clear_user_highpage issue
Lost tracks
commit 6fbaac38b85e4bd3936b882392e3a9b45e8acb46
Author: Linus Torvalds <torvalds@athlon.transmeta.com>
Date:   Mon Feb 4 19:14:54 2002 -0800

    v2.4.7 -> v2.4.7.1
# page
_count set_page_count


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
