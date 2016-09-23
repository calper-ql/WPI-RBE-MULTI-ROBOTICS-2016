#ifndef FIELDCOMPUTER_HPP
#define FIELDCOMPUTER_HPP

#include <sstream>
#include <iostream>
#include <vector>

#include "NetUtil.hpp"
#include "Entity.hpp"
#include "Robot.hpp"

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
	void enableVerbose();
	void disableVerbose();
};

#endif
