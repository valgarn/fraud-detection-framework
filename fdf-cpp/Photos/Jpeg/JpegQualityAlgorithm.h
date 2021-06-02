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

#ifndef QUALITY_LIB_H
#define QUALITY_LIB_H

#ifdef WIN32
    #ifdef BUILDING_FDF_LIB
    #define FDF_LIB __declspec(dllexport)
    #else
    #define FDF_LIB __declspec(dllimport)
    #endif
#else 
    #define FDF_LIB
#endif

#include <math.h>

#include "../../Service/FraudDetectAlgorithm.h"
#include "QualityConf.h"
#include "JpegTools.h"

using namespace std;

namespace Photos
{
  namespace Jpeg
  {
        class FDF_LIB JpegQualityAlgorithm : public FraudDetectAlgorithm
        {
        private:
            QualityConf *config;

        public:
            JpegQualityAlgorithm() : FraudDetectAlgorithm()
            {
                this->method = "QUALITY";
                this->config = new QualityConf();
            };

            JpegQualityAlgorithm(QualityConf *config) : FraudDetectAlgorithm((Configuration *)config)
            {
                this->method = "QUALITY";
                this->config = config;
            };
            double scoreInterestPointsQuality(Mat original, int quality);
            double scoreWindowsQuality(Mat original, int quality);
            virtual SuspiciousInfo *check(const char *filename);
            virtual ~JpegQualityAlgorithm()
            {
                delete this->config;
            };
        };
    } // namespace Jpeg
} // namespace Photos

#endif // QUALITY_LIB_H
