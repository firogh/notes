

ngx_http_proxy_handler -> ngx_http_upstream_init -> ngx_http_upstream_init_request -> 
ngx_http_upstream_connect -> ngx_event_connect_peer->ngx_socket() &&bind() && connect()
./http/modules/ngx_http_proxy_module.c
