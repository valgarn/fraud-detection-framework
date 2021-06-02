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

#include <iostream>
#include <math.h>
#include <vector>
#include <set>

#include "PhotosInfoAlgorithm.h"

typedef float float32_t;

using namespace std;
using namespace cv;
using namespace Photos::All;
using namespace Photos::Jpeg;

bool PhotosInfoAlgorithm::isBlur(Mat image)
{
    Mat imageGrey;
    cvtColor(image, imageGrey, CV_BGR2GRAY);
    resize(imageGrey, imageGrey, Size(1000, 1000), 0, 0, INTER_CUBIC);
    Mat dst2;
    image.copyTo(dst2);
    Mat laplacianImage;
    dst2.convertTo(laplacianImage, CV_8UC1);
    Laplacian(imageGrey, laplacianImage, CV_8U);
    Mat laplacianImage8bit;
    laplacianImage.convertTo(laplacianImage8bit, CV_8UC1);
    unsigned char *pixels = laplacianImage8bit.data;
    // 16777216 = 256*256*256
    int maxLap = -16777216;
    for (unsigned long i = 0; i < (laplacianImage8bit.elemSize()*laplacianImage8bit.total()); i++) {
        if (pixels[i] > maxLap) {
            maxLap = pixels[i];
        }
    }
    int threshold = 69;
    return (maxLap <= threshold);
}

int PhotosInfoAlgorithm::getBlur(Mat image)
{
    Mat imageGrey;
    cvtColor(image, imageGrey, CV_BGR2GRAY);
    Mat dst2;
    image.copyTo(dst2);
    Mat laplacianImage;
    dst2.convertTo(laplacianImage, CV_8UC1);
    Laplacian(imageGrey, laplacianImage, CV_8U);
    Mat laplacianImage8bit;
    laplacianImage.convertTo(laplacianImage8bit, CV_8UC1);
    unsigned char *pixels = laplacianImage8bit.data;
    // 16777216 = 256*256*256
    int maxLap = -16777216;
    for (unsigned long i = 0; i < (laplacianImage8bit.elemSize()*laplacianImage8bit.total()); i++) {
        if (pixels[i] > maxLap) {
            maxLap = pixels[i];
        }
    }
    return maxLap;
}

SuspiciousInfo *PhotosInfoAlgorithm::check(const char *filename)
{
    SuspiciousInfo *result = new SuspiciousInfo(this->getMethod());
    result->setProbability(0.0);
    try
    {
        if (PhotosInfoAlgorithm::isFileExists(filename))
        {
            ifstream ifs(filename, ios::binary | ios::ate);
            // Will we use very big jpegs?
            int length = ifs.tellg();
            unsigned char *jpeg = new unsigned char[length];
            ifs.seekg(0, ios::beg);
            ifs.read((char *)jpeg, length);
            string message = "";
            int quality = JpegTools::getQuality(jpeg, length, message);
            delete[] jpeg;
            bool isJpeg = quality > 0;
            Mat image = imread(filename, CV_LOAD_IMAGE_COLOR);
            bool isBlur = this->isBlur(image);
            ostringstream json;
            string s = filename;
            string::size_type i = s.find_last_of(PATH_SEPARATOR);
            if (i != string::npos)
                s = s.substr(i+1);
            json << "{ \"Filename\": \"" << (s.length() < 49 ? s : s.substr(44)) << "\", \"Version\": \"" << this->config->VERSION
                 << "\", \"isBlur\": " << (isBlur ? "true" : "false") << ", \"isJpeg\": " << (isJpeg ? "true" : "false") << ", \"Quality\": "
                << quality << ", \"Width\": " << image.cols << ", \"Height\": " << image.rows << ", \"Channels\": " << image.channels() << "}";
            result->setComments(json.str().c_str());
        }
        else
        {
            result->setErrorCode(SuspiciousInfo::ERROR_MISSING_FILE);
            result->setComments("ERROR_MISSING_FILE");
        }
    }
    catch (const std::runtime_error &re)
    {
        std::exception_ptr p = std::current_exception();
        result->setErrorCode(SuspiciousInfo::ERROR_UNKNOWN);
        result->setComments(re.what());
    }
    catch (const std::exception &ex)
    {
        std::exception_ptr p = std::current_exception();
        result->setErrorCode(SuspiciousInfo::ERROR_UNKNOWN);
        result->setComments(ex.what());
    }
    catch (...)
    {
        result->setProbability(0.0);
        result->setErrorCode(SuspiciousInfo::ERROR_UNKNOWN);
        result->setComments(currentExceptionTypeName());
    }
    return result->generateResult();
}
