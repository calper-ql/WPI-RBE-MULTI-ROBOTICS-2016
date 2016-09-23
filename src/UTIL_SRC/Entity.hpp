#ifndef ENTITY_HPP
#define ENTITY_HPP

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

#endif
