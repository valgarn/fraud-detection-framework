CC=g++
CFLAGS=-c -Wall
LDFLAGS=-g -I./include -I./include/libjpeg -L../lib -std=c++11 \
			-ljpeg \
			-lexif \
			-lopencv_calib3d \
			-lopencv_core \
			-lopencv_dnn \
			-lopencv_features2d \
			-lopencv_flann \
			-lopencv_highgui \
			-lopencv_imgcodecs \
			-lopencv_imgproc \
			-lopencv_ml \
			-lopencv_objdetect \
			-lopencv_photo \
			-lopencv_shape \
			-lopencv_stitching \
			-lopencv_superres \
			-lopencv_video \
			-lopencv_videoio \
			-lopencv_videostab \
			-ltensorflow
LDLIBS=./build/libfdf.so \
			../lib/libjpeg.so \
			../lib/libexif.so \
			../lib/libopencv_calib3d.so \
			../lib/libopencv_core.so \
			../lib/libopencv_dnn.so \
			../lib/libopencv_features2d.so \
			../lib/libopencv_flann.so \
			../lib/libopencv_highgui.so \
			../lib/libopencv_imgcodecs.so \
			../lib/libopencv_imgproc.so \
			../lib/libopencv_ml.so \
			../lib/libopencv_objdetect.so \
			../lib/libopencv_photo.so \
			../lib/libopencv_shape.so \
			../lib/libopencv_stitching.so \
			../lib/libopencv_superres.so \
			../lib/libopencv_video.so \
			../lib/libopencv_videoio.so \
			../lib/libopencv_videostab.so \
			../lib/libtensorflow.so
SOURCES=test/fdftest.cpp
OBJECTS=$(patsubst %.cpp, build/%.o, $(SOURCES))
EXECUTABLE=build/fdftest

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(SOURCES) 
	$(CC) $(LDFLAGS) $(SOURCES) -o $@ $(LDLIBS)
