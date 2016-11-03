#include <iostream>
#include <sstream>
#include <thread>
#include <mutex>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/aruco.hpp>
#include <sys/socket.h>

#include "rbe510.hpp"

#include <fcntl.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

using namespace std;
using namespace cv;

#define ENTITY_STARTING_INDEX 100
#define PI 3.1415926535

/* Mutex for the data sanity */
std::mutex mtx;
std::mutex broadcast_mtx;

vector<Robot> robots;
vector<Entity> entities;

class BT {
public:
	vector<int> files;
	vector<string> message_queue;

	BT(){
		thread update_thread(&BT::thread_program, this);
		update_thread.detach();
	}

	void open(string port){
		int fd = ::open(port.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
		if(fd == -1){
			cout << "Unable to open " << port << endl;
		}else{
			files.push_back(fd);
			set_interface_attribs(fd, B115200, 0);
			set_blocking(fd, 0);
		}
	}

	void broadcastToAll(string message){
		for(unsigned i = 0; i < files.size(); i++){
			write(files[i], message.c_str(), message.size());
		}
	}

	void queue(string message){
		broadcast_mtx.lock();
		message_queue.push_back(message);
		broadcast_mtx.unlock();
	}

	void thread_program(){
		while(1){
			if (!message_queue.empty()) {
				broadcast_mtx.lock();
				broadcastToAll(message_queue.front());
				message_queue.erase(message_queue.begin());
				broadcast_mtx.unlock();
			}
		}
	}

	int set_interface_attribs (int fd, int speed, int parity){
        struct termios tty;
        memset (&tty, 0, sizeof tty);
        cfsetospeed (&tty, speed);
        cfsetispeed (&tty, speed);
        tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
        // disable IGNBRK for mismatched speed tests; otherwise receive break
        // as \000 chars
        tty.c_iflag &= ~IGNBRK;         // disable break processing
        tty.c_lflag = 0;                // no signaling chars, no echo,
                                        // no canonical processing
        tty.c_oflag = 0;                // no remapping, no delays
        tty.c_cc[VMIN]  = 0;            // read doesn't block
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout
        tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl
        tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
                                        // enable reading
        tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
        tty.c_cflag |= parity;
        tty.c_cflag &= ~CSTOPB;
        tty.c_cflag &= ~CRTSCTS;
        return 0;
	}

	void set_blocking (int fd, int should_block){
        struct termios tty;
        memset (&tty, 0, sizeof tty);
        tty.c_cc[VMIN]  = should_block ? 1 : 0;
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout
	}
};

BT blue;

/* Finds distance between two points */
float dist2pf(Point2f a, Point2f b){ return sqrt(pow(a.x - b.x,2.0) + pow(a.y -  b.y,2.0)); }

/* Finds the middle point between two points */
Point2f midPoint(Point2f a, Point2f b){ return Point2f((a.x + b.x)/2.0,(a.y + b.y)/2.0); }

/* Finds the center of a square */
Point2f findSquareCenter(vector<Point2f> square){
	Point2f mid1 = midPoint(square[0], square[1]);
	Point2f mid3 = midPoint(square[2], square[3]);
	return midPoint(mid1, mid3);
}

/* Runs the Aruco program */
void runAruco(Mat image, vector<int> &markerIds, vector< vector<Point2f> > &markerCorners){
	if(image.empty()) return;
	vector<vector<Point2f> > rejectedCandidates;
	cv::Ptr<aruco::DetectorParameters> parameters = aruco::DetectorParameters::create();
	cv::Ptr<aruco::Dictionary> dictionary = aruco::getPredefinedDictionary(aruco::DICT_ARUCO_ORIGINAL);
	aruco::detectMarkers(image, dictionary, markerCorners, markerIds, parameters, rejectedCandidates);
}

/* Attends to the client on the network */
void attendClient(int connfd, vector<Robot> robots, vector<Entity> entities){
	socklen_t len;
	struct sockaddr_storage addr;
	char ipstr[INET6_ADDRSTRLEN];
	int port;

	len = sizeof addr;
	getpeername(connfd, (struct sockaddr*)&addr, &len);

	// deal with both IPv4 and IPv6:
	if (addr.ss_family == AF_INET) {
		struct sockaddr_in *s = (struct sockaddr_in *)&addr;
		port = ntohs(s->sin_port);
		inet_ntop(AF_INET, &s->sin_addr, ipstr, sizeof ipstr);
	} else { // AF_INET6
		struct sockaddr_in6 *s = (struct sockaddr_in6 *)&addr;
		port = ntohs(s->sin6_port);
		inet_ntop(AF_INET6, &s->sin6_addr, ipstr, sizeof ipstr);
	}

	string receive_buffer = NetUtil::readFromSocket(connfd);

	if(receive_buffer.compare("DRIVE") == 0){
		receive_buffer = NetUtil::readFromSocket(connfd);
		stringstream parser(receive_buffer);
		int id;
		float speed;
		float sway;
		parser >> id >> speed >> sway;
		if (!parser.fail()){
			cout << ipstr << ":" << port << " requested to drive"
			<< " id: " << id
			<< " speed: " << speed
			<< " sway: " << sway
			<< endl;
		}
		stringstream deconstructor;
		deconstructor << id << " " << speed << " " << sway << "\n";
		blue.queue(deconstructor.str());
	} else if(receive_buffer.compare("UPDATE") == 0){
		cout << ipstr << ":" << port << " requested update" << endl;
		string send_buffer;
		for(unsigned i = 0; i < robots.size(); i++){
			send_buffer.append(robots[i].toStr());
			send_buffer.append("\n");
		}
		for(unsigned i = 0; i < entities.size(); i++){
			send_buffer.append(entities[i].toStr());
			send_buffer.append("\n");
		}
		write(connfd, send_buffer.c_str(), send_buffer.size());
	}else if(receive_buffer.compare("GRIPPER") == 0){
		receive_buffer = NetUtil::readFromSocket(connfd);
		stringstream parser(receive_buffer);
		int id;
		float open;
		parser >> id >> open;
		if (!parser.fail()){
			cout << ipstr << ":" << port << " requested to adjust gripper"
			<< " id: " << id
			<< " open: " << open
			<< endl;
		}
		stringstream deconstructor;
		deconstructor << id << " - " << open << "\n";
		blue.queue(deconstructor.str());
	}
	close(connfd);
}

/* Network thread running seperately from the detector */
void network(vector<Robot> &robots, vector<Entity> &entities){
	int listenfd = NetUtil::getServerSocket();
	if(listenfd < 0) {
		cout << "Error: Connection problem" << endl;
		exit(1);
	}
	cout << "Network thread working. Now accepting client connections" << endl;
	while(1){
		int connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);
		mtx.lock();
		std::thread pic(attendClient, connfd, robots, entities);
		pic.detach();
		mtx.unlock();
	}
}

/* Updates the database */
void update( vector<int> markerIds, vector< vector<Point2f> > markerCorners) {
	mtx.lock();
	robots.clear();
	entities.clear();
	for(unsigned i = 0; i < markerIds.size(); i++){
		Point2f center = findSquareCenter(markerCorners[i]);
		Point2f mp = midPoint(markerCorners[i][0], markerCorners[i][1]);
		Point2f mp2 = midPoint(markerCorners[i][1], markerCorners[i][2]);
		float theta = atan2(center.y - mp.y, center.x - mp.x);
		theta = theta * 180.0 / PI;
		theta = 180 - theta;
		if(markerIds[i] < ENTITY_STARTING_INDEX && markerIds[i] >= 0){
			Robot robot(markerIds[i]);
			robot.set_x(center.x);
			robot.set_y(center.y);
			robot.set_theta(theta);
			robot.set_width(2.0 * dist2pf(mp, center));
			robot.set_height(2.0 * dist2pf(mp2, center));
			robots.push_back(robot);
		} else if(markerIds[i] >= ENTITY_STARTING_INDEX ){
			Entity entity(markerIds[i]);
			entity.set_x(center.x);
			entity.set_y(center.y);
			entity.set_theta(theta);
			entity.set_width(2.0 * dist2pf(mp, center));
			entity.set_height(2.0 * dist2pf(mp2, center));
			entities.push_back(entity);
		}
	}
	mtx.unlock();
}

int main(int argc, char* argv[]){
    if(argc < 2) return -1;
	for(int i = 2; i < argc; i++){
		blue.open(string(argv[i]));
	}

    int camera_index = atoi(argv[1]);

	cout << "Launching network thread" << endl;
	thread network_thread(network, std::ref(robots), std::ref(entities));
	cout << "Detaching thread" << endl;
	network_thread.detach();

	cout << "Opening Camera" << endl;
	VideoCapture cap(camera_index);
	if(!cap.isOpened()){
		cout << "Error: Could not open camera" << endl;
		return 1;
	}

	cap.set(CV_CAP_PROP_FRAME_HEIGHT, 720);
	cap.set(CV_CAP_PROP_FRAME_WIDTH, 1280);

	cout << "Camera opened" << endl;
	while(true){
		Mat inputImage;
		cap >> inputImage;
		if(!inputImage.empty()){
			vector<int> markerIds;
			vector< vector<Point2f> > markerCorners;
			runAruco(inputImage, markerIds, markerCorners);
			thread update_thread(update, markerIds, markerCorners);
			update_thread.detach();
			aruco::drawDetectedMarkers(inputImage, markerCorners, markerIds);
			imshow("Tracking Vision", inputImage);
			if(waitKey(1) == 27){break;}
		}
	}
	return 0;
}
