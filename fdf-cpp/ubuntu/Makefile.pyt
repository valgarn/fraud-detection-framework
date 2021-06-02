CC := g++
CFLAGS := -c -fPIC -DBUILDING_FDF_LIB -Wall -I/home/valgarn/boost_1_68_0/ -I/home/valgarn/boost_1_68_0/boost/python -I/opt/anaconda3/include \
			-I./include -I./include/libjpeg -L../lib \
			-L/home/valgarn/boost_1_68_0/stage/lib -L/opt/anaconda3/lib -L/opt/anaconda3 -std=c++11 \
			-lpython3.6m -lboost_numpy36 -lboost_python36 \
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
LDFLAGS := -shared -fPIC -Wl,--unresolved-symbols=report-all -Wl,--warn-unresolved-symbols -std=c++11 \
            -L../lib -L/home/valgarn/boost_1_68_0/stage/lib -L/opt/anaconda3/lib -L/opt/anaconda3 \
			-lpython3.6m -lboost_numpy36 -lboost_python36 -ljpeg \
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
            Photos/All/CnnEnv.cpp \
            Photos/All/PhotosCnnAlgorithm.cpp \
            Photos/All/InfoConf.cpp \
            Photos/All/PhotosInfoAlgorithm.cpp \
			Python/fdf.cpp
OBJECTS := $(patsubst %.cpp, build/%.o, ${SOURCES})
EXECUTABLE := build/fdf.so

all: ${EXECUTABLE}

${EXECUTABLE}: ${OBJECTS}
	${CC} ${OBJECTS} -o $@ ${LDFLAGS}

$(OBJECTS) : build/%.o: %.cpp
	$(CC) $(CFLAGS) $< -o $@
