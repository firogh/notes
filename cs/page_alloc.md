---
tags: [ kernel ] 
layout: post
date: 2019-06-08
title: Linux kernel page allocation
category: cs
---

# Reference
[Driver porting: low-level memory allocation]https://lwn.net/Articles/22909/)

# History

# Formal causes
## Hot and cold pages
[Hot and cold pages](https://lwn.net/Articles/14768/)
[mm, Remove cold parameter from free_hot_cold_page*](https://patchwork.kernel.org/patch/10013971/)

## Compound pages
tglx: commit eefb08ee7da81e1548ffd5b664682dc5b229ddc2
Author: Andrew Morton <akpm@digeo.com>
Date:   Wed Feb 5 16:57:54 2003 -0800
    [PATCH] Infrastructure for correct hugepage refcounting
[An introduction to compound pages](https://lwn.net/Articles/619514/)
libfc, fcoe: fixes for highmem skb linearize panics:18fa11efc279c20af5eefff2bbe814ca067e51ae
[what is "compound_page()" all about?](https://www.spinics.net/lists/newbies/msg41159.html)

# GFP flags
__GFP_THISNODE: 9b819d204cf602eab1a53a9ec4b8d2ca51e02a1d - Add __GFP_THISNODE to avoid fallback to other nodes and ignore cpuset/memory policy restrictions
__GFP_HIGHMEM in __alloc_zeroed_user_highpage??

# Do anonymous page, zero page
tglx: commit 382a7dec462a90ad6ae01227f1e8758ae721f6ed
Author: Christoph Lameter <clameter@sgi.com>
Date:   Tue Feb 1 16:34:17 2005 -0800
    [PATCH] alloc_zeroed_user_highpage() to fix the clear_user_highpage issue
Lost tracks
commit 6fbaac38b85e4bd3936b882392e3a9b45e8acb46
Author: Linus Torvalds <torvalds@athlon.transmeta.com>
Date:   Mon Feb 4 19:14:54 2002 -0800
    v2.4.7 -> v2.4.7.1
# Obselete feature - Fair-zone allocation
[mm, page_alloc: Remove fair zone allocation policy](https://lore.kernel.org/patchwork/patch/691300/)
[Configurable fair allocation zone policy](https://lwn.net/Articles/576778/)

# Watermarks
Check Documentation/sysctl/vm.txt for min_free_kbytes
min_free_kbytes_sysctl_handler or watermark_scale_factor_sysctl_handler or
core_initcall(init_per_zone_wmark_min) ->
        setup_per_zone_wmarks-> __setup_per_zone_wmarks
{
        firo@linux-6qg8:~> grep managed /proc/zoneinfo
                managed  3973
                managed  464142
                managed  7726451
        >>> 3973 + 464142 + 7726451
        8194566
        firo@linux-6qg8:~> cat /proc/sys/vm/min_free_kbytes
        67584
        >>> 67584 / 4 * 3973 / 8194566
        8
        # Unit of watermark is Page.    
        WMARK_MIN = page_no(min_free_kbytes) * (zone.managed_pages / \Sum of zone.managed_pages)
        WMARK_LOW = 1.25 * min or min + 1/1000 * zone.managed_pages
        WMARK_HIGH = 1.5 * min or min + 2/1000 * zone.managed_pages
}
if min_free_kbytes > 1/250*total mamanged_pages, we use 1.25 min or 1.5 min

# Zone
## Low memory reserved
Check Documentation/sysctl/vm.txt for lowmem_reserve_ratio
lowmem_reserve_ratio_sysctl_handler or core_initcall(init_per_zone_wmark_min) ->
        setup_per_zone_lowmem_reserve
firo@linux-6qg8:~> cat /proc/zoneinfo | grep protection
        protection: (0, 1813, 31994, 31994, 31994)
        protection: (0, 0, 30181, 30181, 30181)
        protection: (0, 0, 0, 0, 0)
        protection: (0, 0, 0, 0, 0)
        protection: (0, 0, 0, 0, 0)
Check __alloc_pages_nodemask(), lowmem_reserve is used for checking if there is enough pages in current zone to which allcation fallbacks from a prefered zone.
lowmem_reserv is used for fallback allcations from a perfered zone in the zonelist.
The index of lowmme_reserv is the prefered zoneref.zone_idx
## Zone lists
struct zonelist node_zonelists[MAX_ZONELISTS];
 * [0]  : Zonelist with fallback
 * [1]  : No fallback (__GFP_THISNODE)
start_kernel->
        build_all_zonelists
or hotpulg or /proc/sys/vm/numa_zonelist_order: numa_zonelist_order_handler
  node_zonelists = {{
      _zonerefs = {{
          zone = 0xffff88107ffd5d80, # node 0
          zone_idx = 2
          zone = 0xffff88107ffd56c0, # node 0
          zone_idx = 1
          zone = 0xffff88107ffd5000, # node 0
          zone_idx = 0
          zone = 0xffff88207ffd2d80, # Node 1; fallback.
          zone_idx = 2
          zone = 0x0,
          zone_idx = 0

# Material
## _refcount
after __alloc_pages_nodemask _refcount=0,
post_alloc_hook: init should be 0.
but set_page_refcounted -> set_page_count(page, 1);
and Check put_page
