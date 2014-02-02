#ifndef HEADER_RECEIVER_H
#define HEADER_RECEIVER_H

#include "Global.h"

#include <time.h>

#define S_READ_ERROR			-1	// ��ʾ��ͻ��˵Ľ�������
#define S_READ_OK				0	// ��ʾ��ȡ���
#define	S_READ_INCOMPLETE		1	// ��ʾ״̬�����ݶ�ȡδ���

#define S_READ_ACCEPTING 		2	// ��ʾ��ͻ�������TCP����
#define S_READ_HEADER_RECEIVING 3	// ��ʾ��ͻ������ֳɹ����ȴ���ȡͷ��
#define S_READ_DATA_RECEIVING	4	// ��ʾ���ڽ��տͻ��˵�����
#define S_READ_RESPONSE_SENDING 5   // ��ʾ���ڸ��û�������Ӧ����
#define S_READ_RESPONSE_OK		6	// ��ʾ���û����͵������ѳɹ�

#define S_READ_SUCESS			100	// ��ʾ��ȡ�ɹ�
#define S_READ_FAILED			101	// ��ʾ��ȡʧ��

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
		int  rSockFd;		// ���տͻ������ݵ�socket
		int  rState;		// ���տͻ������ݵ�״̬

		bool isHeaderReadCompelete;	// ��ʾ�Ѿ���ȡ��ͻ��˷��͵�ͷ����Ϣ
		char postHeaderBuf[4097];	// Peer Post�����ı���ͷ��
		int  postHeaderLen;			// �Ѿ���ȡ�ı���ͷ��

		char postHeaderItem[MAX_ADD_HEADERS][MAX_STRING];	// post ��ͷ������
		int  postHeaderItemCnt;								// post ��ͷ�����

		unsigned long long taskDataLen;		// ��ʾ����Peer�ϴ����ݵĴ�С, �ļ��а���Boundary�ָ�

		char postServHost[100];		// ������������ȡֵ HTTP Header��Host �ֶ�
		char postServPort[7];		// �������˿�	

		int  skyDiskType;			// �ϴ�����������

		char cacheHome[256];		// ����Ŀ¼

		char postUri[MAX_STRING];	// post ��URI

		char localCacheFilePath[256];		// ���ػ����ļ�·��
		int  localCacheFileFd;				// ���ػ����ļ���fd
		unsigned long long writeFileLen;	// �Ѿ�д�뱾���ļ����ֽ���

		char postDataBuf[1048577];			// Peer Post����������
		unsigned long long  postDataLen;	// Peer ��Post���������ݳ���

		bool isHaveInfohash;			// �Ƿ��Ѿ���ȡinfohashֵ
		char tcpHeader[400];			// tcp ��Ƭ�ϴ�ʱ��ͷ��
		int  tcpHeaderLen;				// �Ѿ�����tcpͷ��������
		unsigned char infohash[20];		// tcp ��Ƭ�ϴ�ʱ��infohash
		char infohashStr[41];			// tcp ��Ƭ�ϴ�ʱ��infohash
		unsigned long long int tcpFileSize;	// tcp ��Ƭ�ϴ�ʱ��Ӧ���ļ��ܴ�С
		unsigned long long int tcpOffset;	// tcp ��Ƭ�ϴ�ʱ��Ӧ��ƫ��
		unsigned int tcpSliceLen;		// tcp ��Ƭ�ϴ�ʱ��ǰ�������ݴ�С
		unsigned int tcpHeaderCmd;		// tcp ��Ƭ��ͷ�������֣�1000(<4G) �� 1007(>4G)

		char inputResBuf[1024];		// �ͻ����ϴ����ݵ���Ӧ����
		int  inputResLen;			// �ȴ������ϴ���Ӧ���ݵĳ���
		int  inputResSendLen;		// �Ѿ����͵�����

		time_t connTime;			// �ϴ����ӵ�ʱ���
};

#endif 
