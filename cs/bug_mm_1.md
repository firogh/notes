---
tags: [ cs ] 
title: Kernel memory bug - SLAB's 3 lists are corrupted.
date: 2019-01-02
category: cs
---

Recently, I was working on a kernel memory bug. 

https://apibugzilla.suse.com/show_bug.cgi?id=1118875
L3: kernel BUG at ../mm/slab.c:2804! bad LRU list and active values in page structs in possible use-after-free

After digging the binary vmcore file of kdump, I got the following findings.
# Node 0
## Partial
list page.lru  -H 0xffff8801a7c01348 -s page.lru,s_mem,active,slab_cache,flags >n0p.log
n0p -> n0f=0xffff8801a7c01358
## Full
list page.lru  -H 0xffff8801a7c01358 -s page.lru,s_mem,active,slab_cache,flags >n0f.log
n0f -> 
ffffea0006902380
    lru = {
      next = 0xffffea0080ed53e0,
      prev = 0xffffea00405f8ae0
    }
    s_mem = 0xffff8801a408e000
      active = 16
    slab_cache = 0xffff8801a7c00400
  flags = 6755398367314048
ffffea0080ed53c0
    lru = {
      next = 0xffffea00422a34e0,
      prev = 0xffffea00069023a0
    }
    s_mem = 0xffff88203b54f000
      active = 7
    slab_cache = 0xffff8801a7c00400
  flags = 24769796876796032
... -> n1f = 0xffff881107c00358 

# Node 1
## Partial
crash> list page.lru  -H 0xffff881107c00348 -s page.lru,s_mem,active,slab_cache,flags >n1p.log
nip-> SLAB ffffea0043ab74e0 -> 0xffff881107c00348 = n1p
SLAB ffffea0043ab74e0's prev pointing to 0xffff881107c00358
## Full
crash> list page.lru  -H 0xffff881107c00358 -s page.lru,s_mem,active,slab_cache,flags >n1f.log
n1f-> SLAB ffffea0043ab74e0  -> ... -> 0xffff881107c00348 = n1p

This issue occured on a NUMA system with 2 memory nodes.
Both node 0 and node 1's SLAB's partial and full lists were corrupted. After looking into this issue a few days, I talked to Vlastimil Babka.
He provided a fix for this issue. That is 7810e6781e0fcbca78b91cf65053f895bf59e85f - mm, page_alloc: do not break __ GFP_THISNODE by zonelist reset.

Now, I have a question: why did I cannot solve this issue?









