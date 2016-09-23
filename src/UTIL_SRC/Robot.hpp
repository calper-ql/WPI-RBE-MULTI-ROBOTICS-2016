#ifndef ROBOT_HPP
#define ROBOT_HPP

#include "Entity.hpp"

class Robot : public Entity {
private:

public:
	Robot(int id);
	virtual ~Robot();
	std::string toStr();
};

#endif
