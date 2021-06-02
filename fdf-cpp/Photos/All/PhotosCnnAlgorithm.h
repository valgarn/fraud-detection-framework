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

#ifndef CNN_LIB_H
#define CNN_LIB_H

#ifdef WIN32
    #ifdef BUILDING_FDF_LIB
    #define FDF_LIB __declspec(dllexport)
    #else
    #define FDF_LIB __declspec(dllimport)
    #endif
#else 
    #define FDF_LIB
#endif

typedef float float32_t;

#include <vector>
#include <string>
#include <cctype>
#include <locale>
#include <functional>

#include <tensorflow/c_api.h>
#include <opencv2/opencv.hpp>

#include "../../Service/FraudDetectAlgorithm.h"
#include "CnnConf.h"
#include "CnnEnv.h"

using namespace std;
using namespace cv;

namespace Photos
{
  namespace All
  {
        class FDF_LIB PhotosCnnAlgorithm : public FraudDetectAlgorithm
        {
        private:
            static const int CHUNK_SIZE = 28 * 28 * 3;
            static const int INPUT_SIZE = 1000;
            CnnConf *config;
            CnnEnv *env;
            float* readImage(const char *filename, int& samplesNumber);
            double process(float32_t *values, int samplesNumber);
        public:
            PhotosCnnAlgorithm() : FraudDetectAlgorithm()
            {
                this->method = "CNN";
                this->config = new CnnConf();
                this->env = &CnnEnv::getInstance(this->config);
            };

            PhotosCnnAlgorithm(CnnConf *config) : FraudDetectAlgorithm((Configuration *)config)
            {
                this->method = "CNN";
                this->config = config;
                this->env = &CnnEnv::getInstance(this->config);
            };
            double getScore(Mat image, const double FINE_TUNE = 0.5);
            virtual SuspiciousInfo *check(const char *filename);
            virtual ~PhotosCnnAlgorithm()
            {
                delete this->config;
            };
        };
    } // namespace All
} // namespace Photos

#endif // CNN_LIB_H
