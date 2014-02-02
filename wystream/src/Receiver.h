#ifndef HEADER_RECEIVER_H
#define HEADER_RECEIVER_H

#include "Global.h"

#include <time.h>

#define S_READ_ERROR			-1	// 表示与客户端的交互出错
#define S_READ_OK				0	// 表示读取完成
#define	S_READ_INCOMPLETE		1	// 表示状态的数据读取未完成

#define S_READ_ACCEPTING 		2	// 表示与客户端正在TCP握手
#define S_READ_HEADER_RECEIVING 3	// 表示与客户端握手成功，等待读取头部
#define S_READ_DATA_RECEIVING	4	// 表示正在接收客户端的数据
#define S_READ_RESPONSE_SENDING 5   // 表示正在给用户发送响应报文
#define S_READ_RESPONSE_OK		6	// 表示给用户发送的数据已成功

#define S_READ_SUCESS			100	// 表示读取成功
#define S_READ_FAILED			101	// 表示读取失败

class Receiver {
	public:
		Receiver();
		~Receiver();

	public:
		void setReadSockFd(int sockFd);
		int  getReadSockFd();
		int  readInputHeader(int sockFd);
		int  initHTTPPostCache();
		void setReadState(int state);
		int  getReadState();
		void closeReadSockFd();
		int  readInputData(int sockFd);
		int  initTCPSliceCache(char *hashStr, unsigned long long offset);
		int  initTCPSliceCacheFile(unsigned long long offset);
		int  initCacheFile(char * cacheHome);
		void closeCacheFileFd();
		int  writeHeaderToFile();
		void clearCacheHome();
		void genInputHTTPRes();
		void genInputTCPRes();
		int  getInputResLen();
		int  sendResData();
		char * getCacheHome();
		int  getSkyDiskType();
		char * getPostServHost();
		char * getPostServPort();
		unsigned long long getTcpFileSize();
		unsigned long long getTcpOffset();
		unsigned int getTcpSliceLen();
		bool isTcpLastSlice();
		void   setConnTime(time_t tTime);
		time_t getConnTime();

	private:
		int  rSockFd;		// 接收客户端数据的socket
		int  rState;		// 接收客户端数据的状态

		bool isHeaderReadCompelete;	// 表示已经读取完客户端发送的头部信息
		char postHeaderBuf[4097];	// Peer Post过来的报文头部
		int  postHeaderLen;			// 已经读取的报文头部

		char postHeaderItem[MAX_ADD_HEADERS][MAX_STRING];	// post 报头项内容
		int  postHeaderItemCnt;								// post 报头项个数

		unsigned long long taskDataLen;		// 表示本次Peer上传数据的大小, 文件中包含Boundary分隔

		char postServHost[100];		// 服务器域名，取值 HTTP Header的Host 字段
		char postServPort[7];		// 服务器端口	

		int  skyDiskType;			// 上传的网盘类型

		char cacheHome[256];		// 缓存目录

		char postUri[MAX_STRING];	// post 的URI

		char localCacheFilePath[256];		// 本地缓存文件路径
		int  localCacheFileFd;				// 本地缓存文件的fd
		unsigned long long writeFileLen;	// 已经写入本地文件的字节数

		char postDataBuf[1048577];			// Peer Post过来的数据
		unsigned long long  postDataLen;	// Peer 已Post过来的数据长度

		bool isHaveInfohash;			// 是否已经获取infohash值
		char tcpHeader[400];			// tcp 分片上传时的头部
		int  tcpHeaderLen;				// 已经接收tcp头部的数据
		unsigned char infohash[20];		// tcp 分片上传时的infohash
		char infohashStr[41];			// tcp 分片上传时的infohash
		unsigned long long int tcpFileSize;	// tcp 分片上传时对应的文件总大小
		unsigned long long int tcpOffset;	// tcp 分片上传时对应的偏移
		unsigned int tcpSliceLen;		// tcp 分片上传时当前传输数据大小
		unsigned int tcpHeaderCmd;		// tcp 分片的头部命令字，1000(<4G) 或 1007(>4G)

		char inputResBuf[1024];		// 客户端上传数据的响应报文
		int  inputResLen;			// 等待发送上传响应数据的长度
		int  inputResSendLen;		// 已经发送的数据

		time_t connTime;			// 上次连接的时间戳
};

#endif 
