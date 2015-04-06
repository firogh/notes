---
tags: net
title: Linux Network Stack
date: 2015-02-27 15:46:13
category: net
---

#Reference
[Understanding TCP/IP Network Stack & Writing Network Apps](http://www.cubrid.org/blog/dev-platform/understanding-tcp-ip-network-stack/#.VB6Vx9c6mKc.twitter)
[TCP/IP Reference Page](http://www.protocols.com/pbook/tcpip1.htm)

# INET 
An implementation of the TCP/IP protocol suite for the LINUX operating system.  
INET is implemented using the  BSD Socket interface as the means of communication with the user level. 

#PF_*
* Capture frames
PF_PACKET
AF_PACKET sockets hand frames directly to dev_queue_xmit

# Encapuslation

#offload
* TSO in tcp_v4_connect
[TSO Explained](https://tejparkash.wordpress.com/2010/03/06/tso-explained/)
One Liner says: It is a method to reduce cpu workload of packet cutting in 1500byte and asking hardware to perform the same functionality.
* GSO
[GSO: Generic Segmentation Offload](http://thread.gmane.org/gmane.linux.network/37287)
TSO = GSO_TCPV4
frags = sg I/O
frag_list
*GRO
napi -> dev ->inet->skb

# package name in different layer
An individual package of transmitted data is commonly called a frame on the link layer, L2; 
a packet on the network layer; a segment on the transport layer; and a message on the application layer.

# apple talk in linux network stack
talk_dgram_ops
&atalk_family_ops

# What is LAN

# WAN
[WAN](https://en.wikipedia.org/wiki/Wide_area_network)

# relations of concept
Qdisc -- NET_XMIT_SUCCESS
dev -- NETDEV_TX_OK

#sk_buff
[sk_buff{} documents and resources](http://www.skbuff.net/skbuff.html)
[How SKBs work by David S. Miller](http://www.skbuff.net/skbuff.html)

* fclone -- fast clone
[NET Implement SKB fast cloning.](http://git.kernel.org/cgit/linux/kernel/git/torvalds/linux.git/commit/?id=d179cd12928443f3ec29cfbc3567439644bd0afc)
[Fast SKB cloning, continued](http://lwn.net/Articles/140552/)
use in skb_clone function
use case 1: tcpdump and network stack
fclones->fclone_ref 这就是引用, 用处见skb_clone
skbuff_head_cache alloc的skb对应n->fclone = SKB_FCLONE_UNAVAILABLE;
* pskb_pull -- p abbrivated from oprivate
* truesize -- len of sk_buff + head_len + frags + frag_list
* data_len -- len of frags + frag_list
* len -- head_len + frgas + frag_list

#skb reference usage
* 计数引用 count ref skb->users -- skb_get
	没有私有的
* 克隆引用 clone ref skb->dataref --skb_clone, skb_cloned
	sk_buff是私有的
* 页碎片共享 --pskb_copy
	sk_buff, head_len 是私用的
* 不共享 --skb_copy skb_copy_expand

#sock->pfmemealloc
Yes, I only wanted to drop the packet if we were under pressure
when skb was allocated. If we hit pressure between when skb was
allocated and when __netdev_alloc_page is called,
+in sk_filter
[netvm: Allow skb allocation to use PFMEMALLOC reserves](https://groups.google.com/forum/#!msg/linux_net/-YtWB66adxY/Qqm_y4U09IAJ)
[netvm: Allow skb allocation to use PFMEMALLOC reserves - gmane 08/14](http://thread.gmane.org/gmane.linux.kernel/1152658)

* sk_set_memalloc
SOCK_MEMALLOC, sock has feature mem alloc for free memory.
只有到了sock层才能分辨, sock是否是memalloc的.
sk_filter

* control buffer skb-cb
tcp_skb_cb

# socket
* What is socket?
A socket is one endpoint of a two-way communication link between two programs running on the network.
An Internet socket is characterized by at least the following :
Local socket address: Local IP address and port number
Protocol: A transport protocol (e.g., TCP, UDP, raw IP, or others).
Remote socket address, if connected to another socket.
struct sockaddr
socket是跟协议族绑定的概念, 所以要用inet_create, netlink_create

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
* Encapuslation to segment in skb
	tcp_sendmsg->skb_add_data_nocache()

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
tcp_slow_start}
TCP send queue len /proc/sys/net/core/wmem_default

##FIXME
* Create TCP options
tcp_syn_build_options()
* Receive ack
tcp_ack()
记录ack的数据大小mss or tcp abc
update snd_wl1 and snd_una
slow path update mtu mss tcp_skb_cb.sacked
* Active send data
tcp_sendpage()/tcp_sendmsg()->tcp_write_xmit()/tcp_push_one()->tcp_transmit_skb
* Timer expiring retransmiter
tcp_retransmiter_timer()...->tcp_transmit_skb()
* reponse for receiving an ACK
tcp_data_snd_check()->tcp_write_xmit()
* tcp_v4_rcv
[skb->dev = NULL;](http://thread.gmane.org/gmane.linux.network/85613/focus=85614)

#Network layer
* Error detection, unreliable
Best effort service,IP has a simple error handling algorithm: 
throw away the datagram and try to send an ICMP message back to the source
* Host addressing

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

#ipv4 address
* A类 0*******，
范围00000000~01111111 即0~127
 10.0.0.0~10.255.255.255
* B类 10******，
范围10000000~10111111 即128~191
 172.16.0.0~172.31.255.255
* c类 110***** 
范围11000000~11011111 即192~223
192.168.0.0~192.168.255.255
* D类
范围224~239用于组播
* E类
范围240~255 用于科学试验

##IP fragmention/defragmention
iphdr->id, iphdr->frag_off
skb_shared_info->frag_list 
ip_fragment/ip_defrag
[Updated Specification of the IPv4 ID Field](http://tools.ietf.org/html/rfc6864)

## Route
* state structure
fib_info:route info
fib_config:
* add new rule
iproute2 ...->inet_rtm_newroute()->fib_new_table()->fib_hash_table()
* Multi-time line
fib_create_info(): create a fib_info

## Netfilter

#data link layer

#Physical layer -- PHY
* Physical Coding Sublayer
* Physical Medium Attachment Sublayer
* Physical Medium Dependent Sublayer

#out
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

#in & forward
* NAPI poll_list net_device
driver intr add skb to private queue -> e100_intr()->__netif_rx_schedule()->__napi_schedule(netdev,nic->napi)->:
add napi to poll_list and __raise_softirq_irqoff()
do_softirq->net_rx_action()->
+netdev->poll()=e100_poll()private function->e100_rx_clean()...->
netif_receive_skb()->deliver_skb->
private queue and private function

* Non-NAPI input_pkt_queue skb
driver intr vortex_rx()->netif_rx()->add skb to SD input_pkt_queue->napi_schedule(backlog)->add backlog to SD poll_list __raise_softirq_irqoff()
async:net_rx_action()->
+backlog->poll()=process_backlog()
->netif_receive_skb()->deliver_skb->
skb to sd input_pkt_queue process

* common path
pt_prev->func=ip_rcv()->NF_INET_PRE_ROUTING->ip_rcv_finish()->
ip_route_input()->ip_route_input_slow()
{
	local_input dst.input??=ip_local_deliver()
	or
	ip_mkroute_input()->__mkroute_input():dst.input=ip_forward() 紧接着dst.output??=ip_output()
}
dst_input()->
{
	ip_local_deliver()->NF_INET_LOCAL_IN->ip_local_deliver_finish()->inet_protos.tcp_v4_rcv()
	or 
	ip_forward()->NF_INET_FORWARD->ip_forward_finish()->dst_output()见上。
}
* Differences
1 NAPI has not netif_rx():input_pkt_queue.
2 NAPI and Non-NAPI used different napi->poll 决定本质上的区别。
3 vortex_rx() 多，e100_rx_clean()多！这点可以看出不同优势来。

* Need clean
net_tx_action->output_queue/每个设备的qdisc and  clear_bit__QDISC_STATE_SCHED qdisc_run add back
__QDISC_STATE_SCHED是否加入softdata
qdisc_restart: 如果队列有数据就返回大于零 继续减小weight_p
__qdisc_run queue no data __QDISC_STATE_SCHED not set, only in this case!
driver tx, stack xmit

#System initialization
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

#network init
inet_init()->ip_init()->ip_rt_init()->ip_fib_init()->fib_hash_init():create kmem_cache

#net device init
* net_dev_init
* nic init
e100_init_module	pci_register_driver:构建结构	driver_regiser:注册到内核	really_probe()drv->probe:初始化。
vconfig add		regiser_vlan_device：构建结构	register_netdevice:注册到内核	dev->init():初始化

