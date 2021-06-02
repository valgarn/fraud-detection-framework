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

#ifndef PCA_LIB_H
#define PCA_LIB_H

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
#include "PcaConf.h"

using namespace std;
using namespace cv;

namespace Photos
{
  namespace All
  {
        class FDF_LIB PhotosPcaAlgorithm : public FraudDetectAlgorithm
        {
        private:
                PcaConf *config;

        public:
            PhotosPcaAlgorithm() : FraudDetectAlgorithm()
            {
                this->method = "PCA";
                this->config = new PcaConf();
            };

            PhotosPcaAlgorithm(PcaConf *config) : FraudDetectAlgorithm((Configuration *)config)
            {
                this->method = "PCA";
                this->config = config;
            };
            double getScore(Mat image, const double FINE_TUNE = 0.54);
            virtual SuspiciousInfo *check(const char *filename);
            virtual ~PhotosPcaAlgorithm()
            {
                delete this->config;
            };
        };
    } // namespace All
} // namespace Photos

#endif // PCA_LIB_H
