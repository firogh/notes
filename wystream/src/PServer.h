#ifndef HEADER_PSERVER_H
#define HEADER_PSERVER_H

#include "Worker.h"

#include <sys/epoll.h>
#include <list>
using std::list;

class PServer{
public:
	PServer();
	~PServer();

public:
	bool initSocket();
	void Run(struct epoll_event* event);
	void MainLoop(struct epoll_event* event);
	void Accepter();
	void checkPeerEpollout();
	void readFun(Worker * &tpWorker);
	void writeFun(Worker * &tpWorker);
	void RemovePeerByItr(list<Worker *>::iterator &itrPrev);
	
private:
	int  socketFd;
	list <Worker *> mWorkingPeerList;
};

extern PServer g_PServer;

#endif

