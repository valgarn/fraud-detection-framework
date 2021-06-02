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

#ifndef INFO_LIB_H
#define INFO_LIB_H

#ifdef WIN32
    #ifdef BUILDING_FDF_LIB
    #define FDF_LIB __declspec(dllexport)
    #else
    #define FDF_LIB __declspec(dllimport)
    #endif
#else 
    #define FDF_LIB
#endif


#include <vector>
#include <string>
#include <cctype>
#include <locale>
#include <functional>

#include <opencv2/opencv.hpp>

#include "../../Service/FraudDetectAlgorithm.h"
#include "InfoConf.h"

#include "../Jpeg/JpegTools.h"

using namespace std;
using namespace cv;

namespace Photos
{
  namespace All
  {
        class FDF_LIB PhotosInfoAlgorithm : public FraudDetectAlgorithm
        {
        private:
                InfoConf *config;

        public:
            PhotosInfoAlgorithm() : FraudDetectAlgorithm()
            {
                this->method = "INFO";
                this->config = new InfoConf();
            };
            PhotosInfoAlgorithm(InfoConf *config) : FraudDetectAlgorithm((Configuration *)config)
            {
                this->method = "INFO";
                this->config = config;
            };
            bool isBlur(Mat image);
            int getBlur(Mat image);
            virtual SuspiciousInfo *check(const char *filename);
            virtual ~PhotosInfoAlgorithm()
            {
                delete this->config;
            };
        };
    } // namespace All
} // namespace Photos

#endif // PCA_LIB_H
