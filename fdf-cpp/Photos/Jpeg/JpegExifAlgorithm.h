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

#ifndef EXIF_LIB_H
#define EXIF_LIB_H

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

#include "../../thirdparty/jpeg-data/jpeg-data.h"
#include <libexif/exif-data.h>

#include "../../Service/FraudDetectAlgorithm.h"
#include "ExifConf.h"

using namespace std;

namespace Photos
{
  namespace Jpeg
  {
        class FDF_LIB JpegExifAlgorithm : public FraudDetectAlgorithm
        {
        private:
            ExifConf *config;
            const int BUFFER_SIZE = 1024;
            void checkTags(ExifData *ed);
            void checkImage(Mat image, SuspiciousInfo *result);
            void checkExif(const char *filename, Mat image, SuspiciousInfo *result);

        public:
            JpegExifAlgorithm() : FraudDetectAlgorithm()
            {
                this->method = "EXIF";
                this->config = new ExifConf();
            };

            JpegExifAlgorithm(ExifConf *config) : FraudDetectAlgorithm((Configuration *)config)
            {
                this->method = "EXIF";
                this->config = config;
            };
            virtual SuspiciousInfo *check(const char *filename);
            virtual ~JpegExifAlgorithm()
            {
                delete this->config;
            };
        };
    } // namespace Jpeg
} // namespace Photos

#endif // EXIF_LIB_H
