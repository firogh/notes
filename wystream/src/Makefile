CXX := $(CROSS)g++
CCFLAGS = -Wall -g
	
Main:WYStream.o Util.o ConfPara.o tinyxml.o tinystr.o tinyxmlerror.o tinyxmlparser.o CLog.o PServer.o http.o msgprotobuf.pb.o Worker.o
	 $(CXX) $(CFLAGS) $(LDFLAGS) -lprotobuf -lz -o wystream WYStream.o Util.o ConfPara.o tinyxml.o tinystr.o tinyxmlerror.o tinyxmlparser.o CLog.o PServer.o http.o msgprotobuf.pb.o Worker.o -lpthread

msgprotobuf.pb.o:pb/msgprotobuf.pb.cc
	$(CXX) $(CFLAGS) -c pb/msgprotobuf.pb.cc
	
clean:
	rm -f *.o
	rm -f wystream

