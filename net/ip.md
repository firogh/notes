---
tags: [ net ] 
title: Network layer
date: 2015-02-27T15:46:13+08:00 
category: net
---

#Network layer
* Error detection, unreliable
Best effort service,IP has a simple error handling algorithm: 
throw away the datagram and try to send an ICMP message back to the source
* Host addressing

# IPv6 wrl6 
__ipv6_addr_type
ip6_input or ip6_output
F1: OK
F2: OK 

# 4.3 IP address
IPv6 wrl6 
__ipv6_addr_type
ip6addrlbl_init_table
ip6_input or ip6_output
ipv6_addr_v4mapped
* 0: 
1 ok IPV6_ADDR_LOOPBACK ::1/128 
2 ok IPV6_ADDR_ANY ::/128
3 ok IPV6_ADDR_MAPPED ::ffff:0:0/96
4 ? IPV6_ADDR_COMPATv4  0000::/96 ipgre_tunnel_xmit ddr_type & IPV6_ADDR_COMPATv4) == 0 ; goto tx_error_icmp;
5 G?  
* 0100::/64, NG, Used by CISCO
A Discard Prefix for IPv6
Discard-Only Address Block
https://tools.ietf.org/html/rfc6666
 Remote Triggered Black Hole (RTBH)
https://tools.ietf.org/html/rfc5635
* 0200::/7,  NG, Deprecated
OSI NSAPs and IPv6
http://tools.ietf.org/html/rfc1888
Internet Code Point (ICP) Assignments for NSAP Addresses
http://tools.ietf.org/html/rfc4548
Interfaces between protocol layers
http://www.erg.abdn.ac.uk/users/gorry/course/intro-pages/sap.html
Network Service Access Point (NSAP): v4,v6?
http://searchnetworking.techtarget.com/definition/Network-Service-Access-Point
* 0400::/6, NG, No information, maybe used as Global address see __ipv6_addr_type
* 0800::/5, NG, ditto
* (000, 111)x::/3, OK, unicasts. For more details please reference __ipv6_addr_type
1000::/4, OK, ditto
2000::/3, OK, Global Unicast, 
2002::/16, OK, SIT, 6in4
http://tools.ietf.org/html/rfc3056
2001::/32, OK, used in Default policy table for routing
2001:10::/28, OK, Ditto
* e000::/4, NG?, No information in google; but used as GU in and kernel by default.
* fc00::/7, OK,
IPV6_ADDR_UNICAST
* fe80::/10, OK, 
IPV6_ADDR_LINKLOCAL
* fec0::/10, OK, But deprecated by RFC3879, used in kernel?
IPV6_ADDR_SITELOCAL
Deprecating Site Local Addresses
http://tools.ietf.org/html/rfc3879
* ff00::/8, OK
IPV6_ADDR_MULTICAST
http://tools.ietf.org/html/rfc4291
* addr not described in __ipv6_addr_type working as global unicast

#IP
* IP Packet Fragmentation/Defragmentation
* MSS tcp_sock->mss_cache in tcp_sync_mss not minus SACK option
	in *tcp_current_mss* minus SACK option
rfc1122
+ IP option is  fixed in a session icsk->icsk_ext_hdr_len;
+ is network header icsk->icsk_af_ops->net_header_len
+ tcp_sock->tcp_header_len all except SACK option (Not sure)
##Reference
[What’s wrong with IPv4 and Why we are moving to IPv6](http://www.tecmint.com/ipv4-and-ipv6-comparison/)

## Classless Inter-Domain Routing
CIDR is a method for allocating IP addresses and routing Internet Protocol packets. 
IETF introduced CIDR in 1993 to replace the classful network.
* prefix/length
* Prefix aggregation

##Supernetwork
prefix/route aggregation
decrease the memroy and the time of search route table.

## Private network
In the Internet addressing architecture, a private network is a network that uses private IP address space.

## Fragmention/defragmention
iphdr->id, iphdr->frag_off
skb_shared_info->frag_list 
ip_fragment/ip_defrag
[Updated Specification of the IPv4 ID Field](http://tools.ietf.org/html/rfc6864)
## SG IO
如果device support NETIF_F_SG 直接copy_form user msghdr to frgs[] zero copy!
p_append_data
[NETIF_F_FRAGLIST and NETIF_F_SG difference](http://thread.gmane.org/gmane.linux.network/153666)
validate_xmit_skb()->__skb_linearize()
ip fragment 不是为了fraglist而是把skb变小. 所以这里可能有问题linearize后skb过大.
如果经过ip_fragment应该,不会出现, 自己倒腾的就可能.

# send
ip_append_data 和ip_push_pending_frames弄frag_list
ip_push_pending_frames -> __ip_make_skb & ip_send_skb ->ip_local_out
把&sk->sk_write_queue上的数据最后编程skb链表变成了, 还skb pull掉了潜在的ip 头部
第一个skb->frag_list的成员. 用的不太多啊.
ip_append_data中间出了以为如果可以ufo 那么就到frags的碗里去!
否则就生成一串skb挂到&sk->sk_write_queue上, 
