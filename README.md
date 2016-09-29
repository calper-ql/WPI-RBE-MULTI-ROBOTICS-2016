# WPI-RBE-MULTI-ROBOTICS-2016-FIELD
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

###To build:
```
mkdir build  
cd build  
cmake ..  
make  
```
