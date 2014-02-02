#include "CLog.h"
#include "Receiver.h"
#include "ConfPara.h"
#include "Global.h"
#include "Util.h"

//#include "pb/WYPBMessage.pb.h"
#include "pb/msgprotobuf.pb.h"

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <arpa/inet.h>

//using namespace WYStreamMessage;

Receiver::Receiver()
{
	rSockFd = -1;

	isHeaderReadCompelete = false;
	postHeaderLen = 0;
	memset(postHeaderBuf, 0, sizeof(postHeaderBuf));

	postHeaderItemCnt = 0;

	taskDataLen = 0;
	memset(postServHost, 0, sizeof(postServHost));
	memset(postServPort, 0, sizeof(postServPort));

	skyDiskType = -1;

	writeFileLen = 0;
	postDataLen = 0;

	memset(cacheHome, 0, sizeof(cacheHome));
	memset(postUri, 0, sizeof(postUri));

	memset(localCacheFilePath, 0, sizeof(localCacheFilePath));
	localCacheFileFd = -1;

	tcpHeaderLen = 0;
	isHaveInfohash = false;
	memset(tcpHeader, 0, sizeof(tcpHeader));

	memset(inputResBuf, 0, sizeof(inputResBuf));
	inputResSendLen = 0;
	inputResLen = 0;

	tcpFileSize = 0;
	tcpOffset   = 0;
	tcpSliceLen = 0;

	connTime = 0;
}

Receiver::~Receiver()
{
	if (rSockFd > 0){
		close(rSockFd);
		rSockFd = -1;
	}

	if (localCacheFileFd > 0){
		close(localCacheFileFd);
		localCacheFileFd = -1;
	}

	printf("Receiver remove!\n");
}

void Receiver::setReadSockFd(int sockFd)
{
	rSockFd = sockFd;
}

int Receiver::getReadSockFd()
{
	return rSockFd;
}

void Receiver::setReadState(int state)
{
	rState = state;
}

int Receiver::getReadState()
{
	return rState;
}

void Receiver::closeReadSockFd()
{
	if (rSockFd > 0){
		close(rSockFd);
		rSockFd = -1;
	}
}

int Receiver::readInputData(int sockFd)
{
	int readCnt;

	if (WEIYUN_HTTP_POST == skyDiskType){
		do{
			readCnt = read(sockFd, postDataBuf, 1048576);
			if (readCnt < 0){
				if ((errno != EAGAIN) || (errno != EWOULDBLOCK)){
					if (g_CP.getIsLogPrint())
						g_Logger.writeLog("In %s:%d, Client Connection gone.\n", __FILE__, __LINE__);

					return S_READ_ERROR;
				}

				break;
			}else if (readCnt == 0){	// close by client
				if ((taskDataLen > 0) && (taskDataLen == postDataLen)){
					genInputHTTPRes();
					setReadState(S_READ_RESPONSE_SENDING);
					return S_READ_OK;
				}else if ((taskDataLen == 0) && (postDataLen > 0)){
					genInputHTTPRes();
					setReadState(S_READ_RESPONSE_SENDING);
					return S_READ_OK;
				}

				if (g_CP.getIsLogPrint())
					g_Logger.writeLog("In %s:%d, Client Connection gone.", __FILE__, __LINE__);

				return S_READ_ERROR;
			}else{
				lseek(localCacheFileFd, writeFileLen, SEEK_SET);
				if( write(localCacheFileFd, postDataBuf, readCnt) != readCnt){
					if (g_CP.getIsLogPrint())
						g_Logger.writeLog("In %s:%d, Recv Post Data Error, Write File Error!", __FILE__, __LINE__);

					return S_READ_ERROR;
				}

				postDataLen += readCnt;
				writeFileLen += readCnt;

				if ((taskDataLen > 0) && (taskDataLen == postDataLen)){
					genInputHTTPRes();
					setReadState(S_READ_RESPONSE_SENDING);
					return S_READ_OK;
				}
			}
		}while(1);
	}
	else if (WEIYUN_TCP_SLICE == skyDiskType || WEIYUN_CHROME_TCP_SLICE == skyDiskType){
		do {
			if (false == isHaveInfohash){
				readCnt = read(rSockFd, &tcpHeader[tcpHeaderLen], sizeof(tcpHeader) - tcpHeaderLen);
				if (readCnt < 0){
					if ((errno != EAGAIN) || (errno != EWOULDBLOCK)){
						if (g_CP.getIsLogPrint())
							g_Logger.writeLog("In %s:%d, Client Connection gone.", __FILE__, __LINE__);

						return S_READ_ERROR;
					}

					break;
				}else if (readCnt == 0){	// close by client
					if (g_CP.getIsLogPrint())
						g_Logger.writeLog("In %s:%d, close by client!", __FILE__, __LINE__);

					return S_READ_ERROR;
				}else{
					tcpHeaderLen += readCnt;
					if (tcpHeaderLen >= 360){
						if ((unsigned char)tcpHeader[0] != 0xab || (unsigned char)tcpHeader[1] != 0xcd ||
							(unsigned char)tcpHeader[2] != 0x98 || (unsigned char)tcpHeader[3] != 0x76)
							return S_READ_ERROR;

						char *ptr = tcpHeader;
						int  iOffset = 0;

						// Header CMD
						iOffset = 4;
						tcpHeaderCmd = ntohl(*(unsigned int *)(ptr + iOffset));
						printf("Header Cmd = %u\n", tcpHeaderCmd);

						// UKeyLen
						iOffset = 16;
						unsigned short int headerUKeyLen = ntohs(*(unsigned short int *)(ptr + iOffset));
						printf("Header Key Len = %u\n", headerUKeyLen);

						// FileHash
						iOffset = 16 + 2 + headerUKeyLen + 2;
						memcpy(infohash, ptr + iOffset, 20);
						
						char *pIndex = infohashStr;
						memset(infohashStr, 0 ,sizeof(infohashStr));
						for (int i = 0; i < 20; i++)
							sprintf(pIndex + 2*i, "%.2X", infohash[i]);
						printf("Infohash = %s\n", infohashStr);
						
						if (1000 == tcpHeaderCmd){
							iOffset = 16 + 2 + headerUKeyLen + 2 + 20;
							tcpFileSize = ntohl(*(unsigned int *)(ptr + iOffset));
							
							iOffset = 16 + 2 + headerUKeyLen + 2 + 20 + 4;
							tcpOffset = ntohl(*(unsigned int *)(ptr + iOffset));

							iOffset = 16 + 2 + headerUKeyLen + 2 + 20 + 4 + 4;
							tcpSliceLen = ntohl(*(unsigned int *)(ptr + iOffset));
						}else if (1007 == tcpHeaderCmd){
							unsigned int tcpFileSizeLow, tcpFileSizeHigh;
							unsigned int tcpOffsetLow, tcpOffsetHigh;
							
							iOffset = 16 + 2 + headerUKeyLen + 2 + 20;
							tcpFileSizeLow = ntohl(*(unsigned int *)(ptr + iOffset));

							iOffset = 16 + 2 + headerUKeyLen + 2 + 20 + 4;
							tcpOffsetLow = ntohl(*(unsigned int *)(ptr + iOffset));

							iOffset = 16 + 2 + headerUKeyLen + 2 + 20 + 4 + 4;
							tcpSliceLen = ntohl(*(unsigned int *)(ptr + iOffset));

							iOffset = 16 + 2 + headerUKeyLen + 2 + 20 + 4 + 4 + 4;
							tcpFileSizeHigh = ntohl(*(unsigned int *)(ptr + iOffset));

							iOffset = 16 + 2 + headerUKeyLen + 2 + 20 + 4 + 4 + 4 + 4;
							tcpOffsetHigh = ntohl(*(unsigned int *)(ptr + iOffset));
								
							tcpFileSize = 4294967296ULL * tcpFileSizeHigh + tcpFileSizeLow;
							tcpOffset = 4294967296ULL * tcpOffsetHigh + tcpOffsetLow;
						}

						printf("%s:%d, FiliSize = %llu, Offset = %llu, Slice Len = %u\n", __FILE__, __LINE__, tcpFileSize, tcpOffset, tcpSliceLen);

						isHaveInfohash = true;
						if (initTCPSliceCache(infohashStr, tcpOffset) < 0){
							if (g_CP.getIsLogPrint())
										g_Logger.writeLog("In %s:%d, init tcp slice error!", __FILE__, __LINE__);

							return S_READ_ERROR;
						}

						// write to file (header)
						if (writeHeaderToFile() != S_OK){
							if (g_CP.getIsLogPrint())
								g_Logger.writeLog("In %s:%d, write header to file error!", __FILE__, __LINE__);

							return S_READ_ERROR;
						}	

						// write left data
						lseek(localCacheFileFd, writeFileLen, SEEK_SET);
						write(localCacheFileFd, tcpHeader, tcpHeaderLen);

						writeFileLen += tcpHeaderLen;
						postDataLen  += tcpHeaderLen;
						printf("postDataLen = %llu\n", postDataLen);

						continue;
					}
				}	
			}
			else {
				readCnt = read(rSockFd, postDataBuf, 1048576);
				if (readCnt < 0){
					if ((errno != EAGAIN) || (errno != EWOULDBLOCK)){
						if (g_CP.getIsLogPrint())
							g_Logger.writeLog("In %s:%d, Client Connection gone.\n", __FILE__, __LINE__);

						return S_READ_ERROR;
					}

					break;
				}else if (readCnt == 0){	// close by client
					if ((taskDataLen > 0) && (taskDataLen == postDataLen)){
						genInputTCPRes();
						setReadState(S_READ_RESPONSE_SENDING);
						return S_READ_OK;
					}

					if (g_CP.getIsLogPrint())
						g_Logger.writeLog("In %s:%d, Client Connection gone.\n", __FILE__, __LINE__);

					return S_READ_ERROR;
				}else{
					lseek(localCacheFileFd, writeFileLen, SEEK_SET);
					if( write(localCacheFileFd, postDataBuf, readCnt) != readCnt){
						if (g_CP.getIsLogPrint())
							g_Logger.writeLog("In %s:%d, Recv Post Data Error, Write File Error!\n", __FILE__, __LINE__);

						return S_READ_ERROR;
					}

					postDataLen  += readCnt;
					writeFileLen += readCnt;
					printf("postDataLen = %llu\n", postDataLen);

					if (taskDataLen > 0 && taskDataLen == (unsigned long long)postDataLen){
						genInputTCPRes();
						setReadState(S_READ_RESPONSE_SENDING);
						printf("ready to return ok!\n");
						return S_READ_OK;
					}
				}
			}
		}while(1);
	}

	return S_READ_INCOMPLETE;
}

int Receiver::readInputHeader(int sockFd)
{
	//char s[2] = {0};
	char httpMethod[10] = {0};

	int readSize = read(sockFd, postHeaderBuf, 1000);
	if (readSize >= 16){
		for (int i = 0; i < 16; i++)
			printf("%.2X ", (unsigned char)postHeaderBuf[i]);

		int offset = 0;
		char *p = postHeaderBuf;

		offset += 4;
		unsigned short int wType = *(unsigned short int *)(p + offset);

		offset += 2;
		unsigned int dwPackLen = *(unsigned int *)(p + offset);

		offset += 4;
		unsigned int dwBodyLen = *(unsigned int *)(p + offset);

		offset += 4;
		unsigned short int wReserved = *(unsigned short int *)(p + offset);

		printf("wType = %u, dwPackLen = %u, dwBodyLen = %u, wReserved = %u\n", wType, dwPackLen, dwBodyLen, wReserved);

		printf("readSize = %d\n", readSize);
//		if (dwPackLen == (unsigned int)readSize){
//			printf("packet ok!\n");

//			p = &postHeaderBuf[16];

//			PBHead *pbHead = new PBHead();
//			pbHead->ParseFromArray(p, dwPackLen - dwBodyLen - 16);
//		}


	}


	

#if 0
	if (false == isHeaderReadCompelete){
		while(1) {
			if( read(sockFd, s, 1) <= 0 ){
				if ((errno != EAGAIN) || (errno != EWOULDBLOCK)){
					if (g_CP.getIsLogPrint())
						g_Logger.writeLog("In %s:%d, Client Connection gone.", __FILE__, __LINE__);

					return S_READ_ERROR;
				}

				return S_READ_INCOMPLETE;
			}

			strcat(postHeaderBuf, s);
			postHeaderLen++;

			if (postHeaderLen >=4){
				printf("%.2X %.2X %.2X %.2X\n", postHeaderBuf[0], postHeaderBuf[1], postHeaderBuf[2], postHeaderBuf[3]);
				exit(0);
			}

#if 0
			if ( (postHeaderLen > 4) && 
				'\n' == postHeaderBuf[postHeaderLen-1] && '\r' == postHeaderBuf[postHeaderLen-2] &&
				'\n' == postHeaderBuf[postHeaderLen-3] && '\r' == postHeaderBuf[postHeaderLen-4] ){

				isHeaderReadCompelete = true;
				break;
			}
#endif
		}
	}
#endif

	postHeaderBuf[postHeaderLen-1] = '\0';	postHeaderBuf[postHeaderLen-2] = '\0';
	postHeaderLen -= 2;

	// decode post header
	char *start = postHeaderBuf, *end;
	while ( (end = strstr(start, "\r\n")) != NULL){
		*end = '\0';
		memset(postHeaderItem[postHeaderItemCnt], 0, MAX_STRING);
		strcpy(postHeaderItem[postHeaderItemCnt], start);

		if (0 == strncmp(postHeaderItem[postHeaderItemCnt], "Content-Length", 14)){
			sscanf(postHeaderItem[postHeaderItemCnt], "%*s %lld", &taskDataLen);
		}

		if (0 == strncmp(postHeaderItem[postHeaderItemCnt], "Host", 4)){
			char servInfo[100] = {0};
			sscanf(postHeaderItem[postHeaderItemCnt], "%*s %s", servInfo);
			sscanf(servInfo,"%[^:]:%s", postServHost, postServPort);
		}

		postHeaderItemCnt++;

		end++;	end++;	// skip  '\r\n'
		start = end;

		if ((int)(start - &postHeaderBuf[0]) >= postHeaderLen)
			break;
	}

	//for (int i = 0; i < postHeaderItemCnt; i++)
	//	printf("%s\n", postHeaderItem[i]);

	sscanf(postHeaderItem[0], "%s %s", httpMethod, postUri);
	if (0 != strncmp(httpMethod, "POST", 4)){
		if (g_CP.getIsLogPrint())
			g_Logger.writeLog("In %s:%d, not post packet!", __FILE__, __LINE__);
		
		return S_READ_ERROR;
	}

	// determine SkyDrive type
	if(NULL != strstr(postUri,"/ftn_handler/?ver=12345&ukey=")){	//ÌÚÑ¶Î¢ÔÆ
		skyDiskType = WEIYUN_HTTP_POST;
		printf("HTTP POST!\n");
	}else if ( (NULL != strstr(postUri, "/ftn_handler/")) && (NULL != strstr(postUri, "bmd5="))){
		skyDiskType = WEIYUN_TCP_SLICE;
		printf("TCP SLICE!\n");
	}else if ((NULL != strstr(postUri, "/ftn_handler")) && (NULL == strstr(postUri, "bmd5="))) {
		skyDiskType = WEIYUN_CHROME_TCP_SLICE;
		printf("Chrome TCP SLICE!\n");
	}

	setReadState(S_READ_DATA_RECEIVING);

	if (WEIYUN_HTTP_POST == skyDiskType){
		if (initHTTPPostCache() < 0){
			if (g_CP.getIsLogPrint())
				g_Logger.writeLog("In %s:%d, init tcp slice error!", __FILE__, __LINE__);

			return S_READ_ERROR;
		}

		if (writeHeaderToFile() != S_OK){
			if (g_CP.getIsLogPrint())
				g_Logger.writeLog("In %s:%d, init tcp slice error!", __FILE__, __LINE__);

			return S_READ_ERROR;
		}

		// read data
		int readCnt;
		do{
			readCnt = read(rSockFd, postDataBuf, 1048576);
			if (readCnt < 0){
				if ((errno != EAGAIN) || (errno != EWOULDBLOCK)){
					if (g_CP.getIsLogPrint())
						g_Logger.writeLog("In %s:%d, Client Connection gone.", __FILE__, __LINE__);

					return S_READ_ERROR;
				}

				break;
			}else if (readCnt == 0){	// close by client
				if ((taskDataLen > 0) && (taskDataLen == postDataLen)){
					genInputHTTPRes();
					setReadState(S_READ_RESPONSE_SENDING);
					return S_READ_OK;
				}else if ((taskDataLen == 0) && (postDataLen > 0)){
					genInputHTTPRes();
					setReadState(S_READ_RESPONSE_SENDING);
					return S_READ_OK;
				}

				if (g_CP.getIsLogPrint())
					g_Logger.writeLog("In %s:%d, Client Connection gone.", __FILE__, __LINE__);

				return S_READ_ERROR;
			}else{
				lseek(localCacheFileFd, writeFileLen, SEEK_SET);
				if( write(localCacheFileFd, postDataBuf, readCnt) != readCnt){
					if (g_CP.getIsLogPrint())
						g_Logger.writeLog("In %s:%d, Recv Post Data Error, Write File Error!", __FILE__, __LINE__);

					return S_READ_ERROR;
				}

				postDataLen += readCnt;
				writeFileLen += readCnt;
				if ((taskDataLen > 0) && (taskDataLen == postDataLen)){
					genInputHTTPRes();
					setReadState(S_READ_RESPONSE_SENDING);
					return S_READ_OK;
				}
			}
		}while(1);
	}
	else if (WEIYUN_TCP_SLICE == skyDiskType || WEIYUN_CHROME_TCP_SLICE == skyDiskType){
		// read data
		int readCnt;

		do {
			if (false == isHaveInfohash){
				readCnt = read(rSockFd, &tcpHeader[tcpHeaderLen], sizeof(tcpHeader) - tcpHeaderLen);
				if (readCnt < 0){
					if ((errno != EAGAIN) || (errno != EWOULDBLOCK)){
						if (g_CP.getIsLogPrint())
							g_Logger.writeLog("In %s:%d, Client Connection gone.", __FILE__, __LINE__);

						return S_READ_ERROR;
					}

					break;
				}else if (readCnt == 0){	// close by client
					if (g_CP.getIsLogPrint())
						g_Logger.writeLog("In %s:%d, Recv Post Data Error, Write File Error!", __FILE__, __LINE__);

					return S_READ_ERROR;
				}else{
					tcpHeaderLen += readCnt;
					if (tcpHeaderLen >= 360){
						if ((unsigned char)tcpHeader[0] != 0xab || (unsigned char)tcpHeader[1] != 0xcd ||
							(unsigned char)tcpHeader[2] != 0x98 || (unsigned char)tcpHeader[3] != 0x76){
							if (g_CP.getIsLogPrint())
								g_Logger.writeLog("In %s:%d, Not a valid packet, not start with 0xab cd 98 76.", __FILE__, __LINE__);

							return S_READ_ERROR;
						}

						char *ptr = tcpHeader;
						int  iOffset = 0;

						// Header CMD
						iOffset = 4;
						tcpHeaderCmd = ntohl(*(unsigned int *)(ptr + iOffset));
						printf("Header Cmd = %u\n", tcpHeaderCmd);

						// UKeyLen
						iOffset = 16;
						unsigned short int headerUKeyLen = ntohs(*(unsigned short int *)(ptr + iOffset));
						printf("Header Key Len = %u\n", headerUKeyLen);

						// FileHash
						iOffset = 16 + 2 + headerUKeyLen + 2;
						memcpy(infohash, ptr + iOffset, 20);
						
						char *pIndex = infohashStr;
						memset(infohashStr, 0 ,sizeof(infohashStr));
						for (int i = 0; i < 20; i++)
							sprintf(pIndex + 2*i, "%.2X", infohash[i]);
						printf("Infohash = %s\n", infohashStr);
						
						if (1000 == tcpHeaderCmd){
							iOffset = 16 + 2 + headerUKeyLen + 2 + 20;
							tcpFileSize = ntohl(*(unsigned int *)(ptr + iOffset));
							
							iOffset = 16 + 2 + headerUKeyLen + 2 + 20 + 4;
							tcpOffset = ntohl(*(unsigned int *)(ptr + iOffset));

							iOffset = 16 + 2 + headerUKeyLen + 2 + 20 + 4 + 4;
							tcpSliceLen = ntohl(*(unsigned int *)(ptr + iOffset));
						}else if (1007 == tcpHeaderCmd){
							unsigned int tcpFileSizeLow, tcpFileSizeHigh;
							unsigned int tcpOffsetLow, tcpOffsetHigh;
							
							iOffset = 16 + 2 + headerUKeyLen + 2 + 20;
							tcpFileSizeLow = ntohl(*(unsigned int *)(ptr + iOffset));

							iOffset = 16 + 2 + headerUKeyLen + 2 + 20 + 4;
							tcpOffsetLow = ntohl(*(unsigned int *)(ptr + iOffset));

							iOffset = 16 + 2 + headerUKeyLen + 2 + 20 + 4 + 4;
							tcpSliceLen = ntohl(*(unsigned int *)(ptr + iOffset));

							iOffset = 16 + 2 + headerUKeyLen + 2 + 20 + 4 + 4 + 4;
							tcpFileSizeHigh = ntohl(*(unsigned int *)(ptr + iOffset));

							iOffset = 16 + 2 + headerUKeyLen + 2 + 20 + 4 + 4 + 4 + 4;
							tcpOffsetHigh = ntohl(*(unsigned int *)(ptr + iOffset));
								
							tcpFileSize = 4294967296ULL * tcpFileSizeHigh + tcpFileSizeLow;
							tcpOffset = 4294967296ULL * tcpOffsetHigh + tcpOffsetLow;
						}

						printf("%s:%d, FiliSize = %llu, Offset = %llu, Slice Len = %u\n", __FILE__, __LINE__, tcpFileSize, tcpOffset, tcpSliceLen);

						isHaveInfohash = true;
						if (initTCPSliceCache(infohashStr, tcpOffset) < 0){
							if (g_CP.getIsLogPrint())
								g_Logger.writeLog("In %s:%d, init tcp slice error!", __FILE__, __LINE__);

							return S_READ_ERROR;
						}	

						// write to file (header)
						if (writeHeaderToFile() != S_OK){
							if (g_CP.getIsLogPrint())
								g_Logger.writeLog("In %s:%d, write tcp slice header to file error!", __FILE__, __LINE__);

							return S_READ_ERROR;
						}

						// write left data
						lseek(localCacheFileFd, writeFileLen, SEEK_SET);
						if (write(localCacheFileFd, tcpHeader, tcpHeaderLen) != tcpHeaderLen){
							if (g_CP.getIsLogPrint())
								g_Logger.writeLog("In %s:%d, write left data to file error!", __FILE__, __LINE__);

							return S_READ_ERROR;
						}

						writeFileLen += tcpHeaderLen;
						postDataLen  += tcpHeaderLen;
						printf("postDataLen = %llu\n", postDataLen);

						continue;
					}
				}	
			}
			else {
				readCnt = read(rSockFd, postDataBuf, 1048576);
				if (readCnt < 0){
					if ((errno != EAGAIN) || (errno != EWOULDBLOCK)){
						if (g_CP.getIsLogPrint())
							g_Logger.writeLog("In %s:%d, Client Connection gone.\n", __FILE__, __LINE__);

						return S_READ_ERROR;
					}

					break;
				}else if (readCnt == 0){	// close by client
					if ((taskDataLen > 0) && (taskDataLen == postDataLen)){
						genInputTCPRes();
						setReadState(S_READ_RESPONSE_SENDING);
						return S_READ_OK;
					}
				
					if (g_CP.getIsLogPrint())
						g_Logger.writeLog("In %s:%d, close by client!\n", __FILE__, __LINE__);

					return S_READ_ERROR;
				}else{
					lseek(localCacheFileFd, writeFileLen, SEEK_SET);
					if( write(localCacheFileFd, postDataBuf, readCnt) != readCnt){
						if (g_CP.getIsLogPrint())
							g_Logger.writeLog("In %s:%d, Recv Post Data Error, Write File Error!\n", __FILE__, __LINE__);

						return S_READ_ERROR;
					}

					postDataLen  += readCnt;
					writeFileLen += readCnt;
					printf("postDataLen = %llu\n", postDataLen);

					if ((taskDataLen > 0) && (taskDataLen == postDataLen)){
						genInputTCPRes();
						setReadState(S_READ_RESPONSE_SENDING);
						return S_READ_OK;
					}
				}
			}
		}while(1);
	}
	
	return S_READ_OK;
}

int Receiver::initHTTPPostCache()
{
	int folderValue = rand();
	sprintf(cacheHome, "%s%d%s", g_CP.getCacheRoot(), folderValue, "/");
	
	if (CreateDir(cacheHome) < 0){
		if (g_CP.getIsLogPrint())
			g_Logger.writeLog("In %s:%d, create dir error.", __FILE__, __LINE__);

		return -1;
	}
	
	if (initCacheFile(cacheHome) < 0){
		if (g_CP.getIsLogPrint())
			g_Logger.writeLog("In %s:%d, init http cache file error!", __FILE__, __LINE__);

		return -1;
	}

	return 0;
}

int Receiver::initTCPSliceCache(char *hashStr, unsigned long long offset)
{
	sprintf(cacheHome, "%s%s%s", g_CP.getCacheRoot(), hashStr, "/");
	if (CreateDir(cacheHome) < 0){
		if (g_CP.getIsLogPrint())
			g_Logger.writeLog("In %s:%d, create dir error.", __FILE__, __LINE__);

		return -1;
	}
	
	if (initTCPSliceCacheFile(offset) < 0){
		if (g_CP.getIsLogPrint())
			g_Logger.writeLog("In %s:%d, init tcp slice cache file error!", __FILE__, __LINE__);

		return -1;
	}
	
	return 0;
}

int Receiver::initTCPSliceCacheFile(unsigned long long offset)
{
	sprintf(localCacheFilePath, "%s%llu%s", cacheHome, offset, ".dat");

	if( (localCacheFileFd = open(localCacheFilePath, O_CREAT|O_WRONLY, 0666)) == -1 ){
		if (g_CP.getIsLogPrint())
			g_Logger.writeLog("%s:%d, Error creating local cache file", __FILE__, __LINE__);

		return -1;
	}

	return 0;
}

void Receiver::clearCacheHome()
{
	deleteFileByFolder(cacheHome);
}

void Receiver::genInputHTTPRes()
{
	//char gmtTime[35] = {0};
	//getGMTTime(gmtTime);

	inputResLen = 0;
	memset(inputResBuf, 0, sizeof(inputResBuf));

	strcpy(inputResBuf, "HTTP/1.1 200 OK\r\n");
	strcat(inputResBuf, "Content-Type: text/html; charset=GB18030\r\n");
	strcat(inputResBuf, "Content-Length: 79\r\n\r\n");

	// ÏìÓ¦Âë£¬ÎªÎ¢ÔÆÍøÅÌ
	strcat(inputResBuf,"<script>document.domain=\"qq.com\";try{parent.ftn_post_end(0)}catch(e){}</script>");

	inputResLen = strlen(inputResBuf);
	inputResSendLen = 0;
	//printf("WEIYUNresBuf:\n %s\n", inputResBuf);
}

void Receiver::genInputTCPRes()
{
	inputResLen = 0;
	memset(inputResBuf, 0, sizeof(inputResBuf));

	strcpy(inputResBuf, "HTTP/1.1 200 OK\r\n");
	strcat(inputResBuf, "Content-Type: text/octet\r\n");
	
	if (1000 == tcpHeaderCmd) {
		strcat(inputResBuf, "Content-Length: 17\r\n\r\n");
	}else if (1007 == tcpHeaderCmd) {
		strcat(inputResBuf, "Content-Length: 25\r\n\r\n");
	}

	inputResLen = strlen(inputResBuf);

	// Header Magic
	inputResBuf[inputResLen++] = 0xab;
	inputResBuf[inputResLen++] = 0xcd;
	inputResBuf[inputResLen++] = 0x98;
	inputResBuf[inputResLen++] = 0x76;
	
	// Header Error Code
	inputResBuf[inputResLen++] = 0x00;
	inputResBuf[inputResLen++] = 0x00;
	inputResBuf[inputResLen++] = 0x00;
	inputResBuf[inputResLen++] = 0x00;
	
	// Header Reserve
	inputResBuf[inputResLen++] = 0x00;
	inputResBuf[inputResLen++] = 0x00;
	inputResBuf[inputResLen++] = 0x00;
	inputResBuf[inputResLen++] = 0x00;

	// pkt body len
	char *pResBufOffset = inputResBuf;
	if (1000 == tcpHeaderCmd){
		*(unsigned int *) (pResBufOffset + inputResLen) = htonl(5);
		inputResLen += 4;

		if (tcpFileSize == (tcpOffset + tcpSliceLen)) {	
			*(pResBufOffset + inputResLen) = 0x01;
			inputResLen++;

			*(unsigned int *) (pResBufOffset + inputResLen)	= htonl(tcpFileSize);
			inputResLen += 4;			
		}
		else {
			*(pResBufOffset + inputResLen) = 0x00;
			inputResLen++;

			*(unsigned int *) (pResBufOffset + inputResLen)	= htonl(tcpOffset + tcpSliceLen);
			inputResLen += 4;			
		}
	}else if (1007 == tcpHeaderCmd){
		*(unsigned int *) (pResBufOffset + inputResLen) = htonl(9);
		inputResLen += 4;

		if (tcpFileSize == (tcpOffset + tcpSliceLen)) {
			printf("Complete!\n");
			*(pResBufOffset + inputResLen) = 0x01;
			inputResLen++;

			unsigned int nextOffsetHigh = tcpFileSize / 4294967296ULL;
			unsigned int nextOffsetLow  = tcpFileSize % 4294967296ULL;

			*(unsigned int *)(pResBufOffset + inputResLen) = htonl(nextOffsetLow);
			inputResLen += 4;

			*(unsigned int *)(pResBufOffset + inputResLen) = htonl(nextOffsetHigh);
			inputResLen += 4;			
		}
		else{ 
			printf("Not Complete!\n");
			*(pResBufOffset + inputResLen) = 0x00;
			inputResLen ++;

			unsigned int nextOffsetHigh = (tcpOffset + tcpSliceLen) / 4294967296ULL;
			unsigned int nextOffsetLow  = (tcpOffset + tcpSliceLen) % 4294967296ULL;

			*(unsigned int *)(pResBufOffset + inputResLen) = htonl(nextOffsetLow);
			inputResLen += 4;

			*(unsigned int *)(pResBufOffset + inputResLen) = htonl(nextOffsetHigh);
			inputResLen += 4;
		}
	}

	inputResSendLen = 0;

	//printf("In Receive Len, inputResSendLen = %d\n", );
	//printf("TCP Res = %s\n", inputResBuf);
}

int Receiver::getInputResLen()
{
	return inputResLen - inputResSendLen;
}

char * Receiver::getCacheHome()
{
	return cacheHome;
}

int Receiver::getSkyDiskType()
{
	return skyDiskType;
}

char * Receiver::getPostServHost()
{
	return postServHost;
}

char * Receiver::getPostServPort()
{
	return postServPort;
}

unsigned long long Receiver::getTcpFileSize()
{
	return tcpFileSize;
}

unsigned long long Receiver::getTcpOffset()
{
	return tcpOffset;
}

unsigned int Receiver::getTcpSliceLen()
{
	return tcpSliceLen;
}

bool Receiver::isTcpLastSlice()
{
	if (tcpFileSize == (tcpOffset + tcpSliceLen))
		return true;
	else
		return false;
}

void Receiver::setConnTime(time_t tTime)
{
	connTime = tTime;
}

time_t Receiver::getConnTime()
{
	return connTime;
}

int Receiver::sendResData()
{
	if (0 == inputResLen)
		return S_READ_RESPONSE_OK;
	else if (inputResSendLen >= inputResLen)
		return S_READ_RESPONSE_OK;

	int sendDataLen = inputResLen - inputResSendLen;
	int sendCnt = write(rSockFd, inputResBuf + inputResSendLen, sendDataLen);
	if (sendCnt < 0){
		if(errno == EINTR)
			sendCnt = 0;
		else if(errno != EWOULDBLOCK || errno != EAGAIN){
			return S_READ_ERROR;
		}
	}

	inputResSendLen += sendCnt;
	if (inputResLen == inputResSendLen)
		return S_READ_RESPONSE_OK;

	return S_READ_INCOMPLETE;
}

int Receiver::initCacheFile(char * tCacheHome)
{
	sprintf(localCacheFilePath, "%s%s", tCacheHome, "CacheData.dat");

	if( (localCacheFileFd = open(localCacheFilePath, O_CREAT|O_WRONLY, 0666)) == -1 ){
		if (g_CP.getIsLogPrint())
			g_Logger.writeLog("%s:%d, Error creating local cache file", __FILE__, __LINE__);

		return -1;
	}

	return 0;
}

void Receiver::closeCacheFileFd()
{
	if (localCacheFileFd > 0){
		close(localCacheFileFd);
	}

	localCacheFileFd = -1;
}

int Receiver::writeHeaderToFile()
{
	// Todo: Write Code Flag for hiwifi router
	unsigned int retCnt;
	
	for (int i = 0; i < postHeaderItemCnt; i++){
		if (i != postHeaderItemCnt - 1){
			postHeaderItem[i][strlen(postHeaderItem[i])] = '\r';
			postHeaderItem[i][strlen(postHeaderItem[i])] = '\n';
		}else{
			postHeaderItem[i][strlen(postHeaderItem[i])] = '\r';
			postHeaderItem[i][strlen(postHeaderItem[i])] = '\n';
			postHeaderItem[i][strlen(postHeaderItem[i])] = '\r';
			postHeaderItem[i][strlen(postHeaderItem[i])] = '\n';
		}
			
		lseek(localCacheFileFd, writeFileLen, SEEK_SET);
		retCnt = write(localCacheFileFd, postHeaderItem[i], strlen(postHeaderItem[i]));
		if (retCnt != strlen(postHeaderItem[i])){
			if (g_CP.getIsLogPrint())
				g_Logger.writeLog("In %s:%d, write header to file error!", __FILE__, __LINE__);

			return S_ERROR;
		}

		writeFileLen += strlen(postHeaderItem[i]);
	}

	return S_OK;
}

