#ifndef HEADER_GLOBAL_H
#define HEADER_GLOBAL_H

#define DEFAULTWORKROOT  	"/usr/local/wystream/"
#define ETCDIR      		"etc"
#define CONFIGFILE  		"Config.xml"
#define VERSION 			"Version: WeiYun TCP Stream Upload Accelerator - WYStream_20140115"

extern  int epollfd;
#define MAXEPOLLFD	256
#define MAXEVENT	50000

extern int gExitFlag;

#define FILESLICELEN	(10*1024*1024)		// 10MB



#define MAX_STRING	4096
#define MAX_ADD_HEADERS	20

#define DEFAULT_RECEIVE_MAXTHREAD_NUM 5		// max receiving task/thread
#define DEFAULT_UPLOAD_MAXTHREAD_NUM  4		// max uploading task/thread


#define WEIYUN_HTTP_POST 0			// HTTP POST 类型
#define WEIYUN_TCP_SLICE 1			// HTTP 封装的 TCP 分片
#define WEIYUN_CHROME_TCP_SLICE	2	// HTTP 封装的 TCP 分片(Chrome)

#define S_OK 	0
#define S_ERROR -1

#endif 

