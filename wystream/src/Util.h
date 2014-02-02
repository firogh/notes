#ifndef HEADER_UTIL_H
#define HEADER_UTIL_H

#include <errno.h>

void sig_catch(int sig_no);
void Random_init();
int  CreateDir(const char *sPathName);
int  SetMaxFd(void);
int  setNonblock(int sockFd);
int  disableLinger(int sockfd);
int  enableReuseaddr(int sockFd);
void get_file_path(const char *path, const char *file_name,  char *file_path);
bool is_special_dir(const char *path);
bool is_file(const char *path);
bool is_dir(const char *path);
int  deleteFileByFolder(char * folderPath);
void getGMTTime(char *timeBuf);

#endif 

