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

#ifndef JPEGTOOLS_LIB_H
#define JPEGTOOLS_LIB_H

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
#include <vector>
#include <string>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

namespace Photos
{
  namespace Jpeg
  {
        class FDF_LIB JpegTools
        {
        private:
            static void clearVector(vector<unsigned char *> *v);

        public:
            static vector<unsigned char *> *getDQT(unsigned char *jpeg, int &pos, int size, string& message);
            static vector<unsigned char *> *decode(unsigned char *jpeg, int size, string& message);
            static double getDiff(Mat image1, Mat image2);
            static int getQuality(unsigned char *jpeg, int size, string& message);
            static string type2str(int type);
        };
    } // namespace Jpeg
} // namespace Photos

#endif // JPEGTOOLS_LIB_H
