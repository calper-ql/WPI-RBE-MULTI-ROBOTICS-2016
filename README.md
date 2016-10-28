# WPI Multi-Robot Systems Labs (RBE 510)
This is a repository for the WPI Multi Robotics course's labs and fields

We use OpenCV to track ARUCO codes in the field.
To build opencv in your system please run the install script by:
```
./installopencv.sh
```

If you are using ubuntu, make sure to install all the dependencies.
To do so run:
```
sudo apt-get install build-essential  
sudo apt-get install cmake git libgtk2.0-dev pkg-config libavcodec-dev libavformat-dev libswscale-dev  
sudo apt-get install python-dev python-numpy libtbb2 libtbb-dev libjpeg-dev libpng-dev libtiff-dev libjasper-dev libdc1394-22-dev  
```

##Building

The project is built using cmake:

```
mkdir build  
cd build  
cmake ..  
make  
```

## Developing

As can be seen in the example code below, using the library is very simple. Just include *rbe510.hpp* in your project code. This will enable your code to use all of the libraries and functions available in the class library.

## Example Code

This project contains a few pieces of example code. A brief description of each is given below.

### src/g2p.cpp

This piece of code is a demonstration of how to tell a specific robot to move to a point within the camera frame. It is called from the command line as shown below:

```
Usage: g2p <ip of server> <id> <targetx> <targety>
```

When called, *g2p* will move a robot with id *id* from wherever it is within the camera frame to the coordinate *(targetx, targety)* within the camera frame. 

### src/rbe_test_client.cpp

The test client software demonstrates how to get all of the different types of information from the server. It is called from the command line as shown below:

```
Usage: rbe_test_client <ip of server>
```

When called, *rbe_test_client* will print out all of the information available about all objects tracked by the server.