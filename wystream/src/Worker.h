#ifndef HEADER_WORKER_H
#define HEADER_WORKER_H

#include <time.h>

#define S_CLIENT_CONNECT_ERROR			-1	//表示客户端初始连接失败
#define S_CLIENT_CONNECT_CONNECTING		0	//表示客户端刚刚完成三步握手，未决定状态
#define	S_CLIENT_CONNECT_RECEIVING		1	//表示客户端正在发送第一次请求数据
#define S_CLIENT_CONNECT_OK				2	//表示客户端第一次请求成功，能够定位出客户端类型

#define S_CLIENT_UNDEFINE				3	//初始化
#define S_SERVER_UNDEFINE				4	//初始化

#define S_CLIENT_NORMAL_WORKING			5	//表示客户端为普通连接，连接进行中
#define S_CLIENT_NORMAL_OK				6	//表示客户端正常关闭
#define S_CLIENT_NORMAL_ERROR			7	//表示客户端异常

#define S_SERVER_NORMAL_WORKING			8	//表示服务器为普通连接，连接进行中
#define S_SERVER_NORMAL_OK				9	//表示服务器为普通连接，连接成功结束
#define S_SERVER_NORMAL_ERROR			10	//表示服务器为普通连接，连接失败结束

#define S_CLIENT_DATA_OFFSETREQ_RECV_OK			11	//表示客户端为数据连接，接收Offset请求成功
#define S_CLIENT_DATA_OFFSETRES_SEND_INDOING	12
#define S_CLIENT_DATA_DATA_RECV_INDOING			13
#define S_CLIENT_DATA_DATA_RECV_OK				14
#define S_CLIENT_DATA_DATARES_SEND_INDOING		15

#define S_SERVER_DATA_OFFSETREQ_SEND_INDOING	17	//表示服务器为数据连接，正在发送Offset请求
#define S_SERVER_DATA_OFFSETRES_RECV_INDOING	18	//表示服务器为数据连接，正在接收Offset请求
#define S_SERVER_DATA_OFFSETRES_RECV_OK			19
#define S_SERVER_DATA_DATA_SEND_INDOING			20
#define S_SERVER_DATA_DATARES_RECV_INDOING		21
#define S_SERVER_DATA_DATARES_RECV_OK			22

#define S_CLIENT_DATA_ERROR						23
#define S_CLIENT_DATA_OK						24
#define S_SERVER_DATA_ERROR						25	//表示服务器为数据连接，发送数据失败
#define S_SERVER_DATA_OK						26

#define TASK_NORMAL						0	//表示普通任务，包含信令交互及其它
#define TASK_DATA						1	//表示数据任务
#define TASK_UNDEFINE					2	//表示任务未定义

#define S_READ_ERROR					-1
#define S_READ_OK						0
#define S_READ_INCOMPLETE				1

#define S_SEND_ERROR					-1
#define S_SEND_OK						0
#define S_SEND_INCOMPLETE				1

class Worker{
	public:
		Worker();
		~Worker();

	public:
		void closeClientSock();
		void closeServerSock();
		void setClientSock(int tSock);
		void setServerIP(char *tServIP);
		void setServerPort(unsigned short int tServPort);
		void setStartState(int tStartState);
		int  getStartState();
		int  readFirstConnData();
		int  initTask();
		int  initCacheHome();
		int  initServerSock();
		int  initPktCache();
		void closeClientPktCacheFd();
		void closeClientPktCacheReadFd();
		void closeServerPktCacheFd();
		void closeServerPktCacheReadFd();
		void clearCacheHome();
		int  decodeClientPktAndWriteCache(char * &ptr, int &pktLeftLen);
		int  getServerSock();
		int  getClientSock();
		void setIsSlave(bool tIsSlave);
		bool getIsSlave();
		void setSlavePtr(void *tPtr);
		void * getSlavePtr();
		void setMasterPtr(void *tPtr);
		void * getMasterPtr();
		int  getClientState();
		int  getServerState();
		bool isNeedToSendToServer();
		bool isNeedToSendToClient();
		int  sendDataToServer();
		int  sendDataToClient();
		int  realSendDataToServer();
		void setClientState(int tClientState);
		void setServerState(int tServerState);
		int  readServerRes();
		int  readNormalClientData();
		int  getTaskType();
		void setTaskType(int tTaskType);
		int  readDataClientData();
		void setFileRecvLen(unsigned long long int tFileRecvLen);
		void setFileSendLen(unsigned long long int tFileSendLen);
		int  genClientDataResponse(long long int tUin, int tSeq, int tFlag, unsigned long long int tOffset,
										 int tRetCode, const char * tRetMsg);
		char * getCacheHome();
		void setClientConnTime(time_t tClientConnTime);
		void setServerConnTime(time_t tServerConnTime);
		time_t getClientConnTime();
		time_t getServerConnTime();
		void setClientFileCacheWriteIndex(int tWriteIndex);
		int  initClientFileCacheWrite(int tWriteIndex);
		void setClientFileCacheReadIndex(int tReadIndex);
		int  initClientFileCacheRead(int tReadIndex);
		int  deleteFileSliceByReadIndex(int tReadIndex);

	private:
		int clientSock;					//读取客户端连接的socket
		int serverSock;					//和远端服务器连接的socket

		int startState;					//客户端一开始连接时的状态
		int taskType;					//任务类型: normal/data
		int clientState;				//客户端识别后的连接状态
		int serverState;				//服务器识别后的连接状态

		char serverIP[16];				//远端服务器IP地址
		unsigned short int serverPort;	//远端服务器Port，主机字节序

		char clientConnBuf[4096];		//读取客户端发送报文的缓冲区
		int  clientConnBufLen;			//读取客户端发送报文的长度
		bool isFirstDecode;				//是否为第一次解析客户端发送的报文

		char cacheHome[300];			//缓存目录

		int  clientPktCacheFd;						//接收客户端数据报文的缓存文件描述符，只写模式
		int  clientPktCacheReadFd;					//读取该文件的描述符，只读模式，供Server端发送数据使用
		unsigned long long int clientPktCacheLen;	//接收客户端数据报文的文件长度
		int  serverPktCacheFd;						//接收服务器数据报文的缓存文件描述符，只写模式
		int  serverPktCacheReadFd;					//读取该文件的描述符，只读模式，供Client端发送数据使用
		unsigned long long int serverPktCacheLen;	//接收服务器数据报文的文件长度

		unsigned long long int serverPktSendLen;	//已经发送给服务器端的字节数
		unsigned long long int clientPktSendLen;	//已经发送给客户端的字节数

		bool isSlave;								//是否为从属Worker
		void *pSlave;								//从属Worker指针，用于设置服务器端epoll事件监听
		void *pMaster;								//父Worker指针

		char serverSendBuf[102400];					//发送给服务器的缓冲区
		char serverRecvBuf[2048];					//接收服务器响应的缓冲区
		char clientSendBuf[2048];					//发送给客户端的缓冲区
		char clientRecvBuf[102400];					//接收客户端发送数据的缓冲区

		unsigned long long int fileSize;			//客户端发送文件的长度
		unsigned long long int fileOffset;			//客户端发送文件的偏移
		unsigned long long int fileRecvLen;			//已经接收到客户端发送过来的数据长度
		unsigned long long int fileSendLen;			//已经发送到服务器的数据长度

		long long int uin;							//客户端的QQ号码
		int seq;									//客户端与服务器端进行数据交互时的Seq

		time_t clientConnTime;						//客户端连接的最近时间
		time_t serverConnTime;						//服务器连接的最近时间

		int  clientFileCacheCurrentWriteFd;				//缓存客户端数据的文件写描述符，用于接收客户端数据并写入缓存
		int  clientFileCacheCurrentWriteIndex;			//缓存客户端数据的文件的写定位
		unsigned int clientFileCacheCurrentWriteOffset;	//缓存客户端数据的文件的写偏移
		int  clientFileCacheCurrentReadFd;				//缓存客户端数据的文件读描述符，用于读取客户端缓存数据并发送
		int  clientFileCacheCurrentReadIndex;			//缓存客户端数据的文件的读定位
		unsigned int clientFileCacheCurrentReadOffset;	//缓存客户端数据的文件的读偏移
};

#endif 
