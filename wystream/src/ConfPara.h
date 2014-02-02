#ifndef HEADER_CONF_PARA_H
#define HEADER_CONF_PARA_H


class ConfigurationParameter{
public:
	ConfigurationParameter();
	~ConfigurationParameter();

public:
	bool   readConf();
	void   cmdDump();
	char * getWorkRoot();
	char * getProgramName();
	char * getListenIP();
	char * getListenPort();
	char * getCacheRoot();
	bool   getIsLogPrint();
	void   setIsLogPrint(bool val);
	
private:
	char workRoot[40];
	char configFilePath[255];
	char strProgramName[20];
	char listenIP[16];
	char listenPort[6];
	char cacheRoot[255];
	bool isLogPrint;
};

extern ConfigurationParameter g_CP;

#endif 

