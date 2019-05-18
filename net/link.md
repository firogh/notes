---
tags: [ net ] 
title: Data link layer
date: 2015-02-27T15:46:13+08:00 
category: net
---

# Multi-queue
[Multi-queue network interfaces with SMP on Linux](https://greenhost.nl/2013/04/10/multi-queue-network-interfaces-with-smp-on-linux/)

# Common concepts
The link layer is the group of methods and communications protocols that only operate on the link that a host is physically connected to. 
The link is the physical and logical network component used to interconnect hosts or nodes in the network 
A link protocol is a suite of methods and standards that operate only between adjacent network nodes of a local area network segment or a wide area network connection.

# Frame 
ULNI page 283: Figure 13-8. Differences between Ethernet and 802.3 frames

# L2 header
dst_neigh_output->dev_hard_header ->  eth_header

# Neighbor 
* ip_output_finish2 -> __neigh_create -> tbl->constructor -> arp_constructor{
if !dev->header_ops   //slip is the case, see sl_setup
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
static void ppp_setup(struct nethernetet_device *dev) 
{                                           
    dev->netdev_ops = &ppp_netdev_ops;       
    dev->hard_header_len = PPP_HDRLEN;        
    dev->mtu = PPP_MRU;
    dev->addr_len = 0;  
    dev->tx_queue_len = 3
    dev->type = ARPHRD_PPP
}

