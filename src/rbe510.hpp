#ifndef RBE510_HPP
#define RBE510_HPP

#include <iostream>
#include <sstream>

class Entity {
private:
	// location, orientation and id
	int _id;
	float _x;
	float _y;
	float _theta;
	float _height;
	float _width;
	bool _is_on_field;
	void init();

public:
	Entity();
	Entity(int id);
	virtual ~Entity();

	// Getter functions
	int id();
	float x();
	float y();
	float theta();
	float width();
	float height();
	bool isOnField();

	// Setter functions
	void set_x(float value);
	void set_y(float value);
	void set_theta(float value);
	void set_height(float value);
	void set_width(float value);
	void set_field_status(bool value);

	// Print
	virtual std::string toStr();
	virtual std::string getStrData();
};

class Robot : public Entity {
private:

public:
	Robot(int id);
	virtual ~Robot();
	std::string toStr();
};


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
#include <string.h>

#define PORT 30123

class NetUtil {
public:
	static int getClientSocket(const char* ip);
	static int getServerSocket();
	static std::string readFromSocket(int connfd);
};


#include <sstream>
#include <iostream>
#include <vector>

struct FieldData {
	std::vector<Robot> robots;
	std::vector<Entity> entities;
};

class FieldComputer {
	bool verbose;
	std::string ip;
public:
	FieldComputer(std::string ip);
	FieldData getFieldData();
	void arcadeDrive(int id, float speed, float sway);
  void openGripper(int id);
  void closeGripper(int id);
	void enableVerbose();
	void disableVerbose();
};


Entity::Entity(){
	_id = -1;
	init();
}

Entity::Entity(int id){
	this->_id = id;
	init();
}

void Entity::init(){
	set_x(0.0);
	set_y(0.0);
	set_theta(0.0);
	set_height(0.0);
	set_width(0.0);
	set_field_status(false);
}

Entity::~Entity(){}

// Getter functions
int Entity::id(){
	return _id;
}

float Entity::x(){
	return _x;
}

float Entity::y(){
	return _y;
}

float Entity::theta(){
	return _theta;
}

float Entity::width(){
	return _width;
}

float Entity::height(){
	return _height;
}

bool Entity::isOnField(){
	return _is_on_field;
}

// Setter functions
void Entity::set_x(float value){
	_x = value;
}

void Entity::set_y(float value){
	_y = value;
}

void Entity::set_theta(float value){
	_theta = value;
}

void Entity::set_width(float value){
	_width = value;
}

void Entity::set_height(float value){
	_height = value;
}

void Entity::set_field_status(bool value){
	_is_on_field = value;
}

// Print
std::string Entity::toStr(){
	std::ostringstream strStream;
	strStream << "Entity ";
	strStream << getStrData();
	return strStream.str();
}

std::string Entity::getStrData(){
	std::ostringstream strStream;
	strStream << id() << " ";
	strStream << x() << " ";
	strStream << y() << " ";
	strStream << theta() << " ";
	strStream << width() << " ";
	strStream << height() << " ";
	return strStream.str();
}

FieldComputer::FieldComputer(std::string ip){
	this->ip = ip;
	disableVerbose();
}

FieldData FieldComputer::getFieldData(){
	if(verbose)std::cout << "Requested field update" << std::endl;
	FieldData data;

	int sockfd = NetUtil::getClientSocket(ip.c_str());
	if (sockfd < 0) {
		if(verbose)std::cout << "FieldComputer could not be reached..." << std::endl;
		return data;
	}

	std::string send_buffer = "UPDATE\n";
	write(sockfd, send_buffer.c_str(), send_buffer.size());

	char recieve_char;
	std::string receive_buffer;
	while(true){
		receive_buffer = NetUtil::readFromSocket(sockfd);
		if (receive_buffer.empty()) break;
		std::stringstream parser(receive_buffer);
		std::string type;
		int id;
		float x;
		float y;
		float theta;
		float width;
		float height;
		parser >> type >> id >> x >> y >> theta >> width >> height;
		if(!parser.fail()){
			if(type.compare("Robot") == 0){
				Robot r(id);
				r.set_x(x);
				r.set_y(y);
				r.set_theta(theta);
				r.set_width(width);
				r.set_height(height);
				data.robots.push_back(r);
			} else if(type.compare("Entity") == 0){
				Entity e(id);
				e.set_x(x);
				e.set_y(y);
				e.set_theta(theta);
				e.set_width(width);
				e.set_height(height);
				data.entities.push_back(e);
			}
		} else {
			if(verbose)std::cout << "FieldComputer parser fail" << std::endl;
		}
	}
    close(sockfd);
	return data;
}

void FieldComputer::arcadeDrive(int id, float speed, float sway){
	int sockfd = NetUtil::getClientSocket(ip.c_str());
	if (sockfd < 0) {
		if(verbose)std::cout << "FieldComputer could not be reached..." << std::endl;
		return;
	}
	std::stringstream strStream;
	std::string send_buffer = "DRIVE\n";
	write(sockfd, send_buffer.c_str(), send_buffer.size());
	send_buffer.clear();
	strStream << id << " ";
	strStream << speed << " ";
	strStream << sway;
	send_buffer = strStream.str();
	write(sockfd, send_buffer.c_str(), send_buffer.size());
	close(sockfd);
	if(verbose)std::cout << "Requested to drive id: "
	<< id << " with speed: " << speed << " and sway: " << sway << std::endl;
}

void FieldComputer::openGripper(int id){
	int sockfd = NetUtil::getClientSocket(ip.c_str());
	if (sockfd < 0) {
		if(verbose)std::cout << "FieldComputer could not be reached..." << std::endl;
		return;
	}
	std::stringstream strStream;
	std::string send_buffer = "GRIPPER\n";
	write(sockfd, send_buffer.c_str(), send_buffer.size());
	send_buffer.clear();
	strStream << id << " ";
	strStream << "1.0";
	send_buffer = strStream.str();
	write(sockfd, send_buffer.c_str(), send_buffer.size());
	close(sockfd);
	if(verbose)std::cout << "Requested to open gripper id: "
	<< id << std::endl;
}

void FieldComputer::closeGripper(int id){
	int sockfd = NetUtil::getClientSocket(ip.c_str());
	if (sockfd < 0) {
		if(verbose)std::cout << "FieldComputer could not be reached..." << std::endl;
		return;
	}
	std::stringstream strStream;
	std::string send_buffer = "GRIPPER\n";
	write(sockfd, send_buffer.c_str(), send_buffer.size());
	send_buffer.clear();
	strStream << id << " ";
	strStream << "0.0";
	send_buffer = strStream.str();
	write(sockfd, send_buffer.c_str(), send_buffer.size());
	close(sockfd);
	if(verbose)std::cout << "Requested to open gripper id: "
	<< id << std::endl;
}

void FieldComputer::enableVerbose(){
	verbose = true;
}

void FieldComputer::disableVerbose(){
	verbose = false;
}

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


Robot::Robot(int id) : Entity(id){

}

Robot::~Robot(){}

// Print
std::string Robot::toStr(){
	std::ostringstream strStream;
	strStream << "Robot ";
	strStream << getStrData();
	return strStream.str();
}

#endif
