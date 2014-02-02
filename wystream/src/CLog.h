#ifndef HEADER_CLOG_H
#define HEADER_CLOG_H

#include <stdio.h>
#include <stdlib.h>

class Logger{
	public:
		Logger();
		~Logger();

	public:
		bool init(const char *cacheRoot);
		void writeLog(const char *fmt,...);

	private:
		FILE *fp;
		char filePath[255];
};

extern Logger g_Logger;

#endif

