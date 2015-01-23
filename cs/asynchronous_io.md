---
layout: post
date: 2014-12-28
title:  Asynchronous I/O
category: cs
---

#Common concepts
a hard drive seeking a track to read or write; this is often orders of magnitude slower than the switching of electric current.


* The simplest software solution. polling-based system.
#Forms
* Prcesses, pipeline

* synchronous I/O multiplexing and I/O event notification facility
select, poll
epoll
For the ease of use, the select loop is implemented as an *event loop* with callbacks.

#libevent and libev is a well-designed *event loop*.
Check shadowsocks for using of libev.


