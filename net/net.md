---
tags: net
title: Linux Network Stack
date: 2015-02-27 15:46:13
category: net
---

#Reference
[Understanding TCP/IP Network Stack & Writing Network Apps](http://www.cubrid.org/blog/dev-platform/understanding-tcp-ip-network-stack/#.VB6Vx9c6mKc.twitter)
[TCP/IP Reference Page](http://www.protocols.com/pbook/tcpip1.htm)

# Network
## 为什么Internet产生?
就好比问, 色情片为什么出现一样. 人类的发明都是对自身需求的回应.
Internet 解决信息传递的问题.信息传递只是手段, 古已有之, 比如信鸽, 狼烟等.

## Internet 都能做什么?
看片等...
 
## 什么是Internet
英文[network](http://keithbriggs.info/network.html), 其中work, 构造之意.
etymonline 给出结缔成网之意, net-like arrangement of threads, wires, etc.
Network -> Telecommunications network -> Computer network -> Internet

The Internet is a global system of interconnected computer networks that use 
the standard Internet protocol suite (TCP/IP) to link several billion devices worldwide.
Internet protocol suite,是结网的策略方法核心.

# Internet protocol suite
The Internet protocol suite is the computer networking model and set of 
communications protocols used on the Internet and similar computer networks.

Network stack严格的表述是network protocol stack.
通常简称[protocol stack](http://en.wikipedia.org/wiki/Protocol_stack), 即协议栈
The protocol stack is an implementation of a computer networking protocol suite. 
[Internet protocol suite -- TCP/IP](http://en.wikipedia.org/wiki/Internet_protocol_suite)就是一种[protocol stack.](http://en.wikipedia.org/wiki/List_of_network_protocol_stacks)

## 为什么协议要分层? 怎么分?
这是非常有深度的问题! 可以说本文价值所在.
这里讨论的是如何设计一套网络协议.
抛开TCP/IP, 如果要让我门自己设计一套 network protocol, 有什么思路?
发送方, 接收方, 信息. 这是所有信息传递的要素, 无论是, 狼烟, 信鸽, 还是书信.
不同的是, 信息传递的方式.狼烟这种大范围视觉方式, 关心的最少, 敌人来犯点着烟就行了.
那么信鸽了, 信鸽的英文叫做Homing pigeon, 信鸽的归巢本能, 为我指定了信息的接收房.
抛开隐私, 我们能把信放大到天空那么大, 收信人抬下头也能收到信息. 
对于书信的方式, 在信息不能广泛的传递给每个人的时候, 我们需要把信送到特定的人手里.  
信的地址就成了必选. 信息传输的方法决定了, 接收者的特征集合.
protocol暗指communications protocol, protocol词源上便指diplomatic rules of etiquette
一种合适的交流手段. 而The Internet protocol suite的定义中明确指明了computer networking model.
也就是源于方法决定对象的经验结论, wikipedia上的定义是非常有见地的.
所以回到计算机框架下的信息传输设计. 受限于计算机数据的交互方法, 我们必须指明信息的接收方.
所以我来设计一套network protocol, 第一点, 就是如何表示每个计算机收信方, 也就是地址.
地址成了网络协议的本体!有了地址后, 如何寻址就是个问题了, 这属于衍生的问题, 没什么意思了.
我们假装, 已经设计出一套惊天地泣鬼神的寻路方法, 还是叫寻址更好, 寻找目的地址.
我们只是道出了, 网络协议的基本的要素接收方的本质属性, 也就是model. 那么, 我们怎么定义方法呢?
传输无外乎, 你有消息, 给我发个信, 再有消息, 再发, 当然我也可以给你发. 结合到实际的计算机领域,
假如, 我们只有地址和寻址方法的寒酸网络协议, 在一个小函数里搞定了.
为什么OSI和TCP/IP都搞得那么复杂?
下面才是为什么要[分层](http://en.wikipedia.org/wiki/Abstraction_layer), 和怎么分的问题.
是什么导致了分层? 哲学上, 分乃是不同的存在.也就是说存在我们寒酸的网络协议不同的存在.
我们现在把linux内核的协议栈替换称我们的poor network protocol, (你现在, 应该知道为什么osi中
network layer的名字来源了, 正因为他代表了整个网络协议的实质, 所以名字逼格才这么高!)
显然, 还是不能运行, 为什么?我们缺少和应用层以及底层硬件的交互.也就是空中楼阁! 
加一个poor bsd socket,  poor application layer来了.加一个和底层硬件交互, poor linker layer也来了. 
我靠, 我的协议也有3层了, 拿去用吧.
我现在把传输层也意淫进来. 显然, 传输层不是这么随便的, 他的存在肯定有着合理的理由.
事物存在的理由不在于自身! 我认为汉语传输层(transport)是一个被严重误读了的名字, 这不同于人的名字, 
人类的名字是一个标示系统, 比如你叫马机霸, 你就一定要长得像马jb. 而学术领域的, 名字则具有
另外一个重要的属性就是:

	概念性or推理性的认知.
即从名字中可以推理出相关属性; 在汉语当中, 我第一次看谢希仁《计算机网络第五版》教材时,
就感觉这个名字好晕啊!他和传输有半毛钱关系, 传输不是链路层网卡网线的事吗? 
记得老师说了句, 这是一种抽象, 问题旧搁置了.当年未能深入问题, 非常遗憾. 今天补上!
TCP中的T是transmission和transport中的都被翻译成了汉语的传输! 
有很长一段时间, 我都认为传输层,应该叫做传输控制层, 因为他, 看上去真的更像控制啊!
那么transmission和transport的区别到底事什么呢?
看wikipedia的解释, 立马明白了, 这分明说的是传输层的本质啊:
Transport or transportation is the movement of people, animals and goods from one location to another.
相信不用看etymonline你也知道transport是怎么来的trans + port, 这里的port是名词港口之意.
现在, 就明白了为什么port端口号, 虽然实质是地址的含义, 却不属于network层.
传输层有卸货的含义, 干脆就叫转港层吧!
transport layer 的巨大意义, 就被显示出来了, 他是必须的.
实质上我们也应该看到无论是port还是ip address都是地址的含义, 这也是协议栈模型的本质.
下面我们来讨论, 信息发送的方法问题.
网络协议栈的每一层都有着不同协议, 也就是不同方法.即便是一个协议自身也是众多方法的集合.
理解这些协议程度, 就成为network 工程师能力差异, 下面我们会逐层意淫, 绝不是什么分析理解.
回到最初的问题, 实际上我们已经解决了怎么分层的问题了.
我们现在还差一个, 为什么分层.
简单说这是个设计问题. 设计的好会影响的设计本身与实现.这里是模块化的设计思路.
优点如维基所说:
Because each protocol module usually communicates with two others, 
they are commonly imagined as layers in a stack of protocols. 
还有其他的有点.

# Design of [Communications protocol](http://en.wikipedia.org/wiki/Communications_protocol)
[The Design Philosophy of the DARPA Internet Protocols](http://ccr.sigcomm.org/archive/1995/jan95/ccr-9501-clark.pdf)
我们在这里探讨的是通用的信息交换协议, 也就是说UDP, TCP, IP共有的性质, 之后我们会结合Stevens的
书籍和linux 内核的实现, 去探讨这些协议具体的差异, 当然是偏重为什么导致这种差异.
方法还是思考(意淫).





# INET 
An implementation of the TCP/IP protocol suite for the LINUX operating system.  
INET is implemented using the  BSD Socket interface as the means of communication with the user level. 

#PF_*
* Capture frames
PF_PACKET
AF_PACKET sockets hand frames directly to dev_queue_xmit

# Encapuslation

# offload
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

#Transport layer
Details in l4.md

#Network layer
Details in l3.md

# Data link layer
Details in l2.md

# Physical layer -- PHY
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

#Net initialization
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

