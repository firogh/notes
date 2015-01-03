upload speed fluctuation
=========================
config
nginx
I guessed fluctuation maybe reason in config!

1 kproxy -> nginx.config -> wy-server
Modifed below parameters				Ressults 
State	 1								Not work
client_body_buffers									
client_body_postpone_size	
304840
!(ip.addr == 192.168.5.250) and !(ip.addr == 74.125.0.0/16) and !(ip.addr == 112.90.143.178)
State	2								
Analysis capured packages about weiyun






Chrolet - Nginx 的Boot up
=========================
== in ==
Options
Configuration
Binary module

== do ==
Configuration parse			conf_ctx
							server config
							location cofig
memory pool
sinals
process
epoll
== out ==
listening state



Chrolet - 一个报文的处理
=====================
== in ==
1 接收数据 -> request		ngx_http_init_request

== do ==
2 分析 request				Header
							Body
3 virtual server			addr:port, server name
4 location
5 phase handle
							Rewrite
							Access
							Limit
							LOg
== out ==
6 产生reponse
7 filter header & body


#Etc
ngx_http_proxy_handler -> ngx_http_upstream_init -> ngx_http_upstream_init_request -> 
ngx_http_upstream_connect -> ngx_event_connect_peer->ngx_socket() &&bind() && connect()
./http/modules/ngx_http_proxy_module.c
