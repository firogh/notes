---
tags: [ net ] 
title: Understanding linux netfilter
date: 2015-05-10T15:46:13+08:00 
category: net
---

# Filter
conditions and actions
# Packet filter
type: filter
## What packets and where to filter
* Filter rules
1. Condition
IP addr
Port
Level 4 protocol type
2. Action
accept
drop

* Filter checkpoints
Ingress
Outgress
Route

# Hooking
Interceptor pattern

# Connection tracking
[Netfilter’s connection tracking system](http://people.netfilter.org/pablo/docs/login.pdf)
[netfilters connection tracking subsystem](https://www.netdevconf.org/2.1/session.html?westphal)
[Network Probes Explained: Understanding Port Scans and Ping Sweeps](https://www.linuxjournal.com/article/4234)

# Netfilter
a set of hooks: Local_in local_out forward pre_routing post_routing
packet filter
Connection tracking: 
NAT

# Software implementation
[Overview of xtables in wikipedia](http://en.wikipedia.org/wiki/Iptables#Overview)
* Init
net/netfilter/core.c
netfilter_init()
如mangle表的初始化见iptable_mangle_init
xt_table: ipt_register_table
        struct xt_table         *iptable_filter;
        struct xt_table         *iptable_mangle;
        struct xt_table         *iptable_raw;
        struct xt_table         *arptable_filter;
        struct xt_table         *iptable_security;
        struct xt_table         *nat_table;
如mark match的初始化 mark_mt_init
xt_match: xt_register_match

如MARK target 的初始化 也在 mark_mt_init
xt_target: xt_register_target

* Nuclus
netfilter hook ->  iptable_mangle_hook -> ipt_do_table ->...
ipt_do_table() 就是内核处理nat, filter, mangle的公用函数.

# Connection tracking
Conntrack 的实现不依赖iptables, 很独立.
## init 
nf_conntrack_standalone_init()
nf_conntrack_l3proto_ipv4_init()
## conntrack -- user-land tools
obsolete /proc/net/nf_conntrack
## tuple -- link a socket 5-arry tuple
Each Netfilter connection is uniquely identified by a 
(layer-3 protocol, source address, destination address, layer-4 protocol, layer-4 key) tuple
nf_conntrack_tuple nf_conn
## Connection tracking helper
connection tracking can be given knowledge of application-layer protocols
ALG

## template
netfilter: nf_conntrack: support conntrack templates

## Details 
以上的工作事实上都很简单，基本思路是：
一个包来了，转换其tuple，看其在连接跟踪表中没有，有的话，更新其状态，以其做一些与协议相关的工作，如果没有，则分配一个新的连接表项，并与skb_buff关连，但是问题是，这个表项，还没有被加入连接表当中来。其实这样做的理由很简单，因为这个时候，这个包是否有机会活命还是个未知数，例如被其它模块给Drop了……所以，要等到一切安全了，再来将这个表项插入至连接跟踪表。
这个“一切安全”当然是Netfilter所有的模块处理完了，最完全了。
徐琛,也这么说!

# NAT
https://www.ietf.org/rfc/rfc3489.txt
symmetric nat, 端口不复用, 访问同一个服务器.
linux 内核的NAT是基于iptables 和 conntrack实现的.

##init
iptable_nat_init

## NAT helper
Similar to connection tracking helpers, NAT helpers will do a packet inspection 
and substitute original addresses by reply addresses in the payload.

## Drop ICMP redict in NAT
http://www.netfilter.org/documentation/HOWTO/NAT-HOWTO-10.html

## SNAT
nf_nat_ipv4_out -> nf_nat_ipv4_fn -> 
{
nf_nat_rule_find -> ipt_do_table -> xt_snat_target_v1 -> nf_nat_setup_info 
	{
		无论是SNAT, 还是DNAT,改的都是ct的reply. 所以这里先把 orig_rely的对应的orig_original形式弄出来.
		但是,必须要保证改skb的真实值要保证source 唯一, orig_original -> new_original找到后再revert,成new_reply在改到ct里面去.
		orig_orignal->skb
		nf_ct_invert_tuplepr(inverse, orig_relply)
		{
			ipv4_invert_tuple
			tcp_invert_tuple
			For example, orig tuple:
			original: 192.168.199.132 -> google.com
			reply: google.com -> 192.168.199.132 //this is orig_relpy

			inverse tuple:
			original(inverse->dst.dir = !orig->dst.dir;):
			192.168.199.132 -> google.com (!!!reverse orig_reply in ipv4_inver_tuple())
			 这个函数的用途可能是担心, orig被人改了, 不能用了.
			except for prior manipulations
		}		

		get_unique_tuple
		{
			1. 如果snat, 且前后可以一致就直接new=orig, 合理.
			2. find_appropriate_src 费点力... 貌似找到已经用到的, 复用
			3. find_best_ips_proto, 找一个 the least-used IP/proto combination in the given range
			4. nf_nat_used_tuple 保证唯一
		}		

		bysoruce 里面存的应该是new_original, hash -> &net->ct.nat_bysource[srchash]


	}
//上面ct改完了该改skb了.
	nf_nat_packet -> nf_nat_ipv4_manip_pkt,
}

* SNAT nftables
nf_nat_prerouting ...-> nft_do_chain 

+ One kind of NAT, just set one flag bit in ct->status (SRC_NAT or DST_NAT), but set both SRC/DST_DONE!

* De-NAT
最简单的路由器 在postrouting 做了snat（masquade）那么回来的报文怎么unsnat呢？
我看了九贱的帖子，一笔带过了。 我不太懂的地方是在nat_packet这个函数里面在发现是rely的报文，要判断ct→status & IPS_DST_NAT 为真 才修改skb里的IP port，我不清楚reply的报文何时给ct→status打的DST_NAT的标记位，看代码好象是prerouting的ip_nat_setup_info这个函数，可是我看到必须改了ct的tuple才能给ct→status打标记位，反复的修改ct，我觉得自己想的不对。


*答案3.17的代码和原来没多大变化就是函数名字变了
发包-POSTROUTING ->SNAT ->修改ct: nf_nat_setup_info->　ct->status |= IPS_SRC_NAT;->修改skb:nf_nat_packet

收报-PREOUTING-> DNAT->修改skb:nf_nat_packet 
{
    enum nf_nat_manip_type mtype = HOOK2MANIP(hooknum);   
	//因为是在PREROUTING, 所以是DNAT, 我以前一直以为, de-snat在postrouting中做的. 
                                           
    if (mtype == NF_NAT_MANIP_SRC)          
        statusbit = IPS_SRC_NAT;             
    else                                      
        statusbit = IPS_DST_NAT;        //到这里
                                                
    /* Invert if this is reply dir. */            
    if (dir == IP_CT_DIR_REPLY) 
		statusbit ^= IPS_NAT_MASK;        //翻转一下变成SNAT 
    /* Non-atomic: these bits don't change. */                                                                                                    
    if (ct->status & statusbit) {                 
	//正好和发包是的   ct->status |= IPS_SRC_NAT;匹配了, 开始de-snat.                    
		struct nf_conntrack_tuple target;
		...
}

#ipset
salist for iptables

#SYN proxy
SYNPROXY target makes handling of large SYN floods possible without 
the large performance penalties imposed by the connection tracking in such cases. 
On 3 November 2013, SYN proxy functionality was merged into the Netfilter, 
with the release of version 3.12 of the Linux kernel mainline

#nftables

#FAQ
* 如何查看某个table 具体在那几个hook点.
iptables -L -t "table 名"

# Reference
[Linux netfilter Hacking HOWTO](http://www.netfilter.org/documentation/HOWTO//netfilter-hacking-HOWTO.html)
[NAT - Network Address Translation](http://www.karlrupp.net/en/computer/nat_tutorial)
