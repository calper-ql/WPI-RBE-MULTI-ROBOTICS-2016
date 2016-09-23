#include "Robot.hpp"

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
