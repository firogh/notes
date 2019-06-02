---
tags: [ net ] 
title: Transport layer
date: 2015-02-27T15:46:13+08:00 
category: net
---
# References
man tcp
[A Roadmap for Transmission Control Protocol (TCP) Specification Documents](http://tools.ietf.org/html/rfc7414)
[Requirements for Internet Hosts -- Communication Layers](https://tools.ietf.org/html/rfc1122)
[TRANSMISSION CONTROL PROTOCOL 1981](http://tools.ietf.org/html/rfc793)

# Send tcp message
server:
nc -l -p 2046
nc  127.0.0.1 2046

# Syn queue and listen queue
[How TCP backlog works in Linux](http://veithen.github.io/2014/01/01/how-tcp-backlog-works-in-linux.html)

# Syn flood
[SYN Flood Mitigation with synsanity](https://githubengineering.com/syn-flood-mitigation-with-synsanity/)

# ucopy
ucopy.task was temporarily enabled in tcp_recvmsg in order to read more data after processing receive queue.
It will be disabled in the same fuction. So go back to tcp_prequeue, we know if user need more data, then we queue new skb in ucopy.preueue otherwise not.
In tcp_rcv_established(), the tp->ucopy.task == current indicats we are in process context.

# Multiplexing
Ports can provide multiple endpoints on a single node. 
inet_hash_connect()
inet_sock: dport, sport

# MSS
MSS tcp_sock->mss_cache in tcp_sync_mss not minus SACK option
        in *tcp_current_mss* minus SACK option
sudo tcpdump -s0 -p -ni enp0s31f6 '(ip and ip[20+13] & tcp-syn != 0)'
send: tcp_advertise_mss
tcp_current_mss
[用 SystemTap 找出 TCP 如何決定 MSS 的值](https://medium.com/fcamels-notes/%E7%94%A8-systemtap-%E6%89%BE%E5%87%BA-tcp-%E5%A6%82%E4%BD%95%E6%B1%BA%E5%AE%9A-mss-%E7%9A%84%E5%80%BC-4b6b7a969d04)
mss derives from MTU; see rfc6691
When TCP is used in a situation where either the IP or TCP headers are not minimum, the sender must reduce the amount of TCP data in any given packet by the number of octets used by the IP and TCP options.

# Time-wait
[Coping with the TCP TIME-WAIT state on busy Linux servers](https://vincent.bernat.ch/en/blog/2014-tcp-time-wait-state-linux)
tcp: remove tcp_tw_recycle - 4396e46187ca5070219b81773c4e65088dac50cc

双工, 1. 被动关闭收到ack就%100圆满了.而clinet就不能确认被动关闭是否收到ack, 
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
如果启用recycle 就是rto, 这个rto是const int rto = (icsk->icsk_rto << 2) - (icsk->icsk_rto >> 1); 3.5倍的icsk_rto
在FIN_WAIT_2状态下没有接收到FIN包就进入TIME_WAIT的情况下，如果tcp_fin_timeout的值设置的太小，可能会导致TIME_WAIT套接字（子状态为FIN_WAIT_2）过早地被释放，这样对端发送的FIN（短暂地延迟或者本来就是正常的时间到达）到达时就没有办法处理，导致连接不正常关闭，所以tcp_fin_timeout参数的值并不是越小越好，通常设置为30S比较合适。

# TCP receive
# Established socket receive window
https://tools.ietf.org/html/rfc793
https://tools.ietf.org/html/rfc1122
tcp_transmit_skb
th->ack_seq		= htonl(tp->rcv_nxt);
tcp_data_queue
tp->rcv_nxt = TCP_SKB_CB(skb)->end_seq;
----------------------------------

# Connection-oriented communications
## Handshak of kproxy
chome -> syn(kproxy reocrd syn) -> firoyang.org
firoyang.org -> sync ack -> chrome
chrome -> ack -> firoyang.org
chrome -> GET(firoyang.org) kproxy match then send record syn then setup natinfo -> nginx
nginx -> tcp send fake syn ack-> chrome
chrome -> ack -> nginx(then -> firoyang.org)
tcp_v4_do_rcv{
	sk->sk_state == TCP_ESTABLISHED
	tcp_rcv_established{
	len <= tcp_header_len =>
	tcp_ack -> tcp_fastretrans_alert{retrans ack and GET(firoyang) -> nginx
	}
}
nginx-> GET -> firoyang.org
firoyang.org-> nginx-> chrome

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
## Timer expiring retransmiter
tcp_retransmiter_timer()...->tcp_transmit_skb()
## reponse for receiving an ACK
tcp_data_snd_check()->tcp_write_xmit()
## Timer
sk_timer
listen: synack
estblished: keepalive
timewait:
