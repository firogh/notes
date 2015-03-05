---
tags: net
title: Linux Network stack
date: 2015-02-27 15:46:13
category: net
---

#Reference
[Understanding TCP/IP Network Stack & Writing Network Apps](http://www.cubrid.org/blog/dev-platform/understanding-tcp-ip-network-stack/#.VB6Vx9c6mKc.twitter)
[TCP/IP Reference Page](http://www.protocols.com/pbook/tcpip1.htm)

#Common concepts
* INET     
An implementation of the TCP/IP protocol suite for the LINUX operating system.  
INET is implemented using the  BSD Socket interface as the means of communication with the user level. 

* Encapuslation
tcp ip

* package name in different layer
An individual package of transmitted data is commonly called a frame on the link layer, L2; 
a packet on the network layer; a segment on the transport layer; and a message on the application layer.

* apple talk in linux network stack
talk_dgram_ops
&atalk_family_ops

* What is LAN

* WAN
[WAN](https://en.wikipedia.org/wiki/Wide_area_network)

* relations of concept
Qdisc -- NET_XMIT_SUCCESS
dev -- NETDEV_TX_OK

#INIT
inet_init
net_dev_init

# socket
* What is socket?
A socket is one endpoint of a two-way communication link between two programs running on the network.
An Internet socket is characterized by at least the following :
Local socket address: Local IP address and port number
Protocol: A transport protocol (e.g., TCP, UDP, raw IP, or others).
Remote socket address, if connected to another socket.
struct sockaddr

* Abstruction Concepts of socket
sock_common: 5 tuples, the essence of sock
inet_timewait_sock: deal heavily loaded servers without violating the protocol specification 
sock: network sock
atalk_sock: apple talk
unix_sock: unix_address
netlink_sock:portid
socket: BSD socket with VFS stuff
inet_sock: INET sock, sock_common inet-nise!ip addr, Multicast, TTL 
inet_connection_sock: INET connection oriented sock, Pluggable congestion control hook, Delayed ACK control data
tcp_sock: tcp sock, snd_cwnd, tcp_options_received, reordering, keepalive_probes

#Session
In computer science, in particular networking, a session is a semi-permanent interactive information interchange
session，中文经常翻译为会话，其本来的含义是指*有始有终*的一系列动作/消息
[Instance of tcp session in BSD socket](http://www.scottklement.com/rpg/socktut/overview.html)
[TCP Session - Handshaking in protocol](http://www.dummies.com/how-to/content/network-basics-tcp-session-establishment-handshaki.html)

#Virtul circuit
A virtual circuit (VC) is a means of transporting data over a packet switched computer network 
in such a way that it appears as though there is a dedicated physical layer link between the source and destination end systems of this data. 

#Implemention of protocols
* inet_create
sock->ops = inet_protosw->ops = inet_stream_ops

* proto_ops -- fops 
is a good name stand for all PF_*, all 协议族, but sock_generic_ops is better 具体协议与BSD socket api的通用接口

* proto, -- specific fs, like ext,  btfs in *inetsw*
sock的lab决定具体的slab, 如tcp_sock/udp_sock, 根本的发送方法tcp_sendmsg, 协议的真正实体!

* 越来越具体
BSD socket api ->proto_ops(sock type base)协议通用api ->proto (udp/tcp_prot)
sys_bind -> inet_stream_ops ->inet_bind ->sk_prot->bind(likely, is NULL)
write->inet_stream_ops->sendmsg->tcp_sendmsg

* inet_connection_sock_af_ops
icsk->icsk_af_ops

* net_protocol -- l4 rcv in *inet_protos*
是iphdr中protocol成员的延伸, 所以有了tcp_protocol/udp_protocol all in inet_protos

* packet_type -- l3 rcv in ptype_all and ptype_base
pt_prev->func


#BSD socket layer
Details and skills in Unix network programming.
* sockfs -- using read, write, close to manipulate socket fd.
[Linux Sockets and the Virtual Filesystem](http://isomerica.net/~dpn/socket_vfs.pdf)

#Transport layer -- common
* Multiplexing --  Ports can provide multiple endpoints on a single node. 
inet_hash_connect()

#TCP -- or some connetion scok
* Connection-oriented communication -- Session and virtual circuits
Connection-oriented (CO-mode[1]) communication is a network communication mode in telecommunications and computer networking, where a communication session or a semi-permanent connection is established before any useful data can be transferred, and where a stream of data is delivered in the same order as it was sent
Connection-oriented communication may be a circuit switched connection, or a packet-mode virtual circuit connection. 
Layer 4 virtual circuits uses segment number fix routed reorder delivery. Same order delivery.

* Reliability -- assured,Error detection and correction
Error --  checksum, the transport protocol may check that the data is not corrupted
ACK is an indiction of segments lost.
correction -- Retransmission, ARQ,Automatic repeat request schemes may be used to retransmit lost or corrupted data.
verify correct receipt by sending an ACK or NACK message to the sender. 

* Flow control
Sliding Window

* Congestion control
icsk_ca_ops;
tcp_ack {
tcp_cong_avoid
tcp_fastretrans_alert
tcp_slow_start
}

TCP send queue len /proc/sys/net/core/wmem_default

#Network layer
* Error detection, unreliable
Best effort service,IP has a simple error handling algorithm: throw away the datagram and try to send an ICMP message back to the source

* Host addressing

* Netfilter


#data link layer

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

# Frame 
[Ethernet Frame](http://www.infocellar.com/networks/ethernet/frame.htm)
+ 一种不太确定的非严格的真实划分
TCP/IP -> Ethenet II frame
IPX/APPLETALK -> 802.3/LLC(802.2), SNAP, mac 发来的包走这条路.

#net_device
*Understand __QUEUE_STATE_FROZEN
http://thread.gmane.org/gmane.linux.kernel/709444/focus=714632

* dev_watchdog,

* the generic network queueing layer
netdev_state_t
__LINK_STATE_START,

#Neighbor 
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
static void ppp_setup(struct net_device *dev) 
{                                           
    dev->netdev_ops = &ppp_netdev_ops;       
    dev->hard_header_len = PPP_HDRLEN;        
    dev->mtu = PPP_MRU;
    dev->addr_len = 0;  
    dev->tx_queue_len = 3
    dev->type = ARPHRD_PPP
}

#PPP SLIP
+
#Data Framing
dst_neigh_output->dev_hard_header ->  eth_header

#TC Qdisc

+
#LLC (TCP/IP rarely use this sub layer)
* ptype MAC layer 之上, 可能是data link(llc) or network layer(ip)
定义了所有从驱动上来的packet接收函数, 这里有ip_rcv 还有pppoe_rcv,llc_rcv, NO snap_rcv
dev_add_pack
llc_rcv{
snap_rcv
}
netif_receive_skb ->ip/llc_rcv

#NAPI

#Driver 
* skb->protocol
+ assignment in ip_output by = htons(ETH_P_IP)
+ assignment in driver by = eth_type_trans() 

+
#MAC

+
# Addressing
LAN switching

*IEEE 802 suite
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

#Physical layer -- PHY

* Physical Coding Sublayer
* Physical Medium Attachment Sublayer
* Physical Medium Dependent Sublayer

###out
inet_stream_ops->tcp_sendmsg()->tcp_push()->__tcp_push_pending_frames()->tcp_write_xmit()->tcp_transmit_skb()->ipv4_specific.ip_queue_xmit()->
ip_local_out()->__ip_local_out()->NF_INET_LOCAL_OUT->dst_output()->
ip_output()
{
	//set in ip_mkroute_output
	skb->dev = dev = skb_dst(skb)->dev; //!!!
	skb->protocol = htons(ETH_P_IP);
}->NF_INET_POST_ROUTING->ip_finish_output()->

ip_finish_output2-> dst_neigh_output
{ 
	neigh_hh_output // hh already in below:-)
	or 
	n->output = neigh_resolve_output{dev_hard_header}
}
->dev_queue_xmit()
{

	__dev_xmit_skb->__qdisc_run->qdisc_restart()->dev_hard_start_xmit()
	or 
	validate_xmit_skb->skb_gso_segment->skb_mac_gso_segment-> ptype->callbacks.gso_segment=inet_gso_segment->tcp4_gso_segment,
	dev_hard_start_xmit()
}
xmit_one->
{
	dev_queue_xmit_nit is Sun's Network Interface Tap (NIT)
	netdev_start_xmit->ops->ndo_start_xmit{this functions is init in createing device} = e100_xmit_frame
}

softirq:net_tx_action()->qdisc_run()

###in & forward
* NAPI poll_list net_device
driver intr e100_intr()->__netif_rx_schedule()->__napi_schedule(netdev,nic->napi)->:add napi to poll_list and __raise_softirq_irqoff()
do_softirq->net_rx_action()->netdev->poll()=e100_poll()->e100_rx_clean()...netif_receive_skb()->

* Non-NAPI input_pkt_queue skb
driver intr vortex_rx()->netif_rx()->napi_schedule(backlog)->add napi to poll_list and__raise_softirq_irqoff()
async:net_rx_action()->backlog->poll()=process_backlog()->netif_receive_skb()->deliver_skb->

* common path
pt_prev->func=ip_rcv()->NF_INET_PRE_ROUTING->ip_rcv_finish()->
ip_route_input()->ip_route_input_slow()
{
	local_input dst.input??=ip_local_deliver()
	ip_mkroute_input()->__mkroute_input():dst.input=ip_forward() 紧接着dst.output??=ip_output()
}
dst_input()->
{
	ip_local_deliver()->NF_INET_LOCAL_IN->ip_local_deliver_finish()->inet_protos.tcp_v4_rcv()
	or 
	ip_forward()->NF_INET_FORWARD->ip_forward_finish()->dst_output()见上。
}

* Differences
1 NAPI has not  netif_rx():input_pkt_queue.
2 NAPI and Non-NAPI used different napi->poll 决定本质上的区别。
3 vortex_rx() 多，e100_rx_clean()多！这点可以看出不同优势来。

* Need clean
net_tx_action->output_queue/每个设备的qdisc and  clear_bit__QDISC_STATE_SCHED qdisc_run add back
__QDISC_STATE_SCHED是否加入softdata
qdisc_restart: 如果队列有数据就返回大于零 继续减小weight_p
__qdisc_run queue no data __QDISC_STATE_SCHED not set, only in this case!
driver tx, stack xmit





###register
=outer
e100_init_module	pci_register_driver:构建结构	driver_regiser:注册到内核	really_probe()drv->probe:初始化。
vconfig add		regiser_vlan_device：构建结构	register_netdevice:注册到内核	dev->init():初始化


###network init
inet_init()->ip_init()->ip_rt_init()->ip_fib_init()->fib_hash_init():create kmem_cache

#Net  link
iproute2 ...->inet_rtm_newroute()->fib_new_table()->fib_hash_table()


#PF_*
* Capture frames
PF_PACKET
AF_PACKET sockets hand frames directly to dev_queue_xmit

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

