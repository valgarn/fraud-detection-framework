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

#include "PhotosPcaAlgorithm.h"

using namespace std;
using namespace cv;
using namespace Photos::All;

double PhotosPcaAlgorithm::getScore(Mat image, const double FINE_TUNE)
{
    // the parameters set should be consistent
    int blockSize = this->config->getBlockSize();
    double fakeBorder = this->config->getFakeBorder();

    image.convertTo(image, CV_32FC1, 1.0 / 255.0);
    int n = (image.cols / blockSize) * (image.rows / blockSize);
    int q = blockSize * blockSize;
    Mat samples = Mat(q, n, CV_64FC1);
    for (int w = 0; w < image.cols / blockSize; w++)
    {
        for (int h = 0; h < image.rows / blockSize; h++)
        {
            for (int x = 0; x < blockSize; x++)
            {
                for (int y = 0; y < blockSize; y++)
                {
                    samples.at<double>(x + y * blockSize, h + (w * (image.rows / blockSize))) = image.at<float>(h * blockSize + y, w * blockSize + x);
                }
            }
        }
    };

    double sum = 0.0;
    PCA pca_analysis(samples, Mat(), CV_PCA_DATA_AS_ROW);
    // TODO: the length value should be adaptive
    for (int m = 0; m < q; m++)
    {
        double mean = 0.0;
        for (int s = 0; s < q; s++)
        {
            mean += abs(pca_analysis.eigenvectors.at<double>(m, s));
        }
        sum += mean / (double)q;
    }

    // Normalization [0, inf) - > [0, 1); fakeBorder => 0.5
    double d = 1.0 - (fakeBorder / (fakeBorder + sum));
    return this->config->getParanoidLevel() * (1.0 - (FINE_TUNE / (FINE_TUNE + d)));
}

SuspiciousInfo *PhotosPcaAlgorithm::check(const char *filename)
{
    SuspiciousInfo *result = new SuspiciousInfo(this->getMethod());
    try
    {
        if (PhotosPcaAlgorithm::isFileExists(filename))
        {
            result->setProbability(this->getScore(imread(filename)));
        }
        else
        {
            result->setProbability(0.5);
            result->setErrorCode(SuspiciousInfo::ERROR_MISSING_FILE);
            result->setComments("ERROR_MISSING_FILE");
        }
    }
    catch (const std::runtime_error &re)
    {
        result->setProbability(0.5);
        result->setErrorCode(SuspiciousInfo::ERROR_UNKNOWN);
        result->setComments(re.what());
    }
    catch (const std::exception &ex)
    {
        result->setProbability(0.5);
        result->setErrorCode(SuspiciousInfo::ERROR_UNKNOWN);
        result->setComments(ex.what());
    }
    catch (...)
    {
        result->setProbability(0.5);
        result->setErrorCode(SuspiciousInfo::ERROR_UNKNOWN);
        result->setComments(currentExceptionTypeName());
    }
    return result->generateResult();
}
