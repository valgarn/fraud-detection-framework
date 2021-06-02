CC := g++
CFLAGS := -c -DBUILDING_FDF_LIB -Wall -IC:/boost_1_68_0 -IC:/boost_1_68_0/boost/python -IC:/ProgramData/Anaconda3/include \
			-I./include -I./include/libjpeg -L./lib \
			-LC:/boost_1_68_0/stage/lib -LC:/ProgramData/Anaconda3/Library/bin -LC:/ProgramData/Anaconda3 -std=c++17 \
			-lpython36 -llibboost_numpy36-mgw47-mt-x64-1_68 -llibboost_python36-mgw47-mt-x64-1_68 \
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
            -llibopencv_videostab342 \
			-ltensorflow
LDFLAGS := -shared -LC:/boost_1_68_0/stage/lib -LC:/ProgramData/Anaconda3/Library/bin -LC:/ProgramData/Anaconda3 -L./lib \
			-lpython36 -llibboost_numpy36-mgw47-mt-x64-1_68 -llibboost_python36-mgw47-mt-x64-1_68 \
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
            -llibopencv_videostab342 \
			-ltensorflow
SOURCES :=  Service/SuspiciousInfo.cpp \
            Service/FraudDetectAlgorithm.cpp \
            Service/Configuration.cpp \
            Photos/Jpeg/BenfordConf.cpp \
            Photos/Jpeg/JpegBenfordAlgorithm.cpp \
            thirdparty/jpeg-data/jpeg-data.cpp \
            thirdparty/jpeg-data/jpeg-marker.cpp \
            Photos/Jpeg/ExifConf.cpp \
            Photos/Jpeg/JpegExifAlgorithm.cpp \
            Photos/All/PcaConf.cpp \
            Photos/All/PhotosPcaAlgorithm.cpp  \
            Photos/Jpeg/JpegTools.cpp \
            Photos/Jpeg/QualityConf.cpp \
            Photos/Jpeg/JpegQualityAlgorithm.cpp \
            Photos/All/CnnConf.cpp \
            Photos/All/PhotosCnnAlgorithm.cpp \
			Python/fdf.cpp
OBJECTS := $(patsubst %.cpp, bin/%.o, ${SOURCES})
EXECUTABLE := bin/fdf.pyd

all: ${EXECUTABLE}

${EXECUTABLE}: ${OBJECTS}
	${CC} ${LDFLAGS} ${OBJECTS} -o $@

$(OBJECTS) : bin/%.o: %.cpp
	$(CC) $(CFLAGS) $< -o $@
