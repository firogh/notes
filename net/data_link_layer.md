---
tags: net
title: data link layer
date: 2015-02-27 15:46:13
category: net
---

#Reference

#Common concepts
* MTU
This limits the number of bytes of data to 1500(Ethernet II) and 1492(IEEE 802), respectively. 
This characteristic of the link layer is called the MTU, its maximum transmission unit.

* PMTU
/proc/sys/net/ipv4/ip_no_pmtu_disc
0 enable, 1 disable

cat /proc/sys/net/core/warnings

/proc/sys/net/ipv4/tcp_mtu_probing
!0 enable tcp_mtu_probing()
If you are using Jumbo Frames, we recommend setting tcp_mtu_probing = 1 to 
help avoid the problem of MTU black holes. Setting it to 2 sometimes causes performance problems.

net/ipv4/icmp.c
icmp_unreach(
type 3, code 4
icmph->type == ICMP_DEST_UNREACH //3
case ICMP_FRAG_NEEDED //4
icmp_err,


###System initialization
start_kernel-> parse_early_param irq timers softirq -> rest_init(): kthread
{
	do_basic_setup()  
	{
		driver_init
		sock_init
		do_initcalls()
		{
			net_dev_init: Initializing the Device Handling Layer
			{
				per-CPU 
				proc
				sysfs
				ptype_base
				dst_init
				softirq: net_rx/tx_action
				dev_cpu_callback: CPU hotplug.
			}
		}

	}
	free_init_mem()
	run_init_process()
}

#ipv4 Neighbor output
* ip_output_finish2 -> __neigh_create -> tbl->constructor -> arp_constructor{
if !dev->header_ops
	neigh->ops = &arp_direct_ops
	neigh->output = neigh_direct_output
else if ARPHRD_ROSE/AX25/NETROM
	arp_broken_ops
	neigh->ops->output
else if dev->header_ops->cache
	neigh->ops = &arp_hh_ops
else
	arp_generic_ops
                                                          
if (neigh->nud_state & NUD_VALID)                  
	neigh->output = neigh->ops->connected_output;   
else                                  
	neigh->output = neigh->ops->output;
}

* ip_output_finish2 -> dst_neigh_output -> neigh_resolve_output

*  ipv4 Neighbor output instance of ethernet
see alloc_etherdev_mqs-> ether_setup{
dev->header_ops = &eth_header_ops;
dev->type       = ARPHRD_ETHER;
eth_header_ops.cache = eth_header_cache
}
so neigh->ops = &arp_hh_ops; neigh->output = neigh_resolve_output in arp_hh_ops

	//tg3_init_one
	dev->netdev_ops = &tg3_netdev_ops;
	dev->ethtool_ops = &tg3_ethtool_ops;
	dev->watchdog_timeo = TG3_TX_TIMEOUT;

	//In ppp
static void ppp_setup(struct net_device *dev) 
{                                           
    dev->netdev_ops = &ppp_netdev_ops;       
    dev->hard_header_len = PPP_HDRLEN;        
    dev->mtu = PPP_MRU;
    dev->addr_len = 0;  
    dev->tx_queue_len = 3
    dev->type = ARPHRD_PPP

#the hard header
in net/ipv4/ip_output.c we find skb->protocol = htons(ETH_P_IP);
include/linux/netdevice.h
dev_hard_header -> dev->header_ops->create = eth_header
net/ethernet/eth.c
eth_header_ops, eth_header

IEEE 802.1—概述、体系结构和网络互连，以及网络管理和性能测量。 
IEEE 802.2—逻辑链路控制LLC。最高层协议与任何一种局域网MAC子层的接口。 
IEEE 802.3—CSMA/CD网络，定义CSMA/CD总线网的MAC子层和物理层的规范。 
IEEE 802.4—令牌总线网。定义令牌传递总线网的MAC子层和物理层的规范。 
IEEE 802.5—令牌环形网。定义令牌传递环形网的MAC子层和物理层的规范。 
IEEE 802.6—城域网。 
IEEE 802.7—宽带技术。 
IEEE 802.8—光纤技术。 
IEEE 802.9—综合话音数据局域网。 
IEEE 802.10—可互操作的局域网的安全。 
IEEE 802.11—无线局域网。 
IEEE 802.12—优先高速局域网(100Mb/s)。 
IEEE 802.13—有线电视(Cable-TV)。 

#LLC SNAP
=INIT begin in llc_init
+dev_add_pack(&llc_packet_type), this function hanlder is used in netif_receive_skb!
static struct packet_type llc_packet_type __read_mostly = { .type = cpu_to_be16(ETH_P_802_2),.func = llc_rcv, };

=RUNTIME input flow , it was just a protocol handler used in netif_receive_skb, add by dev_add_pack like ip_rcv
+ driver:b44_rx -> 
{//the key point is eth_type_trans function.
skb->protocol = eth_type_trans(skb, bp->dev);
#Understand __QUEUE_STATE_FROZEN
http://thread.gmane.org/gmane.linux.kernel/709444/focus=714632
