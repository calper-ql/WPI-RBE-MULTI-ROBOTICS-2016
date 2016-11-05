#include <iostream>
#include <string>
#include "rbe510.hpp"

using namespace std;

void printFieldData(FieldComputer& fc, FieldData& data) {
	for(unsigned i = 0; i < data.robots.size(); i++){
		cout << " - Robot id: " << data.robots[i].id() << endl;
		cout << "x: " << data.robots[i].x() << endl;
		cout << "y: " << data.robots[i].y() << endl;
		cout << "theta: " << data.robots[i].theta() << endl;
		cout << "width: " << data.robots[i].width() << endl;
		cout << "height: " << data.robots[i].height() << endl;
		fc.arcadeDrive(data.robots[i].id(), 1.0, -1.0);
		sleep(1);
		fc.arcadeDrive(data.robots[i].id(), 0.0, 0.0);
	}

	for(unsigned i = 0; i < data.entities.size(); i++){
		cout << " - Entity id: " << data.entities[i].id() << endl;
		cout << "x: " << data.entities[i].x() << endl;
		cout << "y: " << data.entities[i].y() << endl;
		cout << "theta: " << data.entities[i].theta() << endl;
		cout << "width: " << data.entities[i].width() << endl;
		cout << "height: " << data.entities[i].height() << endl;
	}
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
	printFieldData(fc, data);

	vector<int> queryIds;
	if (data.robots.size() > 1) {
		queryIds.push_back(data.robots[0].id());
	}
	if (data.entities.size() > 1) {
		queryIds.push_back(data.entities[0].id());
	}
	
	if (queryIds.size() == 0) {
		// Push back some dummy data to test log output
		queryIds.push_back(0);
		queryIds.push_back(101);
		queryIds.push_back(100);
	}

	data = fc.getFieldData(queryIds);
	printFieldData(fc, data);

	return 0;
}
