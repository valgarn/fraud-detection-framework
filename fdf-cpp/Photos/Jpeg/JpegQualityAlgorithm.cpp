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

#include <math.h>
#include <iostream>
#include <fstream>

#include "JpegQualityAlgorithm.h"

using namespace std;
using namespace Photos::Jpeg;

double JpegQualityAlgorithm::scoreInterestPointsQuality(Mat original, int quality)
{
    vector<int> params;
    vector<unsigned char> buff;
    params.push_back(CV_IMWRITE_JPEG_QUALITY);
    params.push_back(quality);
    imencode(".jpg", original, buff, params);
    Mat drawing(1, buff.size(), CV_8UC1, &buff[0]);
    drawing = imdecode(drawing, CV_LOAD_IMAGE_COLOR);
    cvtColor(original, drawing, CV_BGR2GRAY);
    blur(drawing, drawing, Size(3, 3));

    int thresh = 100;
    int maxThresh = 220;
    Mat thresholdOutput;
    Mat M;
    Mat rotated;
    Mat cropped;
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    threshold(drawing, thresholdOutput, thresh, maxThresh, THRESH_BINARY);
    findContours(thresholdOutput, contours, hierarchy, RETR_TREE, CHAIN_APPROX_TC89_KCOS, Point(0, 0));

    double maxdiff = 0;

    // Find the rotated rectangle for each contour
    RotatedRect rect;
    for (int i = 0; i < (int)contours.size(); i++)
    {
        rect = minAreaRect(Mat(contours[i]));
        if (rect.size.height > 15 && rect.size.width > 15)
        {
            float angle = rect.angle;
            Size2f rectSize = rect.size;
            if (rect.angle < -45.)
            {
                angle += 90.0;
                swap(rectSize.width, rectSize.height);
            }
            M = getRotationMatrix2D(rect.center, angle, 1.0);
            warpAffine(original, rotated, M, drawing.size(), INTER_CUBIC);
            getRectSubPix(rotated, rectSize, rect.center, cropped);
            imencode(".jpg", cropped, buff, params);
            Mat jpg(1, buff.size(), CV_8UC1, &buff[0]);
            drawing = imdecode(jpg, CV_LOAD_IMAGE_COLOR);
            double d1 = JpegTools::getDiff(cropped, drawing);
            if (maxdiff < d1)
            {
                maxdiff = d1;
            }
        }
    }
    // Normalization [0, inf) - > [0, 1); fakeBorder => 0.5
    double fakeBorder = 2.0 * this->config->getFakedJpegRealError();
    return this->config->getParanoidLevel() * (1.0 - (fakeBorder / (fakeBorder + maxdiff)));
}

double JpegQualityAlgorithm::scoreWindowsQuality(Mat original, int quality)
{
    int windowSize = 25;
    double maxdiff = 0;
    Mat cropped;
    Mat drawing;
    vector<int> params;
    vector<unsigned char> buff;
    params.push_back(CV_IMWRITE_JPEG_QUALITY);
    params.push_back(quality);

    for (int x = 0; x < original.cols; x += windowSize)
    {
        for (int y = 0; y < original.rows; y += windowSize)
        {
            Size2f rectSize = Size2f(min(windowSize, original.cols - 1 -x), min(windowSize, original.rows - 1 - y));
            Point2f center = Point2f(x - 1 + rectSize.width / 2, y - 1 + rectSize.height / 2);
            getRectSubPix(original, rectSize, center, cropped);
            imencode(".jpg", cropped, buff, params);
            Mat jpg(1, buff.size(), CV_8UC1, &buff[0]);
            drawing = imdecode(jpg, CV_LOAD_IMAGE_COLOR);
            double d1 = JpegTools::getDiff(cropped, drawing);
            if (maxdiff < d1)
            {
                maxdiff = d1;
            }
        }
    }
    // Normalization [0, inf) - > [0, 1); fakeBorder => 0.5
    double fakeBorder = 3.0 * this->config->getFakedJpegRealError();
    return this->config->getParanoidLevel() * (1.0 - (fakeBorder / (fakeBorder + maxdiff)));
}

SuspiciousInfo *JpegQualityAlgorithm::check(const char *filename)
{
    SuspiciousInfo *result = new SuspiciousInfo(this->getMethod());
    string message = "";
    try
    {
        if (JpegQualityAlgorithm::isFileExists(filename))
        {
            ifstream ifs(filename, ios::binary | ios::ate);
            // Will we use very big jpegs?
            int length = ifs.tellg();
            unsigned char *jpeg = new unsigned char[length];
            ifs.seekg(0, ios::beg);
            ifs.read((char *)jpeg, length);
            int quality = JpegTools::getQuality(jpeg, length, message);
            delete[] jpeg;

            if (quality == 0)
            {
                result->setProbability(0.5);
                result->setErrorCode(SuspiciousInfo::ERROR_QUALITY_WRONG_JPEG);
                message += " ERROR_QUALITY_WRONG_JPEG";
                result->setComments(message.c_str());
            }
            else if (quality < this->config->getFakedQuality())
            {
                char *buf = new char[1024]();
                sprintf(buf, " Bad JPEG quality %d", quality);
                message += buf;
                result->setComments(message.c_str());
                result->setProbability(0.5);
            }
            else
            {
                Mat original = imread(filename);
                vector<int> params;
                vector<unsigned char> buff;
                params.push_back(CV_IMWRITE_JPEG_QUALITY);
                params.push_back(quality);
                imencode(".jpg", original, buff, params);
                Mat drawing(1, buff.size(), CV_8UC1, &buff[0]);
                drawing = imdecode(drawing, CV_LOAD_IMAGE_COLOR);
                double d = JpegTools::getDiff(original, drawing);
                double fakeBorder = this->config->getFakedJpegRealError();
                d = 1.0 - (fakeBorder / (fakeBorder + d));
                double d1 = this->scoreInterestPointsQuality(original, quality);
                double d2 = this->scoreWindowsQuality(original, quality);
                d = max(d, max(d1, d2));
                d = this->config->getParanoidLevel() * (1.0 - (0.52 / (0.52 + d)));
                result->setProbability(d);
            }
        }
        else
        {
            result->setProbability(0.5);
            result->setErrorCode(SuspiciousInfo::ERROR_MISSING_FILE);
            message += " ERROR_MISSING_FILE";
            result->setComments(message.c_str());
        }
    }
    catch (const std::runtime_error &re)
    {
        result->setProbability(0.5);
        result->setErrorCode(SuspiciousInfo::ERROR_UNKNOWN);
        message += re.what();
        result->setComments(message.c_str());
    }
    catch (const std::exception &ex)
    {
        result->setProbability(0.5);
        result->setErrorCode(SuspiciousInfo::ERROR_UNKNOWN);
        message += ex.what();
        result->setComments(message.c_str());
    }
    catch (...)
    {
        result->setProbability(0.5);
        result->setErrorCode(SuspiciousInfo::ERROR_UNKNOWN);
        message += currentExceptionTypeName();
        result->setComments(message.c_str());
    }
    return result->generateResult();
}
