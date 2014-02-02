#include "PServer.h"
#include "Util.h"
#include "CLog.h"
#include "Global.h"
#include "ConfPara.h"

#include <unistd.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <limits.h>
#include <linux/netfilter_ipv4.h>

PServer::PServer()
{
	socketFd = -1;
}

PServer::~PServer()
{
	if (socketFd > 0){
		close(socketFd);
		socketFd = -1;
	}
}

bool PServer::initSocket()
{
	struct sockaddr_in local;
	
	socketFd = socket(PF_INET, SOCK_STREAM, 0);
	if (socketFd < 0){
		if (g_CP.getIsLogPrint())
			g_Logger.writeLog("In %s:%d, Create socket error!", __FILE__, __LINE__);

		return false;
	}

	// Set socket to non block
	if (setNonblock(socketFd) < 0){
		if (g_CP.getIsLogPrint())
			g_Logger.writeLog("In %s:%d, Set socket to nonblock error!", __FILE__, __LINE__);

		goto INIT_SOCKET_ERROR_RETURN;
	}

	// Set socket to nolinger and reuseaddr
	if (disableLinger(socketFd) < 0){
		if (g_CP.getIsLogPrint())
			g_Logger.writeLog("In %s:%d, Set socket to nonblock error!", __FILE__, __LINE__);

		goto INIT_SOCKET_ERROR_RETURN;
	}
	
	if (enableReuseaddr(socketFd) < 0){
		if (g_CP.getIsLogPrint())
			g_Logger.writeLog("In %s:%d, Set socket to reuseaddr error!", __FILE__, __LINE__);

		goto INIT_SOCKET_ERROR_RETURN;
	}

	bzero(&local, sizeof(local));   
	local.sin_family = AF_INET;
	local.sin_addr.s_addr = inet_addr(g_CP.getListenIP());
	local.sin_port = htons(atoi(g_CP.getListenPort()));	
	if( bind(socketFd, (struct sockaddr *) &local, sizeof(local)) < 0){   
		if (g_CP.getIsLogPrint())
			g_Logger.writeLog("In %s:%d, bind server socket error!", __FILE__, __LINE__);

		goto INIT_SOCKET_ERROR_RETURN;
	}   

	if (listen(socketFd, 8000) < 0){
		if (g_CP.getIsLogPrint())
			g_Logger.writeLog("In %s:%d, listen socket error!", __FILE__, __LINE__);

		goto INIT_SOCKET_ERROR_RETURN;
	}

	return true;
	
INIT_SOCKET_ERROR_RETURN:
	close(socketFd);
	return false;
}

void PServer::Run(struct epoll_event* event)
{
	struct epoll_event ev;
	ev.data.fd = socketFd;
	ev.events = EPOLLIN | EPOLLET;

	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, socketFd, &ev) == -1) {   
		if (g_CP.getIsLogPrint())
			g_Logger.writeLog("In %s:%d, epoll_ctl error!", __FILE__, __LINE__);

		return;
	}

	MainLoop(event);
}

void PServer::MainLoop(struct epoll_event* event)
{
	int nfds;

	while(true){		
		if (gExitFlag){
			if (g_CP.getIsLogPrint())
				g_Logger.writeLog("In %s:%d, Program Exit!", __FILE__, __LINE__);

			return;
		}

		checkPeerEpollout();
		
		nfds = epoll_wait(epollfd, event, MAXEVENT, 1000);
		if (nfds < 0)			{	continue;	}
		else if (0 == nfds )	{	continue;	}

		for (int i = 0; i < nfds; i++){
			if( event[i].data.fd == socketFd){
				Accepter();
			}else if (event[i].events & EPOLLIN){
				Worker *pWorker = (Worker *) event[i].data.ptr;
				readFun(pWorker);
			}else if (event[i].events & EPOLLOUT){
				Worker *pWorker = (Worker *) event[i].data.ptr;
				writeFun(pWorker);
			}else if (event[i].events & EPOLLERR){

			}
		}
	}
}

void PServer::readFun(Worker * &tpWorker)
{
	int retVal;
	Worker *pWorker = tpWorker;

	bool isSlave = pWorker->getIsSlave();

	// Server packet
	if (isSlave){
		pWorker = (Worker *) tpWorker->getMasterPtr();
		pWorker->setServerConnTime(time(NULL));
		retVal = pWorker->readServerRes();
		if (S_READ_ERROR == retVal){
			if (TASK_NORMAL == pWorker->getTaskType()){
				pWorker->setServerState(S_SERVER_NORMAL_ERROR);
			}else if (TASK_DATA == pWorker->getTaskType()){
				pWorker->setServerState(S_SERVER_DATA_ERROR);
			}				
		}else if (S_READ_OK == retVal){
			
		}
		
		return;
	}

	// Client packets
	pWorker->setClientConnTime(time(NULL));
	if (S_CLIENT_CONNECT_RECEIVING == pWorker->getStartState()){
		retVal = pWorker->readFirstConnData();
		if (S_READ_OK == retVal){

		}else if (S_READ_ERROR == retVal){
			pWorker->setStartState(S_CLIENT_CONNECT_ERROR);
		}else if (S_READ_INCOMPLETE == retVal){

		}
	}else if (S_CLIENT_NORMAL_WORKING == pWorker->getClientState()){
		retVal = pWorker->readNormalClientData();
		if (S_READ_OK == retVal){
			
		}else if (S_READ_ERROR == retVal){
			pWorker->setClientState(S_CLIENT_NORMAL_ERROR);
		}else if (S_READ_INCOMPLETE == retVal){
			
		}		
	}else if (S_CLIENT_DATA_DATA_RECV_INDOING == pWorker->getClientState()){
		retVal = pWorker->readDataClientData();
		if (S_READ_ERROR == retVal){
			pWorker->setClientState(S_CLIENT_DATA_ERROR);
		}else{

		}
	}
}

void PServer::writeFun(Worker * &tpWorker)
{
	int retVal;
	Worker *pWorker = tpWorker;

	bool isSlave = pWorker->getIsSlave();
	if (isSlave){
		pWorker = (Worker *)tpWorker->getMasterPtr();
		pWorker->setServerConnTime(time(NULL));
		retVal = pWorker->sendDataToServer();
		if (S_SEND_ERROR == retVal){
			if (g_CP.getIsLogPrint()){
				g_Logger.writeLog("In %s:%d, write data to server error!", __FILE__, __LINE__);
			}

			if (TASK_NORMAL == pWorker->getTaskType())
				pWorker->setServerState(S_SERVER_NORMAL_ERROR);
			else if (TASK_DATA == pWorker->getTaskType())
				pWorker->setServerState(S_SERVER_DATA_ERROR);
		}else{
			if (S_CLIENT_DATA_OFFSETREQ_RECV_OK == pWorker->getClientState() &&
				S_SERVER_DATA_OFFSETREQ_SEND_INDOING == pWorker->getServerState()){
				pWorker->setServerState(S_SERVER_DATA_OFFSETRES_RECV_INDOING);
			}
		}
	}else{
		pWorker->setClientConnTime(time(NULL));
		retVal = pWorker->sendDataToClient();
		if (S_SEND_ERROR == retVal){
			if (g_CP.getIsLogPrint()){
				g_Logger.writeLog("In %s:%d, write data to client error!", __FILE__, __LINE__);
			}

			if (TASK_NORMAL == pWorker->getTaskType())
				pWorker->setClientState(S_CLIENT_NORMAL_ERROR);
			else if (TASK_DATA == pWorker->getTaskType())
				pWorker->setClientState(S_CLIENT_DATA_ERROR);
		}else{
			if (TASK_DATA == pWorker->getTaskType()){
				if (S_CLIENT_DATA_OFFSETRES_SEND_INDOING == pWorker->getClientState() &&
					S_SERVER_DATA_OFFSETRES_RECV_OK == pWorker->getServerState()){
					pWorker->setClientState(S_CLIENT_DATA_DATA_RECV_INDOING);
					pWorker->setServerState(S_SERVER_DATA_DATA_SEND_INDOING);
					pWorker->setFileRecvLen(0);
					pWorker->setFileSendLen(0);
					//printf("In %s:%d, begin to recv client data and send to server!\n", __FILE__, __LINE__);

					pWorker->setClientFileCacheWriteIndex(0);
					pWorker->initClientFileCacheWrite(0);
					pWorker->setClientFileCacheReadIndex(0);
					pWorker->initClientFileCacheRead(0);
				}else if (S_CLIENT_DATA_DATARES_SEND_INDOING == pWorker->getClientState()){
					pWorker->setClientState(S_CLIENT_DATA_OK);
					printf("====================================\n");
					printf("*       Client Data Recv OK        *\n");
					printf("*        Client Side Finish        *\n");
					printf("====================================\n");
				}
			}
		}
	}
}

void PServer::RemovePeerByItr(list<Worker *>::iterator &itrPrev)
{
	struct epoll_event ev;
	std::list<Worker *>::iterator itr;
	Worker *pWorker = *itrPrev;
		
	epoll_ctl(epollfd, EPOLL_CTL_DEL, pWorker->getClientSock(), &ev);
	epoll_ctl(epollfd, EPOLL_CTL_DEL, pWorker->getServerSock(), &ev);
	itr = mWorkingPeerList.erase(itrPrev);

	deleteFileByFolder(pWorker->getCacheHome());
	delete (Worker *)pWorker->getSlavePtr();
	delete pWorker;

	itrPrev = itr;
}

void PServer::checkPeerEpollout()
{
	if (mWorkingPeerList.size()){
		std::list<Worker *>::iterator itr;
		for( itr = mWorkingPeerList.begin(); itr != mWorkingPeerList.end(); ){
			Worker *pWorker = *itr;

			// Client Connecting error before taskType can be defined
			if (S_CLIENT_CONNECT_ERROR == pWorker->getStartState()){
				RemovePeerByItr(itr);
				continue;
			}

			// normal working exit
			if ((S_CLIENT_NORMAL_ERROR == pWorker->getClientState() || S_CLIENT_NORMAL_OK == pWorker->getClientState()) &&
				(S_SERVER_NORMAL_ERROR == pWorker->getServerState() || S_SERVER_NORMAL_OK == pWorker->getServerState())){
				//printf("In %s:%d, delete a normal client!\n", __FILE__, __LINE__);
				RemovePeerByItr(itr);
				continue;
			}else if ((S_CLIENT_NORMAL_ERROR == pWorker->getClientState() || S_CLIENT_NORMAL_OK == pWorker->getClientState()) &&
						(false == pWorker->isNeedToSendToServer())){
				//printf("In %s:%d, delete a normal client! client error/ok, no data to server\n", __FILE__, __LINE__);
				RemovePeerByItr(itr);
				continue;
			}else if ((S_SERVER_NORMAL_ERROR == pWorker->getServerState() || S_SERVER_NORMAL_OK == pWorker->getServerState()) &&
						(false == pWorker->isNeedToSendToClient())){
				//printf("In %s:%d, delete a normal client! server error/ok, no data to client\n", __FILE__, __LINE__);
				RemovePeerByItr(itr);
				continue;
			}
			
			// data working exit
			if (S_CLIENT_DATA_OK == pWorker->getClientState() && 
				S_SERVER_DATA_OK == pWorker->getServerState()){
				//printf("In %s:%d, ready to remove data client!\n", __FILE__, __LINE__);
				RemovePeerByItr(itr);
				continue;
			}

			if (S_CLIENT_DATA_ERROR == pWorker->getClientState() || 
				S_SERVER_DATA_ERROR == pWorker->getServerState()){
				RemovePeerByItr(itr);
				continue;
			}

			// timeout exit
			if ((S_CLIENT_DATA_OK != pWorker->getClientState()) && 
				(S_CLIENT_DATA_ERROR != pWorker->getClientState()) &&
				(difftime(time(NULL), pWorker->getClientConnTime()) > 30.0 * 60)){
				RemovePeerByItr(itr);
				continue;
			}

			if ((S_SERVER_DATA_OK != pWorker->getServerState()) &&
				(S_SERVER_DATA_ERROR != pWorker->getServerState()) &&
				(difftime(time(NULL), pWorker->getServerConnTime()) > 30.0 * 60)){
				RemovePeerByItr(itr);
				continue;	
			}

			// Normal Working, check to set epoll out
			if (S_SERVER_NORMAL_WORKING == pWorker->getServerState() &&
				pWorker->isNeedToSendToServer()){
				struct epoll_event ev;
				ev.events = EPOLLOUT | EPOLLIN | EPOLLET;   
				ev.data.ptr = (void *)(pWorker->getSlavePtr());
				epoll_ctl(epollfd, EPOLL_CTL_MOD, pWorker->getServerSock(), &ev);
			}

			if (S_CLIENT_NORMAL_WORKING == pWorker->getClientState() &&
				pWorker->isNeedToSendToClient()){
				struct epoll_event ev;
				ev.events = EPOLLOUT | EPOLLIN | EPOLLET;   
				ev.data.ptr = (void *)pWorker;
				epoll_ctl(epollfd, EPOLL_CTL_MOD, pWorker->getClientSock(), &ev);
			}

			// Data Working, check to set epoll out
			if (S_CLIENT_DATA_OFFSETREQ_RECV_OK == pWorker->getClientState() &&
				S_SERVER_DATA_OFFSETREQ_SEND_INDOING == pWorker->getServerState() &&
				pWorker->isNeedToSendToServer()){
				struct epoll_event ev;
				ev.events = EPOLLOUT | EPOLLIN | EPOLLET;   
				ev.data.ptr = (void *)(pWorker->getSlavePtr());
				epoll_ctl(epollfd, EPOLL_CTL_MOD, pWorker->getServerSock(), &ev);
			}

			if (S_CLIENT_DATA_OFFSETRES_SEND_INDOING == pWorker->getClientState() &&
				S_SERVER_DATA_OFFSETRES_RECV_OK == pWorker->getServerState() &&
				pWorker->isNeedToSendToClient()){
				struct epoll_event ev;
				ev.events = EPOLLOUT | EPOLLIN | EPOLLET;   
				ev.data.ptr = (void *)pWorker;
				epoll_ctl(epollfd, EPOLL_CTL_MOD, pWorker->getClientSock(), &ev);
			}

			if (S_SERVER_DATA_DATA_SEND_INDOING == pWorker->getServerState() &&
				pWorker->isNeedToSendToServer()){
				struct epoll_event ev;
				ev.events = EPOLLOUT | EPOLLIN | EPOLLET;   
				ev.data.ptr = (void *)(pWorker->getSlavePtr());
				epoll_ctl(epollfd, EPOLL_CTL_MOD, pWorker->getServerSock(), &ev);
			}

			if (S_CLIENT_DATA_DATARES_SEND_INDOING == pWorker->getClientState() &&
				pWorker->isNeedToSendToClient()){
				struct epoll_event ev;
				ev.events = EPOLLOUT | EPOLLIN | EPOLLET;   
				ev.data.ptr = (void *)pWorker;
				epoll_ctl(epollfd, EPOLL_CTL_MOD, pWorker->getClientSock(), &ev);
			}

			itr++;
		}
	}
}

void PServer::Accepter()
{
	int conn_sock;
	struct sockaddr_in peerAddr;
	struct epoll_event ev;

	socklen_t addrLen= sizeof(struct sockaddr_in);
	bzero(&peerAddr, sizeof(peerAddr));

	while ((conn_sock = accept(socketFd, (struct sockaddr *) &peerAddr, (socklen_t *)&addrLen)) > 0) {   
		setNonblock(conn_sock);   
		Worker * pWorker = new Worker();
		if (NULL == pWorker){
			if (g_CP.getIsLogPrint())
				g_Logger.writeLog("In %s:%d, No enough memory for create new Worker!", __FILE__, __LINE__);

			close(conn_sock);
			continue;
		}

		struct sockaddr_in originDst = {0};
		socklen_t sin_size = sizeof(originDst);
		if(getsockopt(conn_sock, SOL_IP, SO_ORIGINAL_DST, &originDst, &sin_size) != 0){
			if (g_CP.getIsLogPrint())
				g_Logger.writeLog("In %s:%d, Cann't get original server info!", __FILE__, __LINE__);

			close(conn_sock);
			if (NULL != pWorker)	{delete pWorker; pWorker = NULL;}
			continue;

		}

		printf("Original Server Info: %s:%d\n", inet_ntoa(originDst.sin_addr), ntohs(originDst.sin_port));
		pWorker->setClientSock(conn_sock);
		pWorker->setServerIP(inet_ntoa(originDst.sin_addr));
		pWorker->setServerPort(ntohs(originDst.sin_port));
		pWorker->setStartState(S_CLIENT_CONNECT_CONNECTING);
		if (pWorker->initTask() < 0){
			delete pWorker;		pWorker = NULL;
		}		

		// Epoll Add Clientsock
		ev.events = EPOLLIN | EPOLLET;   
		ev.data.ptr = (void *) pWorker;
		if (epoll_ctl(epollfd, EPOLL_CTL_ADD, conn_sock, &ev) < 0 ) {
			if (g_CP.getIsLogPrint())
				g_Logger.writeLog("In %s:%d, Epoll add worker error! errno = %d, msg = %s", __FILE__, __LINE__, errno, strerror(errno));

			delete pWorker;	 	pWorker = NULL;
			continue;
		}

		Worker * pSlaveWorker = new Worker();
		if (NULL == pSlaveWorker){
			if (g_CP.getIsLogPrint())
				g_Logger.writeLog("In %s:%d, No enough memory for create new slave Worker!", __FILE__, __LINE__);

			delete pWorker;		pWorker = NULL;
			continue;
		}
		
		pSlaveWorker->setIsSlave(true);
		pSlaveWorker->setMasterPtr((void *)pWorker);
		pWorker->setSlavePtr((void *)pSlaveWorker);

		// Epoll Add Serversock
		ev.events = EPOLLIN | EPOLLET;   
		ev.data.ptr = (void *) pSlaveWorker;
		if (epoll_ctl(epollfd, EPOLL_CTL_ADD, pWorker->getServerSock(), &ev) < 0 ) {
			if (g_CP.getIsLogPrint())
				g_Logger.writeLog("In %s:%d, Epoll add slave worker error!", __FILE__, __LINE__);

			delete pWorker;	 		pWorker = NULL;
			delete pSlaveWorker;	pSlaveWorker = NULL;
			continue;
		}		
		
		mWorkingPeerList.push_back(pWorker);

		pWorker->setStartState(S_CLIENT_CONNECT_RECEIVING);
		pWorker->setClientState(S_CLIENT_UNDEFINE);
		pWorker->setServerState(S_SERVER_UNDEFINE);
		pWorker->setClientConnTime(time(NULL));
		pWorker->setServerConnTime(time(NULL));

		bzero(&peerAddr, sizeof(peerAddr));
	}
} 

