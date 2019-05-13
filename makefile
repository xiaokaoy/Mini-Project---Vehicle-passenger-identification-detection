all:
#	g++ -std=c++11 camera_with_fps.cpp -o camera_with_fps `pkg-config --cflags --libs opencv`
#	g++  camera_with_fps.cpp -Wall -std=c++11 -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_imgcodecs 
	g++  main.cpp  -o mini-prj -Wall -std=c++11 -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_imgcodecs `pkg-config --cflags --libs opencv`
camera:
	g++ -std=c++11 camera_with_fps.cpp -o camera_with_fps `pkg-config --cflags --libs opencv`
