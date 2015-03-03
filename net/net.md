---
tags: net
title: Linux Network stack
date: 2015-02-27 15:46:13
category: net
---

#Reference
[Understanding TCP/IP Network Stack & Writing Network Apps](http://www.cubrid.org/blog/dev-platform/understanding-tcp-ip-network-stack/#.VB6Vx9c6mKc.twitter)

#Common concepts
* INET     
An implementation of the TCP/IP protocol suite for the LINUX operating system.  
INET is implemented using the  BSD Socket interface as the means of communication with the user level. 

# socket
* What is socket?
A socket is one endpoint of a two-way communication link between two programs running on the network.
An Internet socket is characterized by at least the following :
Local socket address: Local IP address and port number
Protocol: A transport protocol (e.g., TCP, UDP, raw IP, or others).
Remote socket address, if connected to another socket.
struct sockaddr


#System call
Details and skills in Unix network programming.
* sockfs -- using read, write, close to manipulate socket fd.
[Linux Sockets and the Virtual Filesystem](http://isomerica.net/~dpn/socket_vfs.pdf)

#Transport layer
* Multiplexing -- Port


* TCP

TCP send queue len /proc/sys/net/core/wmem_default


###switch
struct proto tcp_prot;
struct proto_ops inet_stream_ops;
struct inet_protosw inetsw[]
struct inet_connection_sock_af_ops ipv4_specific;
struct net_protocol inet_protos[] //Layer 4 rcv!
struct net_device_ops //Layer 2 xmit functions.

###out
inet_stream_ops.tcp_sendmsg()->tcp_push()->__tcp_push_pending_frames()->tcp_write_xmit()->tcp_transmit_skb()->ipv4_specific.ip_queue_xmit()->
ip_local_out()->__ip_local_out()->NF_INET_LOCAL_OUT->dst_output()->
ip_output()见ip_mkroute_output->NF_INET_POST_ROUTING->
ip_finish_output()->ip_finish_output2()->
							neigh_hh_output() /cachae hardware header.
							dst->neighbour->output=neigh_resolve_output(arp_hh_ops see arp_创建):dev_hard_header()
}dev_queue_xmit()->dev_hard_start_xmit()

###in & forward
do_softirq()->net_rx_action()...->netif_receive_skb()->ptype_base.ip_rcv():protocol handler->NF_INET_PRE_ROUTING->ip_rcv_finish()->
ip_route_input()->ip_route_input_slow():local_input dst.input=ip_local_deliver()
										ip_mkroute_input()->__mkroute_input():dst.input=ip_forward() dst.output=ipoutput()
}dst_input()->
ip_local_deliver()->NF_INET_LOCAL_IN->ip_local_deliver_finish()->inet_protos.tcp_v4_rcv()
ip_forward()->NF_INET_FORWARD->ip_forward_finish()->dst_output()见上。

net_tx_action->output_queue/每个设备的qdisc and  clear_bit__QDISC_STATE_SCHED qdisc_run add back
__QDISC_STATE_SCHED是否加入softdata
qdisc_restart: 如果队列有数据就返回大于零 继续减小weight_p
__qdisc_run queue no data __QDISC_STATE_SCHED not set, only in this case!
driver tx, stack xmit

#skb->protocol
+ assignment in ip_output by = htons(ETH_P_IP)
+ assignment in  driver by = eth_type_trans()


###register
=outer
e100_init_module	pci_register_driver:构建结构	driver_regiser:注册到内核	really_probe()drv->probe:初始化。
vconfig add		regiser_vlan_device：构建结构	register_netdevice:注册到内核	dev->init():初始化


###分形艺术轮回之地
大抵会回到这里。
揭示终极的分形艺术。
文件操作，驱动使用，
create,			open,	read/write, send, recv, close, delete
module_init, regiser	open				

inet_init()->ip_init()->ip_rt_init()->ip_fib_init()->fib_hash_init():create kmem_cache
iproute2 ...->inet_rtm_newroute()->fib_new_table()->fib_hash_table()

#TCP
=port
inet_hash_connect()

#package name in different layer
An individual package of transmitted data is commonly called a frame on the link layer, L2; 
a packet on the network layer; a segment on the transport layer; and a message on the application layer.
