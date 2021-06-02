/*
 * Copyright 2021 The Fraud Detection Framework Authors.  All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND< either express or implied.  See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <numeric>
#include <string>
#include <vector>
#include <chrono>
#include <ctime>
#include <bitset>
#include <time.h>
#include <dirent.h>

#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/image_processing.h>
#include <dlib/image_io.h>
#include <dlib/opencv.h>

#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>

#include "../fdf-cpp/Service/FraudDetectAlgorithm.h"
#include "../fdf-cpp/Photos/Jpeg/JpegQualityAlgorithm.h"
#include "../fdf-cpp/Photos/All/PhotosPcaAlgorithm.h"
#include "../fdf-cpp/Photos/All/CnnConf.h"
#include "../fdf-cpp/Photos/All/InfoConf.h"
#include "../fdf-cpp/Photos/All/PhotosCnnAlgorithm.h"
#include "../fdf-cpp/Photos/All/PhotosInfoAlgorithm.h"

#define MAX_FRAMES (INT_MAX / 1000)

using namespace std;
using namespace cv;
using namespace chrono;
using namespace dlib;

using namespace Photos::All;
using namespace Photos::Jpeg;

class NZN
{
    protected:
        double limitBySeconds = 50000.0;
        double processingSeconds = 0.0;
        duration<double> elapsed;
        time_point<high_resolution_clock> processingStartTime;

        PhotosInfoAlgorithm *testInfo;
        PhotosPcaAlgorithm *testPca;
        PhotosCnnAlgorithm *testCnn;
        JpegQualityAlgorithm *testQuality;

        shape_predictor predictor;

        void printStats(int step, int totalFrames, string path, std::vector<double> scoresBlur, std::vector<double> scoresPca, 
                        std::vector<double> scoresCnn, std::vector<double> scoresInterestPointsQuality, 
                        std::vector<double> scoresWindowsQuality);

    public:
        NZN();
        Mat cropFace(Mat image);
        double getMean(std::vector<double> sortedValues);
        double getMode(std::vector<double> sortedValues);
        double getMedian(std::vector<double> sortedValues);
        double getScore(std::vector<double> sortedValues, double threshold = 0.5);

        void process(string mapPath);
        void loadFaces(string filename);
        void cutFacesToMapsFile(string path);
};

