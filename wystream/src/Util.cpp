#include "Util.h"
#include "Global.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <dirent.h>
#include <time.h>

void Random_init()
{
	unsigned long seed;
	struct timeval tv; 
	gettimeofday(&tv,(struct timezone*) 0);
	seed = tv.tv_usec + tv.tv_sec + getpid();
	srandom(seed);
}

void sig_catch(int sig_no){
	gExitFlag = 1;
}

int CreateDir(const char *sPathName){
	char DirName[256];    

	strcpy(DirName, sPathName);    
	int i, len = strlen(DirName);    
	if(DirName[len-1]!='/')    
		strcat(DirName, "/");    

	len = strlen(DirName);    

	for(i=1; i<len; i++){
		if(DirName[i]=='/') {
			DirName[i] = 0;    
			if( access(DirName, NULL)!=0 ){
				if(mkdir(DirName, 0755)==-1){     
					return -1;
				}    
			}    

			DirName[i] = '/';    
		}    
	}    

	return 0;    
} 

int SetMaxFd(void)
{
	struct rlimit rl;
	rl.rlim_cur = 50000;
	rl.rlim_max = 50000;
	if (setrlimit(RLIMIT_NOFILE, &rl) < 0){
		return -1;
	}
	
	return 0;
}

int setNonblock(int sockFd)
{
	int opts;

	opts = fcntl(sockFd, F_GETFL, 0);
	if (opts < 0) {
		return opts;
	}
	
	opts = opts | O_NONBLOCK;
	if ((opts = fcntl(sockFd, F_SETFL, opts)) < 0) {
		return opts;
	}

	return opts;
} 

int disableLinger(int sockfd)
{
	struct linger ling = {0, 0};

	if (setsockopt(sockfd, SOL_SOCKET, SO_LINGER, &ling, sizeof(ling)) < 0) {
		return -1;
	}

	return 0;
} 

int enableReuseaddr(int sockFd)
{
	int val = 1;
	if (setsockopt(sockFd, SOL_SOCKET, SO_REUSEADDR, (const void*)&val, sizeof(val)) < 0) {
		return -1;
 	}
	
	return 0;
} 

void get_file_path(const char *path, const char *file_name,  char *file_path)
{
	strcpy(file_path, path);
	if(file_path[strlen(path) - 1] != '/')
		strcat(file_path, "/");
	strcat(file_path, file_name);
}

void getGMTTime(char *timeBuf)
{
	const char *wDay[] = {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
	const char *wMonth[] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};

	time_t timep; 
	struct tm *p; 
	time(&timep); 
	p = localtime(&timep); /*取得当地时间*/
	
 	sprintf(timeBuf, "%s, %d %s %d %d:%d:%d GMT",wDay[p->tm_wday],p->tm_mday,wMonth[p->tm_mon],(1900 + p->tm_year),p->tm_hour, p->tm_min, p->tm_sec);
}

bool is_dir(const char *path)
{
	struct stat statbuf;
	if(lstat(path, &statbuf) ==0)//lstat返回文件的信息，文件信息存放在stat结构中
	{
		return S_ISDIR(statbuf.st_mode) != 0;//S_ISDIR宏，判断文件类型是否为目录
	}
	return false;
}

bool is_special_dir(const char *path)
{
	return strcmp(path, ".") == 0 || strcmp(path, "..") == 0;
}

bool is_file(const char *path)
{
	struct stat statbuf;
	if(lstat(path, &statbuf) ==0)
		return S_ISREG(statbuf.st_mode) != 0;//判断文件是否为常规文件
	return false;
}

int deleteFileByFolder(char * folderPath)
{
	DIR *dir;
	dirent *dir_info;
	char file_path[256];

	if(is_dir(folderPath)){
		if((dir = opendir(folderPath)) == NULL)
			return -1;

		while((dir_info = readdir(dir)) != NULL){
            get_file_path(folderPath, dir_info->d_name, file_path);
            if(is_special_dir(dir_info->d_name))
                continue;

			if(is_dir(file_path)){
				deleteFileByFolder(file_path);
				rmdir(file_path);
			}
			else if(is_file(file_path))
				remove(file_path);
        }

		closedir(dir);//关闭目录
	}

	rmdir(folderPath);
	
	return 0;
}

