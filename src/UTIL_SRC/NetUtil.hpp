#ifndef NETUTIL_HPP
#define NETUTIL_HPP

#include <iostream>
#include <string>
#include <chrono>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>

#define PORT 30123

class NetUtil {
public:
	static int getClientSocket(const char* ip);
	static int getServerSocket();
	static std::string readFromSocket(int connfd);
};

#endif
