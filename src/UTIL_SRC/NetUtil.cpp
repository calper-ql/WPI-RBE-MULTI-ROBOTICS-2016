#include "NetUtil.hpp"

int NetUtil::getClientSocket(const char* ip){
	int sockfd = 0;
	struct sockaddr_in serv_addr;
	memset(&serv_addr, '0', sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);

	if(inet_pton(AF_INET, ip, &serv_addr.sin_addr)<=0){
		std::cout << "inet_pton error occured\n";
		return -1;
	}

	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		std::cout << "Error : Could not create socket \n";
		return -1;
	}

	if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
		std::cout << "Error : Connect Failed \n";
		return -1;
	}
	return sockfd;
}

int NetUtil::getServerSocket(){
	int listenfd = 0;
	struct sockaddr_in serv_addr;

	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (listenfd < 0) {
		std::cout << "ERROR opening socket\n";
		return -1;
	}

  const int       optVal = 1;
  const socklen_t optLen = sizeof(optVal);
  setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (void*) &optVal, optLen);

	memset(&serv_addr, '0', sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(PORT);
	if(bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0){
		std::cout << "Error could not bind\n";
		return -1;
	}
	listen(listenfd, 10);
	return listenfd;
}

std::string NetUtil::readFromSocket(int connfd){
	char recieve_char;
	std::string receive_buffer;
	std::chrono::time_point<std::chrono::system_clock> start, end;
	start = std::chrono::system_clock::now();
	while(read(connfd, &recieve_char, sizeof(char)) > 0){
		end = std::chrono::system_clock::now();
		std::chrono::duration<double> elapsed_seconds = end-start;
		if(elapsed_seconds.count() > 1.0){
			receive_buffer.clear();
			break;
		}
		start = std::chrono::system_clock::now();
		if(recieve_char == '\n') break;
		receive_buffer.push_back(recieve_char);
	}
	return receive_buffer;
}
