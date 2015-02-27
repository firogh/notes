---
tags: net
title: internet-protocol-suite
date: 2015-02-27 15:46:13
category: net
---
协议栈本质上是 Transfer这个功能。客观上担当了行为发起与接受主体。
而我要揭示在这一过程中存在的Mandelbrot set —— 局部间相似，局部与整体相似。
从接受的角度
ip --> inet_protos[]
struct net_protocol inet_protos[MAX_INET_PROTOS]是作为接受主体存在的，准确说是后ip的接受者。
这些受体包括tcp, udp, icmp, igmp. 如，我们选取了icmp。
ip --> inet_protos[] --> icmp_rcv --> icmp_pointers[] 
struct icmp_control icmp_pointers[NR_ICMP_TYPES + 1]也是一个受！类似称之为后icmp的接受者。
包括dest_unreach, source_quench, time_exceeded, parameterprob ...
看到这里，我们就能建立"线性"的映射。
inet_protos <=> icmp_pointers
tcp, udp, icmp, igmp <=> dest_unreach, source_quench, parameterprob...

类似的，我们选取igmp。
ip --> inet_protos[] --> igmp_rcv 


同样的，在vlan注册当中我们也会发现同样的，分形艺术。
此时struct ptype_base[], 行为上就是一个Hash链表了。
ptype_base[],本质上就是 inet_protos or icmp_pointers.
同样在执行switch的功能，大多数人称之为hook，钩子机制。 hook会让人觉得迷惑，因为你不知道从哪里开始勾，很乱。
我一直坚持理解时思维应该是平的，smooth！
这三个switch处在不同的层次。ptype_base -> inet_protos -> icmp_pointers.
接下来，会有趣。为什么会是这样的处理顺序，作为一个普通人，我们如何知道，数据是如何被处理的呢？
只是普通的不能普通，从外到内逐步swith过程而已！



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
