#include "ConfPara.h"
#include "Global.h"
#include "tinyxml.h"

#include <string.h>
#include <stdio.h>

ConfigurationParameter::ConfigurationParameter()
{
	memset(workRoot, 0, sizeof(workRoot));
	memset(configFilePath, 0, sizeof(configFilePath));
	memset(strProgramName, 0, sizeof(strProgramName));
	memset(listenIP, 0, sizeof(listenIP));
	memset(listenPort, 0, sizeof(listenPort));
	memset(cacheRoot, 0, sizeof(cacheRoot));
	isLogPrint = false;
}

ConfigurationParameter::~ConfigurationParameter()
{

}

bool ConfigurationParameter::readConf()
{	
	if (0 == strlen(workRoot))
		strncpy(workRoot, DEFAULTWORKROOT, sizeof(workRoot));

	if (snprintf(configFilePath, sizeof(configFilePath), "%s%s/%s", workRoot, ETCDIR, CONFIGFILE) < 0){
		printf("In %s:%d, Can't not find config file %s\n", __FILE__, __LINE__, CONFIGFILE);
		return false;
	}

	TiXmlDocument docConfigFile(configFilePath);
	bool bLoadOK = docConfigFile.LoadFile();
	if (!bLoadOK){
		printf("In %s:%d, Can't not load config file %s\n", __FILE__, __LINE__, CONFIGFILE);
		return false;
	}

  	TiXmlHandle docHandle(&docConfigFile);
  	TiXmlElement *pItemElement = NULL;

	// Get Program Name
	pItemElement = docHandle.FirstChild("XML").FirstChild("CONFIG").FirstChild("NAME").Element();
	if (NULL != pItemElement){
		if(NULL == pItemElement->Attribute("value")){
			printf("In %s:%d, Can't read program name from config file %s\n", __FILE__, __LINE__, CONFIGFILE);
			return false;
		}

    	strcpy(strProgramName, pItemElement->Attribute("value"));
  	}

	// get server listen ip addr from config file
	pItemElement = docHandle.FirstChild("XML").FirstChild("CONFIG").FirstChild("LISTEN_IP").Element();
	if (NULL != pItemElement){
		if(NULL == pItemElement->Attribute("value")){
			printf("In %s:%d, Can't read WYStream listen ip addr from config file %s\n", __FILE__, __LINE__, CONFIGFILE);
			return false;
		}

		strcpy(listenIP, pItemElement->Attribute("value"));
  	}

	// get server listen port from config file
	pItemElement = docHandle.FirstChild("XML").FirstChild("CONFIG").FirstChild("LISTEN_PORT").Element();
	if (NULL != pItemElement){
		if(NULL == pItemElement->Attribute("value")){
			printf("In %s:%d, Can't read WYStream listen port from config file %s\n", __FILE__, __LINE__, CONFIGFILE);
			return false;
		}

		strcpy(listenPort, pItemElement->Attribute("value"));
  	}

	// Get Cache ROOT
	pItemElement = docHandle.FirstChild("XML").FirstChild("CONFIG").FirstChild("CACHE_ROOT").Element();
	if (NULL != pItemElement){
		if(NULL == pItemElement->Attribute("value")){
			printf("In %s:%d, Can't read cache root from config file %s\n", __FILE__, __LINE__, CONFIGFILE);
			return false;
		}

    	strcpy(cacheRoot, pItemElement->Attribute("value"));
  	}

	// Get Log Print
	pItemElement = docHandle.FirstChild("XML").FirstChild("CONFIG").FirstChild("LOG_PRINT").Element();
	if (NULL != pItemElement){
		if(NULL == pItemElement->Attribute("value")){
			printf("In %s:%d, Can't read log print from config file %s\n", __FILE__, __LINE__, CONFIGFILE);
			return false;
		}
	
		int logPrintVal = atoi(pItemElement->Attribute("value"));
		
		if ( logPrintVal != 0)		isLogPrint = true;
		else						isLogPrint = false;	
  	}

	return true;
}

char * ConfigurationParameter::getWorkRoot()
{
	return workRoot;
}

char * ConfigurationParameter::getProgramName()
{
	return strProgramName;
}

char * ConfigurationParameter::getListenIP()
{
	return listenIP;
}

char * ConfigurationParameter::getListenPort()
{
	return listenPort;
}

char * ConfigurationParameter::getCacheRoot()
{
	return cacheRoot;
}

bool ConfigurationParameter::getIsLogPrint()
{
	return isLogPrint;
}

void ConfigurationParameter::setIsLogPrint(bool val)
{
	isLogPrint = val;
}

void ConfigurationParameter::cmdDump()
{
	printf("==========Configuration==========\n");
	printf("Version: %s\n", VERSION);
	printf("Work Root: %s\n", getWorkRoot());
	printf("Program Name: %s\n", getProgramName());	
	printf("Listen IPAddr: %s\n", getListenIP());
	printf("Listen Port: %s\n", getListenPort());
	printf("Cache Root: %s\n", getCacheRoot());
	printf("Log Print: %d\n", getIsLogPrint());
	printf("==================================\n\n");
}

