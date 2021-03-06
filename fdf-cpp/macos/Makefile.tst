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
			../lib/libjpeg.8.dylib \
			../lib/libexif.dylib \
			../lib/libopencv_calib3d.dylib \
			../lib/libopencv_core.dylib \
			../lib/libopencv_dnn.dylib \
			../lib/libopencv_features2d.dylib \
			../lib/libopencv_flann.dylib \
			../lib/libopencv_highgui.dylib \
			../lib/libopencv_imgcodecs.dylib \
			../lib/libopencv_imgproc.dylib \
			../lib/libopencv_ml.dylib \
			../lib/libopencv_objdetect.dylib \
			../lib/libopencv_photo.dylib \
			../lib/libopencv_shape.dylib \
			../lib/libopencv_stitching.dylib \
			../lib/libopencv_superres.dylib \
			../lib/libopencv_video.dylib \
			../lib/libopencv_videoio.dylib \
			../lib/libopencv_videostab.dylib \
			../lib/libtensorflow.so
SOURCES=test/fdftest.cpp
OBJECTS=$(patsubst %.cpp, build/%.o, $(SOURCES))
EXECUTABLE=build/fdftest

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(SOURCES) 
	$(CC) $(LDFLAGS) $(SOURCES)  -march=native -v -o $@ $(LDLIBS)
