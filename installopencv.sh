#!/bin/bash

# If you are running ubuntu, make sure you have all the dependencies
# sudo apt-get install build-essential
# sudo apt-get install cmake git libgtk2.0-dev pkg-config libavcodec-dev libavformat-dev libswscale-dev
# sudo apt-get install python-dev python-numpy libtbb2 libtbb-dev libjpeg-dev libpng-dev libtiff-dev libjasper-dev libdc1394-22-dev

mkdir OpenCVwithContrib
cd OpenCVwithContrib

git clone https://github.com/opencv/opencv_contrib
git clone https://github.com/opencv/opencv

cd opencv
mkdir build
cd build
cmake -DOPENCV_EXTRA_MODULES_PATH=../../opencv_contrib/modules ..
make -j$(nproc)
sudo make install
