#include "FieldComputer.hpp"

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

void FieldComputer::enableVerbose(){
	verbose = true;
}

void FieldComputer::disableVerbose(){
	verbose = false;
}
