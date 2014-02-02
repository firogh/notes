#ifndef HEADER_WORKER_H
#define HEADER_WORKER_H

#include <time.h>

#define S_CLIENT_CONNECT_ERROR			-1	//��ʾ�ͻ��˳�ʼ����ʧ��
#define S_CLIENT_CONNECT_CONNECTING		0	//��ʾ�ͻ��˸ո�����������֣�δ����״̬
#define	S_CLIENT_CONNECT_RECEIVING		1	//��ʾ�ͻ������ڷ��͵�һ����������
#define S_CLIENT_CONNECT_OK				2	//��ʾ�ͻ��˵�һ������ɹ����ܹ���λ���ͻ�������

#define S_CLIENT_UNDEFINE				3	//��ʼ��
#define S_SERVER_UNDEFINE				4	//��ʼ��

#define S_CLIENT_NORMAL_WORKING			5	//��ʾ�ͻ���Ϊ��ͨ���ӣ����ӽ�����
#define S_CLIENT_NORMAL_OK				6	//��ʾ�ͻ��������ر�
#define S_CLIENT_NORMAL_ERROR			7	//��ʾ�ͻ����쳣

#define S_SERVER_NORMAL_WORKING			8	//��ʾ������Ϊ��ͨ���ӣ����ӽ�����
#define S_SERVER_NORMAL_OK				9	//��ʾ������Ϊ��ͨ���ӣ����ӳɹ�����
#define S_SERVER_NORMAL_ERROR			10	//��ʾ������Ϊ��ͨ���ӣ�����ʧ�ܽ���

#define S_CLIENT_DATA_OFFSETREQ_RECV_OK			11	//��ʾ�ͻ���Ϊ�������ӣ�����Offset����ɹ�
#define S_CLIENT_DATA_OFFSETRES_SEND_INDOING	12
#define S_CLIENT_DATA_DATA_RECV_INDOING			13
#define S_CLIENT_DATA_DATA_RECV_OK				14
#define S_CLIENT_DATA_DATARES_SEND_INDOING		15

#define S_SERVER_DATA_OFFSETREQ_SEND_INDOING	17	//��ʾ������Ϊ�������ӣ����ڷ���Offset����
#define S_SERVER_DATA_OFFSETRES_RECV_INDOING	18	//��ʾ������Ϊ�������ӣ����ڽ���Offset����
#define S_SERVER_DATA_OFFSETRES_RECV_OK			19
#define S_SERVER_DATA_DATA_SEND_INDOING			20
#define S_SERVER_DATA_DATARES_RECV_INDOING		21
#define S_SERVER_DATA_DATARES_RECV_OK			22

#define S_CLIENT_DATA_ERROR						23
#define S_CLIENT_DATA_OK						24
#define S_SERVER_DATA_ERROR						25	//��ʾ������Ϊ�������ӣ���������ʧ��
#define S_SERVER_DATA_OK						26

#define TASK_NORMAL						0	//��ʾ��ͨ���񣬰��������������
#define TASK_DATA						1	//��ʾ��������
#define TASK_UNDEFINE					2	//��ʾ����δ����

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
		int clientSock;					//��ȡ�ͻ������ӵ�socket
		int serverSock;					//��Զ�˷��������ӵ�socket

		int startState;					//�ͻ���һ��ʼ����ʱ��״̬
		int taskType;					//��������: normal/data
		int clientState;				//�ͻ���ʶ��������״̬
		int serverState;				//������ʶ��������״̬

		char serverIP[16];				//Զ�˷�����IP��ַ
		unsigned short int serverPort;	//Զ�˷�����Port�������ֽ���

		char clientConnBuf[4096];		//��ȡ�ͻ��˷��ͱ��ĵĻ�����
		int  clientConnBufLen;			//��ȡ�ͻ��˷��ͱ��ĵĳ���
		bool isFirstDecode;				//�Ƿ�Ϊ��һ�ν����ͻ��˷��͵ı���

		char cacheHome[300];			//����Ŀ¼

		int  clientPktCacheFd;						//���տͻ������ݱ��ĵĻ����ļ���������ֻдģʽ
		int  clientPktCacheReadFd;					//��ȡ���ļ�����������ֻ��ģʽ����Server�˷�������ʹ��
		unsigned long long int clientPktCacheLen;	//���տͻ������ݱ��ĵ��ļ�����
		int  serverPktCacheFd;						//���շ��������ݱ��ĵĻ����ļ���������ֻдģʽ
		int  serverPktCacheReadFd;					//��ȡ���ļ�����������ֻ��ģʽ����Client�˷�������ʹ��
		unsigned long long int serverPktCacheLen;	//���շ��������ݱ��ĵ��ļ�����

		unsigned long long int serverPktSendLen;	//�Ѿ����͸��������˵��ֽ���
		unsigned long long int clientPktSendLen;	//�Ѿ����͸��ͻ��˵��ֽ���

		bool isSlave;								//�Ƿ�Ϊ����Worker
		void *pSlave;								//����Workerָ�룬�������÷�������epoll�¼�����
		void *pMaster;								//��Workerָ��

		char serverSendBuf[102400];					//���͸��������Ļ�����
		char serverRecvBuf[2048];					//���շ�������Ӧ�Ļ�����
		char clientSendBuf[2048];					//���͸��ͻ��˵Ļ�����
		char clientRecvBuf[102400];					//���տͻ��˷������ݵĻ�����

		unsigned long long int fileSize;			//�ͻ��˷����ļ��ĳ���
		unsigned long long int fileOffset;			//�ͻ��˷����ļ���ƫ��
		unsigned long long int fileRecvLen;			//�Ѿ����յ��ͻ��˷��͹��������ݳ���
		unsigned long long int fileSendLen;			//�Ѿ����͵������������ݳ���

		long long int uin;							//�ͻ��˵�QQ����
		int seq;									//�ͻ�����������˽������ݽ���ʱ��Seq

		time_t clientConnTime;						//�ͻ������ӵ����ʱ��
		time_t serverConnTime;						//���������ӵ����ʱ��

		int  clientFileCacheCurrentWriteFd;				//����ͻ������ݵ��ļ�д�����������ڽ��տͻ������ݲ�д�뻺��
		int  clientFileCacheCurrentWriteIndex;			//����ͻ������ݵ��ļ���д��λ
		unsigned int clientFileCacheCurrentWriteOffset;	//����ͻ������ݵ��ļ���дƫ��
		int  clientFileCacheCurrentReadFd;				//����ͻ������ݵ��ļ��������������ڶ�ȡ�ͻ��˻������ݲ�����
		int  clientFileCacheCurrentReadIndex;			//����ͻ������ݵ��ļ��Ķ���λ
		unsigned int clientFileCacheCurrentReadOffset;	//����ͻ������ݵ��ļ��Ķ�ƫ��
};

#endif 
