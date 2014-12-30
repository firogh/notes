
#netfilter hooks priorities
    NF_IP_PRI_RAW = -300,
    NF_IP_PRI_SELINUX_FIRST = -225,
    NF_IP_PRI_CONNTRACK = -200,
    NF_IP_PRI_MANGLE = -150,
    NF_IP_PRI_NAT_DST = -100,
    NF_IP_PRI_FILTER = 0,
    NF_IP_PRI_SECURITY = 50,
    NF_IP_PRI_NAT_SRC = 100,
=Answer 
http://www.netfilter.org/documentation/HOWTO/packet-filtering-HOWTO-9.html
9. Mixing NAT and Packet Filtering

#netfilter framwork
=Hook point
local_in local_out
pre_routing post_routing

=Files higherach
net/ipv4/netfilter
net/netfilter
net/ipv6/netfilter
=Init
~/linux/net/netfilter/core.c
netfilter_init()

=Build function
nf_register_hooks
nf_ct_l4proto_register
nf_ct_l3proto_register


#Connection tracking
=Init Build ipv4
nf_conntrack_l3proto_ipv4_init()
struct ipv4_conntrack_ops
ipv4_conntrack_in
ipv4_conntrack_local,
ipv4_helper,
ipv4_confirm,

#NAT
=Init Build
~/linux/net/ipv4/netfilter/iptable_nat.c
iptable_nat_init()
struct nf_nat_ipv4_ops
nf_nat_ipv4_in,
nf_nat_ipv4_out,
nf_nat_ipv4_local_fn, local_out
nf_nat_ipv4_fn, local_in

=Drop ICMP redict in NAT
http://www.netfilter.org/documentation/HOWTO/NAT-HOWTO-10.html

=SNAT iptables  POST_ROUTING
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

=SNAT nftables
nf_nat_prerouting ...-> nft_do_chain 

=Tips
+ One kind of NAT, just set one flag bit in ct->status (SRC_NAT or DST_NAT), but set both SRC/DST_DOWN!

=De-NAT
最简单的路由器 在postrouting 做了snat（masquade）那么回来的报文怎么unsnat呢？
我看了九贱的帖子，一笔带过了。 我不太懂的地方是在nat_packet这个函数里面在发现是rely的报文，要判断ct→status & IPS_DST_NAT 为真 才修改skb里的IP port，我不清楚reply的报文何时给ct→status打的DST_NAT的标记位，看代码好象是prerouting的ip_nat_setup_info这个函数，可是我看到必须改了ct的tuple才能给ct→status打标记位，反复的修改ct，我觉得自己想的不对。


*答案3.17的代码和原来没多大变化就是函数名字变了
发包-POSTROUTING ->SNAT ->修改ct: nf_nat_setup_info->　ct->status |= IPS_SRC_NAT;->修改skb:nf_nat_packet

收报-PREOUTING-> DNAT->修改skb:nf_nat_packet 
{
    enum nf_nat_manip_type mtype = HOOK2MANIP(hooknum);   //因为是在PREROUTING, 所以是DNAT, 我以前一直以为, de-snat在postrouting中做的. 
                                           
    if (mtype == NF_NAT_MANIP_SRC)          
        statusbit = IPS_SRC_NAT;             
    else                                      
        statusbit = IPS_DST_NAT;        //到这里
                                                
    /* Invert if this is reply dir. */            
    if (dir == IP_CT_DIR_REPLY) 
		statusbit ^= IPS_NAT_MASK;        //翻转一下变成SNAT 
    /* Non-atomic: these bits don't change. */                                                                                                    
    if (ct->status & statusbit) {                 //正好和发包是的   ct->status |= IPS_SRC_NAT;匹配了, 开始de-snat.                    
		struct nf_conntrack_tuple target;
		...
}

#Filter
=Init Build iptables
~/linux/net/ipv4/netfilter/iptable_filter.c
iptable_filter_init()
struct xt_table packet_filter + iptable_filter_hook

#FAQ
=Why not use nat_bysource;



