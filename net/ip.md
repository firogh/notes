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

# IP header
rfc1122
IP option is fixed in a session icsk->icsk_ext_hdr_len;
is network header icsk->icsk_af_ops->net_header_len
