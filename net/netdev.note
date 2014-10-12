###INIT
===System initialization

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

#Protocols
Protocols are the framework for all communication.
The TCP/IP model covers most of the protocols used by computers today.

#protocols handler init
For a protocol included statically in the kernel, the initialization function executes at boot time; 
for a protocol compiled as a module, the initialization function
executes when the module is loaded.
The function allocates internal data structures,notifies other subsystems about the protocol’s existence, 
registers files in /proc, and so on. 
A key task is to register a handler in the kernel that handles the traffic for a protocol.

#package name in different layer
An individual package of transmitted data is commonly called a frame on the link layer, L2; 
a packet on the network layer; a segment on the transport layer; and a message on the application layer.

#Network stack
The layers are often called the network stack, Application -> L2.

#Capture frames
PF_PACKET
AF_PACKET sockets hand frames directly to dev_queue_xmit

#tx path
tcp->ip-> dev_queue_xmit
{
	neigh
	dev watchdog
	softirq
}

#rx path
phy
broadcom b44.c
->
{
	softirq
	napi
}

#the header
in net/ipv4/ip_output.c we find skb->protocol = htons(ETH_P_IP);
include/linux/netdevice.h
dev_hard_header
net/ethernet/eth.c
eth_header_ops, ts eth_header

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
netif_receive_skb(skb) -> ptype_base->func(ETH_P_802_2) = llc_rcv -> llc_sap_find: snap_rcv -> ipx_rcv}

Linux supports LLC/SNAP and various things over it (IPX/Appletalk DDP
etc) but not IP over it, as it's one of those standards bodies driven
bogosities which nobody ever actually deployed.

SNAP register_snap_client used by appltalk, IPX, they broke TCP/IP protocol suites!
llc和SNAP 主要是为了兼容apple 和 novell IPX协议族.
IP 不需要 no meaning!


#ARP Neigh
=For IPv4 Network!
in this function, we know everything:
arp_constructor()
{
	if (dev->header_ops->cache) //eth_hedaer_ops support this!
		neigh->ops = &arp_hh_ops;                                                                                                                 
	else
		neigh->ops = &arp_generic_ops;
	if (neigh->nud_state & NUD_VALID) // for arp_hh_ops, they are the same neigh_resolve_output!
		neigh->output = neigh->ops->connected_output;
	else
		neigh->output = neigh->ops->output;
}

=For IPv6,
ndisc_constructor()
{
	THAT IS the same as IPv4 
}








