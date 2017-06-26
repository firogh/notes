---
tags: [ net ] 
title: Transport layer
date: 2015-02-27T15:46:13+08:00 
category: net
---
# RFC
[A Roadmap for Transmission Control Protocol (TCP) Specification Documents](http://tools.ietf.org/html/rfc7414)
[Requirements for Internet Hosts -- Communication Layers](https://tools.ietf.org/html/rfc1122)
[TRANSMISSION CONTROL PROTOCOL 1981](http://tools.ietf.org/html/rfc793)
[RFC Errata 一切都有改口的余地](http://www.rfc-editor.org/errata_search.php?rfc=1122&rec_status=15&presentation=records)
793的errata建议阅读1122.
[1]: https://blog.cloudflare.com/the-story-of-one-latency-spike/
[2]: https://blogs.technet.microsoft.com/networking/2009/08/12/where-do-resets-come-from-no-the-stork-does-not-bring-them/
[3]: http://veithen.github.io/2014/01/01/how-tcp-backlog-works-in-linux.html
[4]: https://githubengineering.com/syn-flood-mitigation-with-synsanity/


# Latency
[The story of one latency spike][1]

# Reset
[Where do resets come from? (No, the stork does not bring them.)][2]
# Backlog 
[How TCP backlog works in Linux][3]



# Introduction
本文主要关注tcp这一类的可靠的传输层协议.
在之前的推理过程中, 我们已经能够明白, tcp的核心性质, 远没有想象
中那么复杂:

	multiplex, 这是传输层的本质, 标志用户地址, 也就是端口
	面向连接的, 收到的数据有序, 且存在session也就是握手挥手.
	可靠性, 报文一个都不能少.
	congestion control, 拥塞控制, 保证网络整体的性能.
	flow control, 流控, 保证对端可以良好稳定接收到报文.免得想双11的快递公司被爆仓.
	数据校验, 防止传输过程导致数据出错. 我常会想起水浒传中, 戴总送信那段.
对tcp就是围绕这6个核心的性质开始设计, 实现的.
无论你是看tcp的头部啊, 还是协议栈的代码基本上都逃不出这六点.
我不建议直接去看内核的tcp实现, 我是吃过苦头的. 
1. 很容易让人掉进去代码里面, 而忘了初衷. 
2. 内核的实现有很多工程性的问题, 这和tcp本身是无关的.比如tcp seq回绕的问题.

# 大话TCP 头部
从上往下, 这也标志了重要性.
先是, 源目的端口, 属于multiplex.
接着sequence number, 这是面向连接, 数据有序
接着是ack, 这是可靠性
再往下是, offset 处于数据管理需要和tcp无关,
tcp flags涉及到了多个性质,主要是握手和拥塞
之后window size, 这是流控.
之后是checksum, 保证传输过程中不会修改数据.
你看tcp真的不复杂, 我们只是没有遇到一个好的解读角度, tcp是有鼻子有眼
很生动的.

# Multiplexing
Ports can provide multiple endpoints on a single node. 
inet_hash_connect()

# Flow control
## receive windowsize
暗含tcp的最大64KB.
## Sliding window protocol 滑动窗口协议
首先滑动窗口本身就是一个协议

Sliding window protocols are used where reliable in-order delivery of packets is required.
For every ack packet received, the window slides by one packet (logically) to transmit one new packet.
发送方的发送窗口, 由发送了但没ack和 可用的发送空间,这个通常有对端的receive windowsize指定.
如果发送了一个可用的发送空间 = receive windowsize - 那个报文大小, 新来报文更新串口.
接收端的窗口就是最后收到连续到不连续的位置.

# Detection of transmission errors
Error --  checksum, the transport protocol may check that the data is not corrupted

# Connection-oriented communications
到底什么是面向连接的网上只有wikipedia给的解释最合理, 其他的扯到了别的性质.
两点: session and in order.
* Session
In computer science, in particular networking, a session is a semi-permanent 
interactive information interchange. 
[Instance of tcp session in BSD socket](http://www.scottklement.com/rpg/socktut/overview.html)
[TCP Session - Handshaking in protocol](http://www.dummies.com/how-to/content/network-basics-tcp-session-establishment-handshaki.html)
这个对应tcp的三次握手, 4次挥手, 
* in order
涉及到的另外一个概念Virtul circuit
A virtual circuit (VC) is a means of transporting data over a packet switched
 computer network in such a way that it appears as though there is a dedicated 
physical layer link between the source and destination end systems of this data. 
对应tcp 的接收队列, seq number
## Handshak
* kproxy reorder
chome ->syn(kproxy reocrd syn) -> firoyang.org
firoyang.org ->sync ack -> chrome
chrome -> ack -> firoyang.org
chrome -> GET(firoyang.org) kproxy match then send record syn then setup natinfo ->nginx
nginx -> tcp send fake syn ack->chrome
chrome -> ack -> nginx(then -> firoyang.org)
tcp_v4_do_rcv{
	sk->sk_state == TCP_ESTABLISHED
	tcp_rcv_established{
	len <= tcp_header_len =>
	tcp_ack -> tcp_fastretrans_alert{retrans ack and GET(firoyang) -> nginx
	}
}
nginx->GET ->firoyang.org
firoyang.org->nginx->chrome
## syn flood
[SYN Flood Mitigation with synsanity][4]
首先syn 也有超时5次指数1 2 4 8 16 32(第五次超时), 如果client发了一个syn就没了,
会被黑客利用. 解决办法是tcp_syncookies, 在syn队列满了的时候, 构造一个特别isn, 
丢掉syn entry, 等待ack,校验合法直接accpet.好像incomplete队列无限大一般.

## 深入理解seq 和ack
[TCP Sequence & Acknowledgment numbers](http://serverfault.com/questions/593037/tcp-sequence-acknowledgment-numbers)
这种问题就是咬住定义就完了.
The sequence number of the first data octet in this segment (except
    when SYN is present). If SYN is present the sequence number is the
    initial sequence number (ISN) and the first data octet is ISN+1.
If the ACK control bit is set this field contains the value of the
    next sequence number the sender of the segment is expecting to
    receive.  Once a connection is established this is always sent.
ACK最简单了, 整个tcp session只有一个segement没有ack number就是第一个syn.
其他情况, ack都是对面发来的seq + len + 1. (排除reorder包问题)
这里的问题是第三次握手的时候, 单独一个ack, 没有数据, 这时候segement的seq
应该是多少呢?RFC给出的是ISN + 1.但这个ack segment是个空数据.
也就出现了下面发一个GET的时seq 和ack number和这个ack segement一致.
那么如果交互过程中出现了空的ack呢?原理和这里是一样的. 空的ack 的seq也是
标志下一个segement的byte序号, 但是如果no data, 下一个segment的seq还是这个.
知道有数据了.
这里想说的是, seq确实是标注segment的data, 知识偶尔因为空ack导致了假象, 会
被后面的有数据的segement还原真相.
总结:
	syn 和 ack, 还有fin这种都不是数据, 不计算在seq里面.
	seq的计算只和实际的数据有关.
	小心处理no data这种情况, 就OK了.
## Time wait
[TCP的TIME_WAIT快速回收与重用](http://blog.csdn.net/dog250/article/details/13760985)
双工, 被动关闭收到ack就%100圆满了.而clinet就不能确认被动关闭是否收到ack, 
显然被动关闭方不能在ack了, 如果下去, 还有个完, 所以两害相权, 取其轻.clinet来吧.

1. 为了server考虑, server的session 链接必须别正常正确的关闭!
如果没有time wait, 而且client的ack丢了, server 重传fin ack, clinet的linux
发现这个fin对应的sock不存在, 直接RST, server异常关闭, 应用程序会检测到错误.
不友好.

2. 为了clinet, 不受旧server 干扰.
这也是为什么要等2MSL
* tcp_timestamps tcp_tw_recycle
tcp_time_wait
一起用如果recycle 不ok就是time wait就是TCP_TIMEWAIT_LEN（60s）

两种方法可以进入time wait 状态 tw_substate做区分. 
FIN_WAIT_2 收到fin 见tcp_fin()这个函数
这个, time wait 如果没有设置recycle就是TCP_TIMEWAIT_LEN,设置了就是rto
可以说rto的值真的要比TCP_TIMEWAIT_LEN要小.

FIN_WAIT_2 超时假的time wait状态.貌似tcp_keepalive_timer()
没有遵从协议但是没有break协议,是个优化.
tcp_sock结构占用的资源要比tcp_timewait_sock结构占用的资源多, tcp_done干掉sock.
在TIME_WAIT下也可以处理连接的关闭。
这个,还一样time_wait是和rto TCP_TIMEWAIT_LEN有关.
inet_twsk_schedule设置等的时间. 

* tcp_tw_reuse
貌似很激进.
FIXME
server 端玩蛋去, 本身像80, 自带重用技能...
用在clinet段inet_hash_connect检查是否可以重用TIME_WAIT状态的套接字的端口.

* tcp_fin_timeout
这个参数是FIN_WAIT_2 转到TIME_WAIT的时间.
跟time wait时间, 没有直接联系! 好多blog都直接说成time wait的时间.
这里是间接作用.
FIXME...
而time wait的时间看代码, 要不然是rto 要不然就是TCP_TIMEWAIT_LEN(60s)
tcp_time_wait
                if (recycle_ok) {
                        tw->tw_timeout = rto;     
                } else {
                        tw->tw_timeout = TCP_TIMEWAIT_LEN;
                        if (state == TCP_TIME_WAIT)
                                timeo = TCP_TIMEWAIT_LEN;
                }
如果启用recycle 就是rto, 这个rto是const int rto = (icsk->icsk_rto << 2) - (icsk->icsk_rto >> 1);
3.5倍的icsk_rto
在FIN_WAIT_2状态下没有接收到FIN包就进入TIME_WAIT的情况下，如果tcp_fin_timeout的值设置的太小，可能会导致TIME_WAIT套接字（子状态为FIN_WAIT_2）过早地被释放，这样对端发送的FIN（短暂地延迟或者本来就是正常的时间到达）到达时就没有办法处理，导致连接不正常关闭，所以tcp_fin_timeout参数的值并不是越小越好，通常设置为30S比较合适。
# Reliability
## RTT
计算发送和返回ack的时间差.
tcp_rtt_estimator()
## ARQ
ack and timeout
Sliding window protocol is based on automatic repeat request/ARQ
My conclusion: in practice TCP is a mixture between both GBN and SR.
* Go-Back-N
* Selective repeat

#FAQ
* What about TCP sequence number warp around
PAWS use timestamp and RTT to solve this problem.
##Timer
*sk_timer
listen: synack
estblished: keepalive
timewait:
# optimization
## Nagle
Nagle算法并没有阻止发送小包，它只是阻止了发送大量的小包！
Nagle算法的初衷：避免发送大量的小包，防止小包泛滥于网络，理想情况下，
对于一个TCP连接而言，网络上每次只能一个小包存在。它更多的是端到端意义上的优化。
正是交互式应用引入了大量的小包，Nagle算法所作用的正是交互式应用！
## Cork
CORK算法的初衷：提高网络利用率，理想情况下，完全避免发送小包，仅仅发送满包以及不得不发的小包。
CORK的真正初衷是提高载荷率，提高网络利用率

## FIXME
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
* tcp的核心发包函数tcp_write_xmit and tcp_transmit_skb
