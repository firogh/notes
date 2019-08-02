---
tags: [ net ] 
title: Network layer
date: 2015-02-27T15:46:13+08:00 
category: net
---

# Fragmentation
ip_defrag -> ip_frag_queue -> ip_frag_reasm
ip_fragment
iphdr->id, iphdr->frag_off

# Route
[IPv4 route lookup on Linux](https://vincent.bernat.im/en/blog/2017-ipv4-route-lookup-linux)
* state structure
fib_info:route info
fib_config:
* add new rule
iproute2 ...->inet_rtm_newroute()->fib_new_table()->fib_hash_table()
* Multi-time line
fib_create_info(): create a fib_info
# dst
[remove dst garbage collector logic](https://lore.kernel.org/netdev/20170617174244.132862-1-tracywwnj@gmail.com/)
commit 222d7dbd258dad4cd5241c43ef818141fad5a87a
Author: Eric Dumazet <edumazet@google.com>
Date:   Thu Sep 21 09:15:46 2017 -0700
    net: prevent dst uses after free
    In linux-4.13, Wei worked hard to convert dst to a traditional
    refcounted model, removing GC.
commit 9e1437937807b0122e8da1ca8765be2adca9aee6
Author: Steffen Klassert <steffen.klassert@secunet.com>
Date:   Tue Sep 11 10:31:15 2018 +0200
    xfrm: Fix NULL pointer dereference when skb_dst_force clears the dst_entry.

# IP header
rfc1122
IP option is fixed in a session icsk->icsk_ext_hdr_len;
is network header icsk->icsk_af_ops->net_header_len
