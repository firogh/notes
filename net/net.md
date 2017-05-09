---
tags: [ net ] 
title: Linux Network Stack
date: 2015-02-27T15:46:13+08:00 
category: net
---

#Reference
[TCP/IP Reference Page](http://www.protocols.com/pbook/tcpip1.htm)
[RAW PACKET FORMATS](http://www.securitywizardry.com/index.php/tools/packet-headers.html)
[从 IPv4 到 IPv6](http://wangcong.org/2012/10/15/-e4-bb-8e-ipv4-e5-88-b0-ipv6/)
[What’s wrong with IPv4 and Why we are moving to IPv6](http://www.tecmint.com/ipv4-and-ipv6-comparison/)
[Improving Linux networking performance](https://lwn.net/Articles/629155/)

The Feynman map of network, check [Thomas Graf's DevConf 2014 Kernel Networking Walkthrough](https://www.slideshare.net/ThomasGraf5/devconf-2014-kernel-networking-walkthrough)
When we talk about network, what we talk about?
Transfer; Truely we are talking about transfer including three component:src, dst, channel.
Address has two properties: relation and scope.

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
[更完整的陈述](http://en.wikipedia.org/wiki/Communications_protocol#Layering)

# Design of [Communications protocol](http://en.wikipedia.org/wiki/Communications_protocol)
[The Design Philosophy of the DARPA Internet Protocols](http://ccr.sigcomm.org/archive/1995/jan95/ccr-9501-clark.pdf)
[Reviews from RICE edu](http://www.cs.rice.edu/~eugeneng/teaching/s04/comp629/reviews/Cla88.txt)
[Reviews of Michael S. Liu Yale](http://zoo.cs.yale.edu/classes/cs633/Reviews/Cla88.msl38.html)
Address formate: 有了地址我们就要有地址的具体格式如哪个街道，哪个小区几号楼几室。
Route: 找到收信人, 计算机网络包括linker, network 和transportlayer
Data formate: 接下来就是信封格式，对应头部。
Reliability:发的信可能丢了，要在写一封吗？
Detection of transmission errors:如信被人篡改了

# Linux network stack workthrough
[DevConf 2014 Kernel Networking Walkthrough](http://www.slideshare.net/ThomasGraf5/devconf-2014-kernel-networking-walkthrough)
[introduction to linux kernel tcp/ip ptocotol stack](http://www.slideshare.net/minibobo/linux-tcp-ip?related=1)
taobao的第5也说明了问题.
这是通常的skb的流向. 就是在socket里面按着协议走, 包括tcp的重传.
还有一种, 就是想kproxy那种, 人为的干扰skb的走向, netif_receive_skb就是一个点.
netif_receive_skb之后就是标准的内核协议栈的事情了包括bonding啊, vlan, bridge这些什么的.
我觉得这么说还是不够深度, 我们确实在探索skb在协议栈中的流转.
我们都知道协议栈中skb按着协议走的, 如果能指出什么时候我们可以合法地让报文转个向.
就能打到我们的目的, 多少能提升下对workthrough的理解的深度;)
* af_packet相关的
dev_queue_xmit的dev_queue_xmit_nit中clone后deliver_skb送上去.
netif_receive_skb 的__netif_receive_skb_core 的deliver_skb. 有个问题?
为什么skb直接送上去了没有skb_get之类的.原来每个deliver_skb都有
atomic_inc(&skb->users);为什么不是skb_get
* 主动调用netif_receive_skb 
很多pptp协议就是这么干的.
其实最经典还是pskb_copy和clone的那个场景!
这个应该多积累, 我感觉挺重要的.
[Understanding TCP/IP Network Stack & Writing Network Apps](http://www.cubrid.org/blog/dev-platform/understanding-tcp-ip-network-stack/)
## out
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

## in & forward
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

## package name in different layer
An individual package of transmitted data is commonly called a frame on the link layer, L2; 
a packet on the network layer; a segment on the transport layer; and a message on the application layer.

## FAQ
* Need clean
net_tx_action->output_queue/每个设备的qdisc and  clear_bit__QDISC_STATE_SCHED qdisc_run add back
__QDISC_STATE_SCHED是否加入softdata
qdisc_restart: 如果队列有数据就返回大于零 继续减小weight_p
__qdisc_run queue no data __QDISC_STATE_SCHED not set, only in this case!
driver tx, stack xmit
### socket lock
[lock_sock or sock_hold](http://www.spinics.net/lists/netdev/msg136306.html)
[bh_lock_sock](http://www.linuxfoundation.org/collaborate/workgroups/networking/socket_locks)
* FIXME sock->pfmemealloc
Yes, I only wanted to drop the packet if we were under pressure
when skb was allocated. If we hit pressure between when skb was
allocated and when __netdev_alloc_page is called,
+in sk_filter
[netvm: Allow skb allocation to use PFMEMALLOC reserves](https://groups.google.com/forum/#!msg/linux_net/-YtWB66adxY/Qqm_y4U09IAJ)
[netvm: Allow skb allocation to use PFMEMALLOC reserves - gmane 08/14](http://thread.gmane.org/gmane.linux.kernel/1152658)
socket是跟协议族绑定的概念, 所以要用inet_create, netlink_create
* FIXME inet_timewait_sock 
deal heavily loaded servers without violating the protocol specification 
* sk_set_memalloc
SOCK_MEMALLOC, sock has feature mem alloc for free memory.
只有到了sock层才能分辨, sock是否是memalloc的.
* first_device 用途？
subsys 在前, device在后.
* What is bridge?
linux bridge 
netdev_rx_handler_register(dev, br_handle_frame, p);
__netif_receive_skb -> rx_handler=br_handle_frame
and generic concept: hub, switch?
hub: layer 1, bradcast, exclusive share, 报文可被侦听.
switch: layer 2,  mac port route, CAM table in linux bridge module!
switch with vlan: layer 3, 因为vlan之间的报文转发需要路由, 所以是layer层技术.
* What is Head-of-line blocking
