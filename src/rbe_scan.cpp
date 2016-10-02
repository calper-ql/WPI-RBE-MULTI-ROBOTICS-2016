#include <iostream>
#include <string>
#include "UTIL_SRC/FieldComputer.hpp"

using namespace std;

int thetaTransform(float thetaRead) 
{
  return ((thetaRead - 270.0) >= 0.0) ? thetaRead - 270 : thetaRead - 270 + 360;
}

int main(int argc, char *argv[])
{
	if(argc != 2){
		printf("\n Usage: %s <ip of server> \n",argv[0]);
		return 1;
	}

	string ip = string(argv[1]);
	FieldComputer fc(ip);
	fc.enableVerbose();
	FieldData data = fc.getFieldData();

	for(unsigned i = 0; i < data.robots.size(); i++){
		cout << " - Robot id: " << data.robots[i].id() << endl;
		cout << "x: " << data.robots[i].x() << endl;
		cout << "y: " << data.robots[i].y() << endl;
		cout << "theta transformed: " << thetaTransform(data.robots[i].theta()) << endl;
		cout << "theta: " << data.robots[i].theta() << endl;
		cout << "width: " << data.robots[i].width() << endl;
		cout << "height: " << data.robots[i].height() << endl;
	}

	for(unsigned i = 0; i < data.entities.size(); i++){
		cout << " - Entity id: " << data.entities[i].id() << endl;
		cout << "x: " << data.entities[i].x() << endl;
		cout << "y: " << data.entities[i].y() << endl;
		cout << "theta transformed:" << thetaTransform(data.entities[i].theta()) << endl;
		cout << "theta: " << data.entities[i].theta() << endl;
		cout << "width: " << data.entities[i].width() << endl;
		cout << "height: " << data.entities[i].height() << endl;
	}

	return 0;
}
