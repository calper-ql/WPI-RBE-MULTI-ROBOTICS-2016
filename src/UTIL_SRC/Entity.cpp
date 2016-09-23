#include "Entity.hpp"

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
