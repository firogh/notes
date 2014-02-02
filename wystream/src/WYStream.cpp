#include "Util.h"
#include "CLog.h"
#include "Global.h"
#include "ConfPara.h"
#include "PServer.h"

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/epoll.h>
#include <string.h>
#include <unistd.h>

int gExitFlag = 0;
int epollfd;	   
struct epoll_event event[MAXEVENT];

PServer g_PServer;
ConfigurationParameter g_CP;

int main(int argc, char *argv[])
{
	printf("\nSky Drive Upload WeiYun App TCP Stream Program Start!\n\n");

	Random_init();
	signal(SIGINT,  sig_catch);		signal(SIGQUIT, sig_catch);
	signal(SIGKILL, sig_catch);  	signal(SIGUSR1, sig_catch);
	signal(SIGPIPE, SIG_IGN);		signal(SIGCHLD, SIG_IGN);

	if ( !g_CP.readConf() ){
		printf("In %s:%d, Init configuration parameter error!\n", __FILE__, __LINE__);
		return -1;
	}

	if (CreateDir(g_CP.getCacheRoot()) < 0){
		printf("In %s:%d, Create Cache Root Error!", __FILE__, __LINE__);		
		return -1;
	}

	if (g_CP.getIsLogPrint()){	
		g_CP.cmdDump();
		g_Logger.init(g_CP.getCacheRoot());
	}

	if (g_CP.getIsLogPrint()){
		g_Logger.writeLog("WY TCP Stream Accelerator Start!");
	}

	epollfd = epoll_create(MAXEPOLLFD);
	if( epollfd < 0 ){
		if (g_CP.getIsLogPrint())
			g_Logger.writeLog("In %s:%d, epoll create error!", __FILE__, __LINE__);

		return -1;
	}
	memset(event,0,sizeof(struct epoll_event) * MAXEVENT);
	if (SetMaxFd()<0){
		if (g_CP.getIsLogPrint())
			g_Logger.writeLog("In %s:%d, Cann't set max fd!", __FILE__, __LINE__);

		return -1;
	}

	if (!g_PServer.initSocket()){
		if (g_CP.getIsLogPrint())
			g_Logger.writeLog("In %s:%d, Init server socket error!", __FILE__, __LINE__);

		return -1;
	}

	// Start the server loop
	g_PServer.Run(event);
}
