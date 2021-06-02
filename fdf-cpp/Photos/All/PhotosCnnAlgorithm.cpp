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

#include "PhotosCnnAlgorithm.h"

using namespace std;
using namespace cv;
using namespace Photos::All;

static void TF_Deallocator(void *data, size_t length, void *arg)
{
    //free(data);
    //*reinterpret_cast<bool*>(arg) = true;
}

float32_t *PhotosCnnAlgorithm::readImage(const char *filename, int &samplesNumber)
{
    Mat image = imread(filename, CV_LOAD_IMAGE_COLOR);
    samplesNumber = ((image.cols) / this->config->blockSize + 1) * ((image.rows) / this->config->blockSize + 1);
    float32_t *values = (float32_t *)malloc(samplesNumber * CHUNK_SIZE * sizeof(float32_t));
    int current = 0;
    for (int w = 0; w < image.cols; w += this->config->blockSize)
    {
        if (w + this->config->blockSize > image.cols)
        {
            w = image.cols - this->config->blockSize;
        }
        for (int h = 0; h < image.rows; h += this->config->blockSize)
        {
            if (h + this->config->blockSize > image.rows)
            {
                h = image.rows - this->config->blockSize;
            }
            for (int i = 0; i < this->config->blockSize; i++)
            {
                for (int j = 0; j < this->config->blockSize; j++)
                {
                    Vec3b colors = image.at<Vec3b>(h + i, w + j);
                    values[current * CHUNK_SIZE + i * this->config->blockSize * 3 + j * 3 + 2] = (float32_t)colors.val[0] / 255.;
                    values[current * CHUNK_SIZE + i * this->config->blockSize * 3 + j * 3 + 1] = (float32_t)colors.val[1] / 255.;
                    values[current * CHUNK_SIZE + i * this->config->blockSize * 3 + j * 3 + 0] = (float32_t)colors.val[2] / 255.;
                }
            }
            current++;
        }
    };
    return values;
}

double PhotosCnnAlgorithm::process(float32_t *values, int samplesNumber)
{
    if (TF_GetCode(this->env->getStatus()) == TF_OK)
    {
        TF_Graph *graph = this->env->getGraph();
        TF_Session *session = this->env->getSession();
        TF_Status *status = TF_NewStatus();

        double fakeBorder = this->config->getFakeBorder();
        float32_t countFakes = 0.0;
        if (values == NULL || samplesNumber == 0)
        {
            return 0.9;
        }
        vector<TF_Output> inputs;
        vector<TF_Tensor *> inputValues;
        TF_Operation *inputOp = TF_GraphOperationByName(graph, this->config->getModelInput());
        TF_Output inputOpout = {inputOp, 0};
        inputs.push_back(inputOpout);
        vector<TF_Output> outputs;
        TF_Operation *outputOp = TF_GraphOperationByName(graph, this->config->getModelOutput());
        TF_Output outputOpout = {outputOp, 0};
        outputs.push_back(outputOpout);
        for (int top = 0; top < samplesNumber; top += INPUT_SIZE)
        {
            int size = top + INPUT_SIZE < samplesNumber ? INPUT_SIZE : samplesNumber % INPUT_SIZE;
            int numBytesIn = size * CHUNK_SIZE * sizeof(float32_t);
            int numBytesOut = size * 2 * sizeof(float32_t);
            int64_t inDims[] = {size, this->config->blockSize, this->config->blockSize, 3}; // CHUNK_SIZE depended
            int64_t outDims[] = {size, 2};                                                  // probabilities: good-0, fake-1
            TF_Tensor *input = TF_NewTensor(TF_FLOAT, inDims, *(&inDims + 1) - inDims, &values[top * CHUNK_SIZE], numBytesIn, &TF_Deallocator, 0);
            inputValues.clear();
            inputValues.push_back(input);
            TF_Tensor *outputValue = TF_AllocateTensor(TF_FLOAT, outDims, *(&outDims + 1) - outDims, numBytesOut);
            vector<TF_Tensor *> outputValues(outputs.size(), nullptr);
            outputValues.clear();
            outputValues.push_back(outputValue);
            TF_SessionRun(session, nullptr, &inputs[0], &inputValues[0], inputs.size(), &outputs[0], &outputValues[0], outputs.size(), nullptr, 0, nullptr, status);
            if (TF_GetCode(status) != TF_OK)
            {
                return 0.5;
            }
            float32_t *outVals = (float32_t *)TF_TensorData(outputValues[0]);
            for (int i = 0; i < size; i++)
            {
                if (outVals[2 * i + 1] > outVals[2 * i + 0])
                {
                    countFakes += 1.0;
                }
            }
            TF_DeleteTensor(input);
            TF_DeleteTensor(outputValue);
        }

        TF_DeleteStatus(status);
        if (values)
        {
            free(values);
        }
        // Normalization [0, inf) - > [0, 1); fakeBorder => 0.0005
        return this->config->getParanoidLevel() * (1.0 - (fakeBorder / (fakeBorder + (countFakes / (float)samplesNumber))));
    } else {
        return 0.5;
    }
}

double PhotosCnnAlgorithm::getScore(Mat image, const double FINE_TUNE)
{
    int samplesNumber = ((image.cols) / this->config->blockSize + 1) * ((image.rows) / this->config->blockSize + 1);
    float32_t *values = (float32_t *)malloc(samplesNumber * CHUNK_SIZE * sizeof(float32_t));
    int current = 0;
    for (int w = 0; w < image.cols; w += this->config->blockSize)
    {
        if (w + this->config->blockSize > image.cols)
        {
            w = image.cols - this->config->blockSize;
        }
        for (int h = 0; h < image.rows; h += this->config->blockSize)
        {
            if (h + this->config->blockSize > image.rows)
            {
                h = image.rows - this->config->blockSize;
            }
            for (int i = 0; i < this->config->blockSize; i++)
            {
                for (int j = 0; j < this->config->blockSize; j++)
                {
                    Vec3b colors = image.at<Vec3b>(h + i, w + j);
                    values[current * CHUNK_SIZE + i * this->config->blockSize * 3 + j * 3 + 2] = (float32_t)colors.val[0] / 255.;
                    values[current * CHUNK_SIZE + i * this->config->blockSize * 3 + j * 3 + 1] = (float32_t)colors.val[1] / 255.;
                    values[current * CHUNK_SIZE + i * this->config->blockSize * 3 + j * 3 + 0] = (float32_t)colors.val[2] / 255.;
                }
            }
            current++;
        }
    };
    return this->config->getParanoidLevel() * (1.0 - (FINE_TUNE / (FINE_TUNE + this->process(values, samplesNumber))));
}

SuspiciousInfo *PhotosCnnAlgorithm::check(const char *filename)
{
    SuspiciousInfo *result = new SuspiciousInfo(this->getMethod());
    try
    {
        if (PhotosCnnAlgorithm::isFileExists(filename))
        {
            // Read input image data
            int samplesNumber = 0;
            float32_t *values = this->readImage(filename, samplesNumber);
            result->setProbability(this->process(values, samplesNumber));
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
        std::exception_ptr p = std::current_exception();
        result->setProbability(0.5);
        result->setErrorCode(SuspiciousInfo::ERROR_UNKNOWN);
        result->setComments(re.what());
    }
    catch (const std::exception &ex)
    {
        std::exception_ptr p = std::current_exception();
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
