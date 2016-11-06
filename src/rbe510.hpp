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
    Entity(const Entity& other);    
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
    Robot();
	Robot(int id);
    Robot(const Robot& other);
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
private:
	bool verbose;
	std::string ip;
private:
	FieldData recieveFieldData(int sockfd);

public:
	FieldComputer(std::string ip);
	FieldData getFieldData();
	FieldData getFieldData(std::vector<int> forIds);
	void arcadeDrive(int id, float speed, float sway);
  	void openGripper(int id);
  	void closeGripper(int id);
	void enableVerbose();
	void disableVerbose();
};



#endif
