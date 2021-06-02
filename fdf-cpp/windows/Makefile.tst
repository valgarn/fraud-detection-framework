CC=g++
CFLAGS=-c -Wall
LDFLAGS=-g -I./include -I./include/libjpeg -L./lib -std=c++17 \
			-llibjpeg \
			-llibexif-12 \
			-llibopencv_calib3d342 \
			-llibopencv_core342 \
			-llibopencv_dnn342 \
			-llibopencv_features2d342 \
			-llibopencv_flann342 \
			-llibopencv_highgui342 \
			-llibopencv_imgcodecs342 \
			-llibopencv_imgproc342 \
			-llibopencv_ml342 \
			-llibopencv_objdetect342 \
			-llibopencv_photo342 \
			-llibopencv_shape342 \
			-llibopencv_stitching342 \
			-llibopencv_superres342 \
			-llibopencv_video342 \
			-llibopencv_videoio342 \
			-llibopencv_videostab342
LDLIBS=bin/fdf.dll
SOURCES=test/fdftest.cpp
OBJECTS=$(patsubst %.cpp, bin/%.o, $(SOURCES))
EXECUTABLE=bin/fdftest.exe

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(SOURCES) 
	$(CC) $(LDFLAGS) $(SOURCES) -o $@ $(LDLIBS)
