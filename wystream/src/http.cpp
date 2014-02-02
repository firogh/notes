#include "http.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdarg.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

int tcp_connect(char *servHost, unsigned short servPort, int &sockFd)
{
	if (servHost == NULL)
		return -1;

	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(sockaddr_in));
	
	addr.sin_addr.s_addr = inet_addr(servHost);
	if (addr.sin_addr.s_addr != INADDR_NONE){			// servHost is an IP addr
		if( ( sockFd = socket( AF_INET, SOCK_STREAM, 0 ) ) == -1 )
			return -1;

		addr.sin_family = AF_INET;
		addr.sin_port = htons(servPort);

		if( connect(sockFd, (struct sockaddr *) &addr, sizeof(struct sockaddr_in) ) == -1 ){
			close(sockFd);
			return -1;
		}
	}else{											// servHost is a String
		char   	buf[1024];
		int 	rc=0;
		struct 	hostent hostEn,*pHost;
		
		if(gethostbyname_r(servHost,&hostEn,buf,1024,&pHost, &rc))
			return -1;

		if( pHost == NULL )
			return -1;
		if( pHost->h_addrtype != AF_INET )
			return -1;

		addr.sin_family = AF_INET;
		addr.sin_port = htons(servPort);

		if( ( sockFd = socket( AF_INET, SOCK_STREAM, 0 ) ) == -1 )
			return -1;

		int i=0;
		char * p = NULL;
		bool isOK = false;
		while((p = pHost->h_addr_list[i]) != NULL)
		{			
			memcpy(&addr.sin_addr,pHost->h_addr_list[i],sizeof(struct in_addr));
			if( connect(sockFd, (struct sockaddr *) &addr, sizeof(struct sockaddr_in) ) == -1 ){
				continue;
			}else{
				isOK = true;	break;
			}

			i++;
		}

		if (!isOK){
			close(sockFd);
			return -1;
		}
	}

	return sockFd;
}

