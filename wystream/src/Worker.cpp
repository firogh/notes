#include "Worker.h"
#include "ConfPara.h"
#include "CLog.h"
#include "Util.h"
#include "http.h"
#include "pb/msgprotobuf.pb.h"

#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <arpa/inet.h>

#include <iostream>
using namespace std;

Worker::Worker()
{
	clientSock = -1;
	serverSock = -1;

	memset(serverIP, 0, sizeof(serverIP));
	serverPort = 0;

	isFirstDecode = true;
	clientConnBufLen = 0;

	memset(cacheHome, 0, sizeof(cacheHome));

	clientPktCacheFd = -1;
	clientPktCacheReadFd = -1;
	serverPktCacheFd = -1;
	serverPktCacheReadFd = -1;
	clientPktCacheLen = 0;
	serverPktCacheLen = 0;
	serverPktSendLen = 0;
	clientPktSendLen = 0;
	
	isSlave = false;
	pSlave = NULL;
	pMaster = NULL;

	taskType = TASK_UNDEFINE;
	clientState = S_CLIENT_UNDEFINE;
	serverState = S_SERVER_UNDEFINE;


	fileSize = 0;
	fileOffset = 0;
	fileRecvLen = 0;
	fileSendLen = 0;

	clientConnTime = time(NULL);
	serverConnTime = time(NULL);

	clientFileCacheCurrentWriteFd = -1;
	clientFileCacheCurrentWriteIndex = -1;
	clientFileCacheCurrentWriteOffset = 0;
	clientFileCacheCurrentReadFd = -1;
	clientFileCacheCurrentReadIndex = -1;
	clientFileCacheCurrentReadOffset = 0;
}

Worker::~Worker()
{
	closeClientSock();
	closeServerSock();
	closeClientPktCacheFd();
	closeClientPktCacheReadFd();
	closeServerPktCacheFd();
	closeServerPktCacheReadFd();
	clearCacheHome();

	printf("====================================\n");
	if (isSlave)
		printf("**         Slave Worker         \n");
	else
		printf("**         Master Worker         \n");
	
	if (TASK_NORMAL == taskType)
		printf("**  Normal Worker Delete! Task Finished! \n");
	else if (TASK_DATA == taskType)
		printf("** Data Worker Delete! Task Finished! \n");
	else
		printf("** Undefined Worker Delete! Task Finished! \n");
	printf("====================================\n");
}

void Worker::closeClientPktCacheFd()
{
	if (clientPktCacheFd > 0){
		close(clientPktCacheFd);
		clientPktCacheFd = -1;
	}
}

void Worker::closeClientPktCacheReadFd()
{
	if (clientPktCacheReadFd > 0){
		close(clientPktCacheReadFd);
		clientPktCacheReadFd = -1;
	}
}

void Worker::closeServerPktCacheReadFd()
{
	if (serverPktCacheReadFd > 0){
		close(serverPktCacheReadFd);
		serverPktCacheReadFd = -1;
	}
}

void Worker::closeServerPktCacheFd()
{
	if (serverPktCacheFd > 0){
		close(serverPktCacheFd);
		serverPktCacheFd = -1;
	}
}

void Worker::closeClientSock()
{
	if (clientSock > 0){
		close(clientSock);
		clientSock = -1;
	}
}
	
void Worker::closeServerSock()
{
	if (serverSock > 0){
		close(serverSock);
		serverSock = -1;
	}
}

void Worker::setClientSock(int tSock)
{
	clientSock = tSock;
}

void Worker::setServerIP(char *tServIP)
{
	strcpy(serverIP, tServIP);
}

void Worker::setServerPort(unsigned short int tServPort)
{
	serverPort = tServPort;
}

void Worker::setStartState(int tStartState)
{
	startState = tStartState;
}

char * Worker::getCacheHome()
{
	return cacheHome;
}

void Worker::setClientConnTime(time_t tClientConnTime)
{
	clientConnTime = tClientConnTime;
}

void Worker::setServerConnTime(time_t tServerConnTime)
{
	serverConnTime = tServerConnTime;
}

time_t Worker::getClientConnTime()
{
	return clientConnTime;
}

time_t Worker::getServerConnTime()
{
	return serverConnTime;
}

int Worker::getStartState()
{
	return startState;
}

bool Worker::isNeedToSendToServer()
{
	if (TASK_NORMAL == taskType)
		return (clientPktCacheLen > serverPktSendLen)? true:false;
	else if (TASK_DATA == taskType){
		if (S_CLIENT_DATA_OFFSETREQ_RECV_OK == clientState &&
			S_SERVER_DATA_OFFSETREQ_SEND_INDOING == serverState){
			return (clientPktCacheLen > serverPktSendLen)? true:false;
		}else if (S_SERVER_DATA_DATA_SEND_INDOING == serverState){
			return (fileRecvLen > fileSendLen)? true:false;
		}
	}

	return false;
}

bool Worker::isNeedToSendToClient()
{
	return (serverPktCacheLen > clientPktSendLen)? true:false;
}

void Worker::setClientState(int tClientState)
{
	clientState = tClientState;
}

void Worker::setServerState(int tServerState)
{
	serverState = tServerState;
}

int Worker::readServerRes()
{
	int readCnt;

	if (TASK_NORMAL == taskType){
		do{
			readCnt = read(serverSock, serverRecvBuf, 2048);
			if (readCnt < 0){
				if ((errno != EAGAIN) || (errno != EWOULDBLOCK)){
					if (g_CP.getIsLogPrint())
						g_Logger.writeLog("In %s:%d, Server Connection gone.", __FILE__, __LINE__);

					serverState = S_SERVER_NORMAL_ERROR;
					return S_READ_ERROR;
				}

				break;
			}else if (readCnt == 0){
				if (g_CP.getIsLogPrint())
						g_Logger.writeLog("In %s:%d, server normal close!", __FILE__, __LINE__);
				
				serverState = S_SERVER_NORMAL_OK;
				break;
			}else{
				if (lseek(serverPktCacheFd, serverPktCacheLen, SEEK_SET) < 0){
					if (g_CP.getIsLogPrint())
						g_Logger.writeLog("In %s:%d, lseek server pkt cache file error!", __FILE__, __LINE__);

					serverState = S_SERVER_NORMAL_ERROR;
					return S_READ_ERROR;
				}
				if( write(serverPktCacheFd, serverRecvBuf, readCnt) != readCnt){
					if (g_CP.getIsLogPrint())
						g_Logger.writeLog("In %s:%d, Recv Server Data Error, Write File Error!", __FILE__, __LINE__);

					serverState = S_SERVER_NORMAL_ERROR;
					return S_READ_ERROR;
				}

				serverPktCacheLen += readCnt;
			}
		}while(true);
	}
	else if (TASK_DATA == taskType){
		do{
			readCnt = read(serverSock, serverRecvBuf, 2048);
			if (readCnt < 0){
				if ((errno != EAGAIN) || (errno != EWOULDBLOCK)){
					if (g_CP.getIsLogPrint())
						g_Logger.writeLog("In %s:%d, Server Connection gone.\n", __FILE__, __LINE__);

					serverState = S_SERVER_DATA_ERROR;
					return S_READ_ERROR;
				}

				break;
			}else if (readCnt == 0){
				printf("In %s:%d, Server Close!\n", __FILE__, __LINE__);

				// Todo: Set state, need to decide server state when is ok!
				serverState = S_SERVER_DATA_ERROR;
				break;
			}else{
				lseek(serverPktCacheFd, serverPktCacheLen, SEEK_SET);
				if( write(serverPktCacheFd, serverRecvBuf, readCnt) != readCnt){
					if (g_CP.getIsLogPrint())
						g_Logger.writeLog("In %s:%d, Recv Server Data Error, Write File Error! Errno=%d, Msg=%s!", __FILE__, __LINE__, errno, strerror(errno));

					serverState = S_SERVER_DATA_ERROR;
					return S_READ_ERROR;
				}

				serverPktCacheLen += readCnt;
			}
		}while(true);

		if (S_CLIENT_DATA_OFFSETREQ_RECV_OK == clientState &&
			S_SERVER_DATA_OFFSETRES_RECV_INDOING == serverState){
			char *p = serverRecvBuf;
			unsigned int pktLen = ntohl(*(unsigned int *)(p + 6));
			unsigned int pbBodyLen = ntohl(*(unsigned int *)(p + 10));

			weiyun::MsgHead stMsgHead;
			if (!stMsgHead.ParseFromArray(p + 16, pktLen - 16 - pbBodyLen)){
				if (g_CP.getIsLogPrint())
					g_Logger.writeLog("In %s:%d, decode server response offset pb head packet error!", __FILE__, __LINE__);

				return S_READ_ERROR;	
			}
			uin = stMsgHead.uin();
			seq = stMsgHead.seq();
			
			weiyun::MsgBody stMsgBody;
			if (!stMsgBody.ParseFromArray(p + pktLen - pbBodyLen, pbBodyLen)){
				if (g_CP.getIsLogPrint())
					g_Logger.writeLog("In %s:%d, decode server response offset pb body packet error!", __FILE__, __LINE__);

				return S_READ_ERROR;
			}
			
			weiyun::RspMsgBody* pstRspMsgBody = stMsgBody.mutable_rspmsg_body();
			weiyun::ClientFileTransQueryMsgRsp* pstMsg = pstRspMsgBody->mutable_clientfiletransquerymsgrsp_body();
			fileOffset = pstMsg->next_offset();

			printf("====================================\n");
			printf("*      Server Offset Response      *\n");
			printf("====================================\n");
			printf("** Flag = %d\n", pstMsg->flag());
			printf("** File_offset = %llu\n", (unsigned long long)pstMsg->next_offset());
			printf("** Process = %d\n", pstMsg->process());
			printf("====================================\n");
			
			serverState = S_SERVER_DATA_OFFSETRES_RECV_OK;
			clientState = S_CLIENT_DATA_OFFSETRES_SEND_INDOING;
		}
		else if (S_CLIENT_DATA_OK == clientState &&
			S_SERVER_DATA_DATARES_RECV_INDOING == serverState){
			//printf("In %s:%d, recv server data response!\n", __FILE__, __LINE__);
			char *p = serverRecvBuf;
			unsigned int pktLen = ntohl(*(unsigned int *)(p + 6));
			unsigned int pbBodyLen = ntohl(*(unsigned int *)(p + 10));

			weiyun::MsgHead stMsgHead;
			if (!stMsgHead.ParseFromArray(p + 16, pktLen - 16 - pbBodyLen)){
				if (g_CP.getIsLogPrint())
					g_Logger.writeLog("In %s:%d, decode server response offset pb head packet error!", __FILE__, __LINE__);

				return S_READ_ERROR;	
			}
			uin = stMsgHead.uin();
			seq = stMsgHead.seq();
			
			weiyun::MsgBody stMsgBody;
			if (!stMsgBody.ParseFromArray(p + pktLen - pbBodyLen, pbBodyLen)){
				if (g_CP.getIsLogPrint())
					g_Logger.writeLog("In %s:%d, decode server response offset pb body packet error!", __FILE__, __LINE__);

				return S_READ_ERROR;
			}
			
			weiyun::RspMsgBody* pstRspMsgBody = stMsgBody.mutable_rspmsg_body();
			weiyun::ClientFileTransQueryMsgRsp* pstMsg = pstRspMsgBody->mutable_clientfiletransquerymsgrsp_body();
			fileOffset = pstMsg->next_offset();

			printf("====================================\n");
			printf("*      Server Final Response       *\n");
			printf("====================================\n");
			printf("* Flag = %d\n", pstMsg->flag());
			printf("* File_offset = %llu\n", (unsigned long long)pstMsg->next_offset());
			printf("* Process = %d\n", pstMsg->process());
			printf("====================================\n");
			
			serverState = S_SERVER_DATA_OK;
		}
		else if (S_SERVER_DATA_DATA_SEND_INDOING == serverState){
			printf("==============================\n");
			printf("Server Response, not normal!\n");
			printf("ClientState = %d, ServerState = %d\n", clientState, serverState);
			char *p = serverRecvBuf;
			unsigned int pktLen = ntohl(*(unsigned int *)(p + 6));
			unsigned int pbBodyLen = ntohl(*(unsigned int *)(p + 10));

			weiyun::MsgHead stMsgHead;
			if (!stMsgHead.ParseFromArray(p + 16, pktLen - 16 - pbBodyLen)){
				if (g_CP.getIsLogPrint())
					g_Logger.writeLog("In %s:%d, decode server response pb head packet error!", __FILE__, __LINE__);

				return S_READ_ERROR;	
			}
			cout<< stMsgHead.Utf8DebugString();
			uin = stMsgHead.uin();
			seq = stMsgHead.seq();
			
			weiyun::MsgBody stMsgBody;
			if (!stMsgBody.ParseFromArray(p + pktLen - pbBodyLen, pbBodyLen)){
				if (g_CP.getIsLogPrint())
					g_Logger.writeLog("In %s:%d, decode server response pb body packet error!", __FILE__, __LINE__);

				return S_READ_ERROR;
			}
			cout<< stMsgBody.Utf8DebugString();
			printf("==============================\n");
			
			weiyun::RspMsgBody* pstRspMsgBody = stMsgBody.mutable_rspmsg_body();
			weiyun::ClientFileTransQueryMsgRsp* pstMsg = pstRspMsgBody->mutable_clientfiletransquerymsgrsp_body();
			fileOffset = pstMsg->next_offset();

			if (1 == pstMsg->flag()){
				serverState = S_SERVER_DATA_OK;
			}else{
				serverState = S_SERVER_DATA_ERROR;
			}

			//printf("====================================\n");
			//printf("*      Server Final Response       *\n");
			//printf("====================================\n");
			//printf("* Flag = %d\n", pstMsg->flag());
			//printf("* File_offset = %llu\n", pstMsg->next_offset());
			//printf("* Process = %d\n", pstMsg->process());
			//printf("====================================\n");			
		}
	}

	//printf("In %s:%d, ServerPktCacheLen = %llu, ClientPktSendLen = %llu\n", __FILE__, __LINE__, serverPktCacheLen, clientPktSendLen);
	return S_READ_OK;
}

int Worker::genClientDataResponse(long long int tUin, int tSeq, int tFlag, unsigned long long int tOffset,
										 int tRetCode, const char * tRetMsg)
{
	int offset;
	
	weiyun::MsgBody stMsgBody;
	weiyun::RspMsgBody* pstRspMsgBody = stMsgBody.mutable_rspmsg_body();
	weiyun::ClientFileTransQueryMsgRsp* pstMsg = pstRspMsgBody->mutable_clientfiletransquerymsgrsp_body();

	pstMsg->set_flag(tFlag);
	pstMsg->set_next_offset(tOffset);

	char * pbBodyBuf = new char[stMsgBody.ByteSize()];
	stMsgBody.SerializeToArray(pbBodyBuf, stMsgBody.ByteSize());

	weiyun::MsgHead stMsgHead;
	stMsgHead.set_uin(tUin);
	stMsgHead.set_seq(tSeq);
	stMsgHead.set_type(weiyun::MsgHead_Type_RspMsg);
	stMsgHead.set_retcode(tRetCode);
	stMsgHead.set_retmsg(tRetMsg);

	char * pbHeadBuf = new char[stMsgHead.ByteSize()];
	stMsgHead.SerializeToArray(pbHeadBuf, stMsgHead.ByteSize());

	// Fill Packet
	char *retMsgPktBuf = new char[16 + stMsgHead.ByteSize() + stMsgBody.ByteSize()];

	// Fill dwMagicNum
	offset = 0;	
	*(unsigned char *)(retMsgPktBuf + offset) = 0x20; 
	*(unsigned char *)(retMsgPktBuf + offset + 1) = 0x13;	
	*(unsigned char *)(retMsgPktBuf + offset + 2) = 0x03; 
	*(unsigned char *)(retMsgPktBuf + offset + 3) = 0x29;

	// Fill wType	
	offset = 4;	
	*(unsigned char *)(retMsgPktBuf + offset) = 0x00; 
	*(unsigned char *)(retMsgPktBuf + offset + 1) = 0x01;

	// Set dwPackLen	
	offset = 6;	
	*(unsigned int *)(retMsgPktBuf + offset) = htonl(16 + stMsgHead.ByteSize() + stMsgBody.ByteSize());

	// Set dwBoldyLen	
	offset = 10;	
	*(unsigned int *)(retMsgPktBuf + offset) = htonl(stMsgBody.ByteSize());

	// Set wReserved	
	offset = 14;	
	*(unsigned char *)(retMsgPktBuf + offset) = 0x00; 
	*(unsigned char *)(retMsgPktBuf + offset + 1) = 0x00;

	// Set PBHead	
	offset = 16;	
	memcpy(retMsgPktBuf + offset, pbHeadBuf, stMsgHead.ByteSize());

	// Set PBBody
	offset += stMsgHead.ByteSize();	
	memcpy(retMsgPktBuf + offset, pbBodyBuf, stMsgBody.ByteSize());

	offset += stMsgBody.ByteSize();

	lseek(serverPktCacheFd, serverPktCacheLen, SEEK_SET);
	if( write(serverPktCacheFd, retMsgPktBuf, offset) != offset){
		if (g_CP.getIsLogPrint())
			g_Logger.writeLog("In %s:%d, Generate client data response error, Write File Error!", __FILE__, __LINE__);

		printf("In %s:%d, write file error!\n", __FILE__, __LINE__);

		delete [] pbBodyBuf;
		delete [] pbHeadBuf;
		delete [] retMsgPktBuf;
		return S_READ_ERROR;
	}

	serverPktCacheLen += offset;
	//printf("In %s:%d, Generate client data response ok! Response PKT Len = %d\n", __FILE__, __LINE__, offset);

	//printf("In %s:%d, serverPktCacheLen = %llu, clientPktSendLen = %llu\n", __FILE__, __LINE__, serverPktCacheLen, clientPktSendLen);

//	for (int i = 0; i < offset; i++){
//		printf("%.2x", *(unsigned char *)(retMsgPktBuf + i));
//	}
//	printf("\n");

	delete [] pbBodyBuf;
	delete [] pbHeadBuf;
	delete [] retMsgPktBuf;

	return S_READ_OK;
}

int Worker::getTaskType()
{
	return taskType;
}

void Worker::setTaskType(int tTaskType)
{
	taskType = tTaskType;
}

void Worker::setFileRecvLen(unsigned long long int tFileRecvLen)
{
	fileRecvLen = tFileRecvLen;
}

void Worker::setFileSendLen(unsigned long long int tFileSendLen)
{
	fileSendLen = tFileSendLen;
}

int Worker::readDataClientData()
{
	int readCnt, writeRetCnt;

	while(true){
		readCnt = read(clientSock, clientRecvBuf, 102400);
		if (readCnt < 0){
			if ((errno != EAGAIN) || (errno != EWOULDBLOCK)){
				if (g_CP.getIsLogPrint())
					g_Logger.writeLog("In %s:%d, Client Connection Error!", __FILE__, __LINE__);

				return S_READ_ERROR;
			}

			break;
		}else if (readCnt == 0){
			if (g_CP.getIsLogPrint())
				g_Logger.writeLog("In %s:%d, Client Connection gone. Close by client!", __FILE__, __LINE__);

			return S_READ_ERROR;
		}else{
			if ( (fileRecvLen/FILESLICELEN) != (unsigned long long) clientFileCacheCurrentWriteIndex){
				if (g_CP.getIsLogPrint())
					g_Logger.writeLog("In %s:%d, Client File Cache Index error!", __FILE__, __LINE__);

				return S_READ_ERROR;
			}

			clientFileCacheCurrentWriteOffset = fileRecvLen % FILESLICELEN;

			if ( ((unsigned)readCnt) < (FILESLICELEN - clientFileCacheCurrentWriteOffset) ){
				// write data to file
				lseek(clientFileCacheCurrentWriteFd, clientFileCacheCurrentWriteOffset, SEEK_SET);
				writeRetCnt = write(clientFileCacheCurrentWriteFd, clientRecvBuf, readCnt);
				if (readCnt != writeRetCnt){
					if (g_CP.getIsLogPrint())
						g_Logger.writeLog("In %s:%d, write data to client cache file error!", __FILE__, __LINE__);

					close(clientFileCacheCurrentWriteFd);
					clientFileCacheCurrentWriteFd = -1;
					return S_READ_ERROR;
				}

				fileRecvLen += writeRetCnt;
				//printf("In %s:%d, FileRecvLen = %llu, FileSize = %llu, FileOffset = %llu\n", __FILE__, __LINE__, fileRecvLen, fileSize, fileOffset);
				if (fileRecvLen == (fileSize - fileOffset)){
					close(clientFileCacheCurrentWriteFd);
					clientFileCacheCurrentWriteFd = -1;

					printf("====================================\n");
					printf("*    Client File Recv Complete     *\n");
					printf("====================================\n");
					printf("* File Size = %llu\n", fileSize);
					printf("* File Offset = %llu\n", fileOffset);
					printf("* File Recv Len = %llu\n", fileRecvLen);
					printf("* Uin = %lld\n", uin);
					printf("* Seq = %d\n", seq);
					printf("====================================\n");
										
					if (S_READ_OK == genClientDataResponse(uin, seq, 1, fileSize, 0, "")){
						clientState = S_CLIENT_DATA_DATARES_SEND_INDOING;
						return S_READ_OK;
					}else{
						return S_READ_ERROR;
					}

					return S_READ_OK;
				}
			}
			else if ( (unsigned)readCnt == (FILESLICELEN - clientFileCacheCurrentWriteOffset) ){
				// write data to file
				lseek(clientFileCacheCurrentWriteFd, clientFileCacheCurrentWriteOffset, SEEK_SET);
				writeRetCnt = write(clientFileCacheCurrentWriteFd, clientRecvBuf, readCnt);
				if (readCnt != writeRetCnt){
					if (g_CP.getIsLogPrint())
						g_Logger.writeLog("In %s:%d, write data to client cache file error!", __FILE__, __LINE__);

					close(clientFileCacheCurrentWriteFd);
					clientFileCacheCurrentWriteFd = -1;
					return S_READ_ERROR;
				}

				fileRecvLen += writeRetCnt;
				//printf("In %s:%d, FileRecvLen = %llu, FileSize = %llu, FileOffset = %llu\n", __FILE__, __LINE__, fileRecvLen, fileSize, fileOffset);
				if (fileRecvLen == (fileSize - fileOffset)){
					close(clientFileCacheCurrentWriteFd);
					clientFileCacheCurrentWriteFd = -1;

					printf("====================================\n");
					printf("*    Client File Recv Complete     *\n");
					printf("====================================\n");
					printf("* File Size = %llu\n", fileSize);
					printf("* File Offset = %llu\n", fileOffset);
					printf("* File Recv Len = %llu\n", fileRecvLen);
					printf("* Uin = %lld\n", uin);
					printf("* Seq = %d\n", seq);
					printf("====================================\n");
										
					if (S_READ_OK == genClientDataResponse(uin, seq, 1, fileSize, 0, "")){
						clientState = S_CLIENT_DATA_DATARES_SEND_INDOING;
						return S_READ_OK;
					}else{
						return S_READ_ERROR;
					}
				}
				else{
					close(clientFileCacheCurrentWriteFd);
					clientFileCacheCurrentWriteIndex++;
					initClientFileCacheWrite(clientFileCacheCurrentWriteIndex);
				}
			}
			else{
				// write data to current file
				lseek(clientFileCacheCurrentWriteFd, clientFileCacheCurrentWriteOffset, SEEK_SET);
				writeRetCnt = write(clientFileCacheCurrentWriteFd, clientRecvBuf, FILESLICELEN - clientFileCacheCurrentWriteOffset);
				if ( (unsigned)writeRetCnt != (FILESLICELEN - clientFileCacheCurrentWriteOffset)){
					if (g_CP.getIsLogPrint())
						g_Logger.writeLog("In %s:%d, write data to client cache file error!", __FILE__, __LINE__);

					close(clientFileCacheCurrentWriteFd);
					clientFileCacheCurrentWriteFd = -1;
					return S_READ_ERROR;
				}

				fileRecvLen += writeRetCnt;
				//printf("In %s:%d, FileRecvLen = %llu, FileSize = %llu, FileOffset = %llu\n", __FILE__, __LINE__, fileRecvLen, fileSize, fileOffset);

				close(clientFileCacheCurrentWriteFd);
				clientFileCacheCurrentWriteIndex++;
				initClientFileCacheWrite(clientFileCacheCurrentWriteIndex);

				// write data to next file
				clientFileCacheCurrentWriteOffset = 0;
				lseek(clientFileCacheCurrentWriteFd, clientFileCacheCurrentWriteOffset, SEEK_SET);
				int writeRetCnt_2 = write(clientFileCacheCurrentWriteFd, clientRecvBuf + writeRetCnt, readCnt - writeRetCnt);
				if (writeRetCnt_2 != (readCnt - writeRetCnt)){
					if (g_CP.getIsLogPrint())
						g_Logger.writeLog("In %s:%d, write data to client cache file error!", __FILE__, __LINE__);

					close(clientFileCacheCurrentWriteFd);
					clientFileCacheCurrentWriteFd = -1;
					return S_READ_ERROR;
				}

				fileRecvLen += writeRetCnt_2;
				//printf("In %s:%d, FileRecvLen = %llu, FileSize = %llu, FileOffset = %llu\n", __FILE__, __LINE__, fileRecvLen, fileSize, fileOffset);
				if (fileRecvLen == (fileSize - fileOffset)){
					close(clientFileCacheCurrentWriteFd);
					clientFileCacheCurrentWriteFd = -1;

					printf("====================================\n");
					printf("*    Client File Recv Complete     *\n");
					printf("====================================\n");
					printf("** File Size = %llu\n", fileSize);
					printf("** File Offset = %llu\n", fileOffset);
					printf("** File Recv Len = %llu\n", fileRecvLen);
					printf("** Uin = %lld\n", uin);
					printf("** Seq = %d\n", seq);
					printf("====================================\n");
										
					if (S_READ_OK == genClientDataResponse(uin, seq, 1, fileSize, 0, "")){
						clientState = S_CLIENT_DATA_DATARES_SEND_INDOING;
						return S_READ_OK;
					}else{
						return S_READ_ERROR;
					}
				}
			}		
		}
	}

	//printf("In %s:%d, write data to client cache, clientPktCacheLen = %llu\n", __FILE__, __LINE__, clientPktCacheLen);
	return S_READ_INCOMPLETE;
}

int Worker::readNormalClientData()
{
	int readCnt;
	char *p = NULL;

	while(true){
		p = clientConnBuf;

		readCnt = read(clientSock, p + clientConnBufLen, 4096);
		if (readCnt < 0){
			if ((errno != EAGAIN) || (errno != EWOULDBLOCK)){
				if (g_CP.getIsLogPrint())
					g_Logger.writeLog("In %s:%d, Client Normal Connection Error!", __FILE__, __LINE__);

				return S_READ_ERROR;
			}

			break;
		}else if (readCnt == 0){
			if (g_CP.getIsLogPrint())
				g_Logger.writeLog("In %s:%d, Client Normal Connection gone. Close by client!", __FILE__, __LINE__);

			clientState = S_CLIENT_NORMAL_OK;
			return S_READ_OK;
		}

		clientConnBufLen += readCnt;

		if (clientConnBufLen >= 4){
			if ((unsigned char)clientConnBuf[0] != 0x20 || (unsigned char)clientConnBuf[1] != 0x13 ||
				(unsigned char)clientConnBuf[2] != 0x03 || (unsigned char)clientConnBuf[3] != 0x29){
				if (g_CP.getIsLogPrint())
					g_Logger.writeLog("In %s:%d, Client send packet do not start with 0x20 13 03 29!", __FILE__, __LINE__);

				return S_READ_ERROR;
			}
		}

		if (clientConnBufLen >= 16){
			// decode pkt
			if (decodeClientPktAndWriteCache(p, clientConnBufLen) < 0){
				if (g_CP.getIsLogPrint())
					g_Logger.writeLog("In %s:%d, Client normal pkt decode error!", __FILE__, __LINE__);

				return S_READ_ERROR;				
			}

			//printf("In %s:%d, p = %p, client conn buffer len = %d, buffer address = %p\n", __FILE__, __LINE__, p, clientConnBufLen, &clientConnBuf[0]);
		}		

		// move to the head
		if (p != &clientConnBuf[0] && clientConnBufLen > 0){
			memcpy(clientConnBuf, p, clientConnBufLen);
		}
	}	

	return S_READ_OK;
}

int Worker::realSendDataToServer()
{
	int sendLen;
	int retWDVal, retRDVal;
	if (clientPktCacheLen <= serverPktSendLen)
		return S_SEND_OK;

	sendLen = ((clientPktCacheLen - serverPktSendLen) >= 102400)? 102400:(clientPktCacheLen - serverPktSendLen);
	retRDVal = read(clientPktCacheReadFd, serverSendBuf, sendLen);
	if (retRDVal > 0){
		retWDVal = write(serverSock, serverSendBuf, retRDVal);

		if (retWDVal <= 0){
			if (g_CP.getIsLogPrint())
				g_Logger.writeLog("In %s:%d, send data to server error!\n", __FILE__, __LINE__);

			return -1;
		}

		serverPktSendLen += retWDVal;
		printf("In %s:%d, clientPktCacheLen = %llu, ServerPktSendLen = %llu\n", __FILE__, __LINE__, clientPktCacheLen, serverPktSendLen);
		return 0;
	}

	return -1;
}

void Worker::setClientFileCacheWriteIndex(int tWriteIndex)
{
	clientFileCacheCurrentWriteIndex = tWriteIndex;
}

void Worker::setClientFileCacheReadIndex(int tReadIndex)
{
	clientFileCacheCurrentReadIndex = tReadIndex;
}

int Worker::initClientFileCacheWrite(int tWriteIndex)
{
	char clientFileCacheWritePath[256] = {0};
	sprintf(clientFileCacheWritePath, "%s%s%011llu", cacheHome, "data_", fileOffset + tWriteIndex * FILESLICELEN);
	
	if( (clientFileCacheCurrentWriteFd = open(clientFileCacheWritePath, O_CREAT|O_WRONLY, 0666)) == -1 ){
		if (g_CP.getIsLogPrint())
			g_Logger.writeLog("%s:%d, Error creating client file cache!", __FILE__, __LINE__);

		return -1;
	}

	return 0;
}

int Worker::initClientFileCacheRead(int tReadIndex)
{
	char clientFileCacheReadPath[256] = {0};
	sprintf(clientFileCacheReadPath, "%s%s%011llu", cacheHome, "data_", fileOffset + tReadIndex * FILESLICELEN);

	if( (clientFileCacheCurrentReadFd = open(clientFileCacheReadPath, O_RDONLY, 0666)) == -1 ){
		if (g_CP.getIsLogPrint())
			g_Logger.writeLog("%s:%d, Error opening client pkt cache file read fd!", __FILE__, __LINE__);

		return -1;
	}

	return 0;
}

int Worker::deleteFileSliceByReadIndex(int tReadIndex)
{
	char clientFileCacheReadPath[256] = {0};
	sprintf(clientFileCacheReadPath, "%s%s%011llu", cacheHome, "data_", fileOffset + tReadIndex * FILESLICELEN);

	if(is_file(clientFileCacheReadPath)){
		remove(clientFileCacheReadPath);
		//printf("In %s:%d, remove file %s ok!\n", __FILE__, __LINE__, clientFileCacheReadPath);
	}else{
		//printf("In %s:%d, remove file %s error!\n", __FILE__, __LINE__, clientFileCacheReadPath);
		return -1;
	}

	return 0;
}

int Worker::sendDataToClient()
{
	int sendLen;
	int retWDVal, retRDVal;
	if (serverPktCacheLen <= clientPktSendLen)
		return S_SEND_OK;

	sendLen = ((serverPktCacheLen - clientPktSendLen) >= 2048)? 2048:(serverPktCacheLen - clientPktSendLen);
	if (lseek(serverPktCacheReadFd, clientPktSendLen, SEEK_SET) < 0){
		if (g_CP.getIsLogPrint())
			g_Logger.writeLog("In %s:%d, lseek server pkt cache file error!", __FILE__, __LINE__);

		return S_SEND_ERROR;
	}	
	retRDVal = read(serverPktCacheReadFd, clientSendBuf, sendLen);
	if (retRDVal > 0){
		retWDVal = write(clientSock, clientSendBuf, retRDVal);

		clientPktSendLen += retWDVal;
		//printf("In %s:%d, ServerPKTCacheLen = %llu, ClientPKtSendLen = %llu\n", __FILE__, __LINE__, serverPktCacheLen, clientPktSendLen);
		return S_SEND_OK;
	}else{
		if (g_CP.getIsLogPrint())
			g_Logger.writeLog("In %s:%d, send server pkt cache file data to client error!", __FILE__, __LINE__);

		return S_SEND_ERROR;
	}

	return S_SEND_INCOMPLETE;	
}

int Worker::sendDataToServer()
{
	int sendLen;
	int retWDVal, retRDVal;

	if (TASK_NORMAL == taskType){
		if (clientPktCacheLen <= serverPktSendLen)
			return S_SEND_OK;

		sendLen = ((clientPktCacheLen - serverPktSendLen) >= 102400)? 102400:(clientPktCacheLen - serverPktSendLen);
		if (lseek(clientPktCacheReadFd, serverPktSendLen, SEEK_SET) < 0){
			if (g_CP.getIsLogPrint())
				g_Logger.writeLog("In %s:%d, lseek client pkt cache file error!", __FILE__, __LINE__);

			return S_SEND_ERROR;
		}

		retRDVal = read(clientPktCacheReadFd, serverSendBuf, sendLen);
		if (retRDVal > 0){
			retWDVal = write(serverSock, serverSendBuf, retRDVal);

			if (retWDVal <= 0){
				if (g_CP.getIsLogPrint())
					g_Logger.writeLog("In %s:%d, send data to server error!", __FILE__, __LINE__);

				return S_SEND_ERROR;
			}

			serverPktSendLen += retWDVal;
		}else{
			if (g_CP.getIsLogPrint())
				g_Logger.writeLog("In %s:%d, read client pkt cache file error!", __FILE__, __LINE__);

			return S_SEND_ERROR;
		}
	}
	else if (TASK_DATA == taskType){
		if (S_CLIENT_DATA_OFFSETREQ_RECV_OK == clientState && S_SERVER_DATA_OFFSETREQ_SEND_INDOING == serverState){
			if (clientPktCacheLen <= serverPktSendLen)
				return S_SEND_OK;

			sendLen = ((clientPktCacheLen - serverPktSendLen) >= 102400)? 102400:(clientPktCacheLen - serverPktSendLen);
			if (lseek(clientPktCacheReadFd, serverPktSendLen, SEEK_SET) < 0){
				if (g_CP.getIsLogPrint())
					g_Logger.writeLog("In %s:%d, lseek client pkt cache file error!", __FILE__, __LINE__);

				return S_SEND_ERROR;
			}

			retRDVal = read(clientPktCacheReadFd, serverSendBuf, sendLen);
			if (retRDVal > 0){
				retWDVal = write(serverSock, serverSendBuf, retRDVal);

				if (retWDVal <= 0){
					if (g_CP.getIsLogPrint())
						g_Logger.writeLog("In %s:%d, send data to server error!", __FILE__, __LINE__);

					return S_SEND_ERROR;
				}

				serverPktSendLen += retWDVal;
				//printf("In %s:%d, ClientPktCacheLen = %llu, ServerPktSendLen = %llu\n", __FILE__, __LINE__, clientPktCacheLen, serverPktSendLen);
			}else{
				if (g_CP.getIsLogPrint())
					g_Logger.writeLog("In %s:%d, read client pkt cache file error!", __FILE__, __LINE__);

				return S_SEND_ERROR;
			}
		}
		else if (S_SERVER_DATA_DATA_SEND_INDOING == serverState){
			if (fileRecvLen <= fileSendLen)
				return S_SEND_OK;

			//printf("fileSendLen = %llu, currentfilereadindex = %d\n", fileSendLen, clientFileCacheCurrentReadIndex);
			if ((fileSendLen/FILESLICELEN) != (unsigned long long)clientFileCacheCurrentReadIndex){
				if (g_CP.getIsLogPrint())
					g_Logger.writeLog("In %s:%d, read client cache file error!", __FILE__, __LINE__);

				close(clientFileCacheCurrentReadFd);
				clientFileCacheCurrentReadFd = -1;
				return S_SEND_ERROR;
			}

			clientFileCacheCurrentReadOffset = fileSendLen % FILESLICELEN;

			unsigned long long maxDataLeftLen = FILESLICELEN - clientFileCacheCurrentReadOffset;
			unsigned long long maxDataSendLen = fileRecvLen - fileSendLen;
			unsigned long long maxDataCanSendLen = (maxDataSendLen > maxDataLeftLen)? maxDataLeftLen:maxDataSendLen;

			sendLen = (maxDataCanSendLen >= 102400)? 102400:maxDataCanSendLen;
			if (lseek(clientFileCacheCurrentReadFd, clientFileCacheCurrentReadOffset, SEEK_SET) < 0){
				if (g_CP.getIsLogPrint())
					g_Logger.writeLog("In %s:%d, lseek client cache file error!", __FILE__, __LINE__);

				close(clientFileCacheCurrentReadFd);
				clientFileCacheCurrentReadFd = -1;
				return S_SEND_ERROR;
			}

			retRDVal = read(clientFileCacheCurrentReadFd, serverSendBuf, sendLen);
			if (retRDVal > 0){
				retWDVal = write(serverSock, serverSendBuf, retRDVal);

				if (retWDVal <= 0){
					if (g_CP.getIsLogPrint())
						g_Logger.writeLog("In %s:%d, send data to server error!", __FILE__, __LINE__);

					close(clientFileCacheCurrentReadFd);
					clientFileCacheCurrentReadFd = -1;
					return S_SEND_ERROR;
				}

				fileSendLen += retWDVal;
				//printf("In %s:%d, FileSendLen = %llu, FileSize = %llu, FileOffset = %llu\n", __FILE__, __LINE__, fileSendLen, fileSize, fileOffset);
			}else{
				if (g_CP.getIsLogPrint())
					g_Logger.writeLog("In %s:%d, read client cache file error!", __FILE__, __LINE__);

				close(clientFileCacheCurrentReadFd);
				clientFileCacheCurrentReadFd = -1;
				return S_SEND_ERROR;
			}

			// if complete
			if (fileSendLen == (fileSize - fileOffset)){
				printf("====================================\n");
				printf("*     Send Data To Server OK       *\n");
				printf("====================================\n");
				serverState = S_SERVER_DATA_DATARES_RECV_INDOING;

				close(clientFileCacheCurrentReadFd);
				clientFileCacheCurrentReadFd = -1;
				return S_SEND_OK;
			}
			else if ((fileSendLen % FILESLICELEN) == 0){
				close(clientFileCacheCurrentReadFd);
				deleteFileSliceByReadIndex(clientFileCacheCurrentReadIndex);

				clientFileCacheCurrentReadIndex++;
				initClientFileCacheRead(clientFileCacheCurrentReadIndex);			
			}else{
				// clientFileCacheCurrentReadIndex not change
			}
		}
	}

	return S_SEND_OK;	
}

int Worker::readFirstConnData()
{
	char *p = NULL;
	int readCnt = 0;
	
	while(true){
		p = clientConnBuf;
		readCnt = read(clientSock, p + clientConnBufLen, 4096);
		if (readCnt < 0){
			if ((errno != EAGAIN) || (errno != EWOULDBLOCK)){
				if (g_CP.getIsLogPrint())
					g_Logger.writeLog("In %s:%d, Client Connection Error!", __FILE__, __LINE__);

				return S_READ_ERROR;
			}

			break;
		}else if (readCnt == 0){
			if (g_CP.getIsLogPrint())
				g_Logger.writeLog("In %s:%d, Client Connection gone. Close by client!", __FILE__, __LINE__);

			return S_READ_ERROR;
		}

		clientConnBufLen += readCnt;
		if (clientConnBufLen >= 4){	// MagicNum is 0x20130329
			if ((unsigned char)clientConnBuf[0] != 0x20 || (unsigned char)clientConnBuf[1] != 0x13 ||
				(unsigned char)clientConnBuf[2] != 0x03 || (unsigned char)clientConnBuf[3] != 0x29){
				if (g_CP.getIsLogPrint())
					g_Logger.writeLog("In %s:%d, Client send first packet do not start with 0x20 13 03 29!", __FILE__, __LINE__);

				return S_READ_ERROR;
			}
		}

#if 0
		//Hexdump
		printf("==========Hex Dump===============\n");
		for(int j = 0; j < clientConnBufLen; j++){
			printf("%.2X ", (unsigned char)clientConnBuf[j]);

			if((j+1)%16 == 0)
				printf("\n");
		}
		printf("\n");
		printf("==========Hex Dump===============\n");
#endif

		if (clientConnBufLen >= 16){
			unsigned int pktLen = ntohl(*(unsigned int *)(p + 6));
			unsigned int pbBodyLen = ntohl(*(unsigned int *)(p + 10));

			// first packet decode
			if ((unsigned int)clientConnBufLen >= pktLen && isFirstDecode == true){
				isFirstDecode = false;
				startState = S_CLIENT_CONNECT_OK;

				weiyun::MsgHead stMsgHead;
				if (!stMsgHead.ParseFromArray(p + 16, pktLen - 16 - pbBodyLen)){
					if (g_CP.getIsLogPrint())
						g_Logger.writeLog("In %s:%d, decode client first pb head packet error!", __FILE__, __LINE__);

					return S_READ_ERROR;	
				}

				switch (stMsgHead.cmd()) {
					case weiyun::ClientFileTransQueryMsg:
					{
						weiyun::MsgBody stMsgBody;
						if (!stMsgBody.ParseFromArray(p + pktLen - pbBodyLen, pbBodyLen)){
							if (g_CP.getIsLogPrint())
								g_Logger.writeLog("In %s:%d, decode client offset request pb body packet error!", __FILE__, __LINE__);

							return S_READ_ERROR;
						}
						weiyun::ReqMsgBody* pstReqMsgBody = stMsgBody.mutable_reqmsg_body();
						weiyun::ClientFileTransQueryMsgReq* pstMsg = pstReqMsgBody->mutable_clientfiletransquerymsgreq_body();
						fileSize = pstMsg->file_size();
						taskType = TASK_DATA;
						((Worker *)pSlave)->setTaskType(taskType);
						clientState = S_CLIENT_DATA_OFFSETREQ_RECV_OK;
						serverState = S_SERVER_DATA_OFFSETREQ_SEND_INDOING;
						
						printf("====================================\n");
						printf("*        Data Client Connect       *\n");
						printf("====================================\n");
						printf("** Decode File Size = %llu\n", fileSize);
						printf("** file_size = %llu\n", (unsigned long long) pstMsg->file_size());
						printf("** ukey.size()= %zu\n", pstMsg->ukey().size());
						printf("** file_key.size() = %zu\n", pstMsg->file_key().size());
						printf("** upload_domain = %s\n", (pstMsg->upload_domain()).c_str());
						printf("** file_name = %s\n", (pstMsg->file_name()).c_str());
						printf("** file_md5 size = %zu\n", pstMsg->file_md5().size());
						printf("====================================\n");
						break;
					}

					default:
					{
						//if(g_CP.getIsLogPrint())
						//	g_Logger.writeLog("In %s:%d, Normal Client Connect!", __FILE__, __LINE__);
						printf("====================================\n");
						printf("*       Normal Client Connect      *\n");
						printf("====================================\n");
						taskType = TASK_NORMAL;
						((Worker *)pSlave)->setTaskType(taskType);
						clientState = S_CLIENT_NORMAL_WORKING;
						serverState = S_SERVER_NORMAL_WORKING;
						break;
					}
				}
			}

			// decode pkt and write to cache file
			if (decodeClientPktAndWriteCache(p, clientConnBufLen) < 0){
				if (g_CP.getIsLogPrint())
					g_Logger.writeLog("In %s:%d, Client pkt decode error!", __FILE__, __LINE__);

				return S_READ_ERROR;				
			}
		}		

		// move to the clientConnBuf head
		if (p != &clientConnBuf[0] && clientConnBufLen > 0){
			memcpy(clientConnBuf, p, clientConnBufLen);
		}
	}

	if (isFirstDecode)
		return S_READ_INCOMPLETE;
	else
		return S_READ_OK;
}

int Worker::getServerSock()
{
	return serverSock;
}

int Worker::getClientSock()
{
	return clientSock;
}

void Worker::setIsSlave(bool tIsSlave)
{
	isSlave = tIsSlave;
}

bool Worker::getIsSlave()
{
	return isSlave;
}

void Worker::setMasterPtr(void *tPtr)
{
	pMaster = tPtr;
}

int Worker::getClientState()
{
	return clientState;
}

int Worker::getServerState()
{
	return serverState;
}

void * Worker::getMasterPtr()
{
	return pMaster;
}

void * Worker::getSlavePtr()
{
	return pSlave;
}

void Worker::setSlavePtr(void *tPtr)
{
	pSlave = tPtr;
}

int Worker::decodeClientPktAndWriteCache(char * &ptr, int &pktLeftLen)
{
	int retCnt;

	if (pktLeftLen <= 16)
		return 0;

	unsigned int pktLen = ntohl(*(unsigned int *)(ptr + 6));
	while ((unsigned int)pktLeftLen >= pktLen){
		if (*(unsigned char *)(ptr + 0) != 0x20 || *(unsigned char *)(ptr + 1) != 0x13 ||
			*(unsigned char *)(ptr + 2) != 0x03 || *(unsigned char *)(ptr + 3) != 0x29){
			if (g_CP.getIsLogPrint())
				g_Logger.writeLog("In %s:%d, not a valid packet! do not start with 0x20 13 03 29!", __FILE__, __LINE__);

			return -1;
		}

		//set hiwifi router flag 0xffff
		*(unsigned char *)(ptr + 14) = 0xff;	*(unsigned char *)(ptr + 15) = 0xff;
		//write to file
		lseek(clientPktCacheFd, clientPktCacheLen, SEEK_SET);
		retCnt = write(clientPktCacheFd, ptr, pktLen);
		if ((unsigned int)retCnt != pktLen){
			if (g_CP.getIsLogPrint())
				g_Logger.writeLog("In %s:%d, write pkt to client cache file error!", __FILE__, __LINE__);

			return -1;
		}
		clientPktCacheLen += pktLen;

		// reset ptr and pktLeftLen
		ptr += pktLen;
		pktLeftLen -= pktLen;

		if (pktLeftLen <= 16)	break;

		pktLen = ntohl(*(unsigned int *)(ptr + 6));
	}

	return 0;
}

int Worker::initTask()
{
	if (initCacheHome() < 0){
		if (g_CP.getIsLogPrint())
			g_Logger.writeLog("In %s:%d, create dir error!", __FILE__, __LINE__);

		return -1;
	}

	if (initServerSock() < 0){
		if (g_CP.getIsLogPrint())
			g_Logger.writeLog("In %s:%d, create server socket error!", __FILE__, __LINE__);

		return -1;
	}


	if (initPktCache() < 0){
		if (g_CP.getIsLogPrint())
			g_Logger.writeLog("In %s:%d, create socket cache file error!", __FILE__, __LINE__);

		return -1;
	}

	return 0;
}

int Worker::initCacheHome()
{
	int folderValue = rand();
	sprintf(cacheHome, "%s%d%s", g_CP.getCacheRoot(), folderValue, "/");
	
	if (CreateDir(cacheHome) < 0){
		if (g_CP.getIsLogPrint())
			g_Logger.writeLog("In %s:%d, create task cache home error!", __FILE__, __LINE__);

		return -1;
	}

	return 0;
}

int Worker::initServerSock()
{
	if (tcp_connect(serverIP, serverPort, serverSock) < 0){
		if (g_CP.getIsLogPrint())
			g_Logger.writeLog("In %s:%d, create server socket error!", __FILE__, __LINE__);

		return -1;
	}

	if (setNonblock(serverSock) < 0){
		if (g_CP.getIsLogPrint())
			g_Logger.writeLog("In %s:%d, can not set server socket to nonblock!", __FILE__, __LINE__);

		return -1;
	}

	return 0;
}

int Worker::initPktCache()
{
	char filePath[300];

	// init client pkt cache
	memset(filePath, 0, sizeof(filePath));
	sprintf(filePath, "%s%s", cacheHome, "ClientPktCache");

	if( (clientPktCacheFd = open(filePath, O_CREAT|O_WRONLY, 0666)) == -1 ){
		if (g_CP.getIsLogPrint())
			g_Logger.writeLog("%s:%d, Error creating client pkt cache file!", __FILE__, __LINE__);

		return -1;
	}

	if( (clientPktCacheReadFd = open(filePath, O_RDONLY, 0666)) == -1 ){
		if (g_CP.getIsLogPrint())
			g_Logger.writeLog("%s:%d, Error opening client pkt cache file read fd!", __FILE__, __LINE__);

		return -1;
	}

	// init server pkt cache
	memset(filePath, 0, sizeof(filePath));
	sprintf(filePath, "%s%s", cacheHome, "ServerPktCache");

	if( (serverPktCacheFd = open(filePath, O_CREAT|O_WRONLY, 0666)) == -1 ){
		if (g_CP.getIsLogPrint())
			g_Logger.writeLog("%s:%d, Error creating server pkt cache file!", __FILE__, __LINE__);

		return -1;
	}

	if( (serverPktCacheReadFd = open(filePath, O_RDONLY, 0666)) == -1 ){
		if (g_CP.getIsLogPrint())
			g_Logger.writeLog("%s:%d, Error opening server pkt cache file read fd!", __FILE__, __LINE__);

		return -1;
	}
	
	return 0;
} 

void Worker::clearCacheHome()
{
	deleteFileByFolder(cacheHome);
}
