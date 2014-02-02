#include "CLog.h"

#include <strings.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

Logger g_Logger;

Logger::Logger()
{
	fp = NULL;
	bzero(filePath, sizeof(filePath));
}

Logger::~Logger()
{
	if (NULL != fp){
		fclose(fp);
		fp = NULL;
	}
}

bool Logger::init(const char *cacheRoot)
{
	strcat(filePath, cacheRoot);
	if (filePath[strlen(filePath) - 1] != '/')
		strcat(filePath, "/");
	strcat(filePath, "wy_stream_log.txt");

	// if file exist, when file is too large, we delete it.
	struct stat buf;
    if (stat(filePath, &buf) >= 0){
		if (buf.st_size >= 5*1024*1024)		// log file large than 5MB, we delete it
			remove(filePath);
	}
	
	if((fp = fopen(filePath, "ab+")) == NULL){
		return false;
	}

	return true;
}

void Logger::writeLog(const char *fmt,...)
{
	va_list ap;
	char buf[2048] = {0};

	va_start(ap, fmt);
	vsprintf(buf, fmt, ap);

	time_t timep;
	time(&timep);
	char strTime[40] = {0};
	strcpy(strTime, ctime(&timep));
	strTime[strlen(strTime) - 1] = '\0';

	fputs("[", fp);
	fputs(strTime, fp);
	fputs("] ", fp);
	fputs(buf, fp);
	fputs("\n", fp);

	fflush(fp);
}

