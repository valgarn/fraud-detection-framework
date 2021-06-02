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

#ifndef FRAUD_LIB_H
#define FRAUD_LIB_H

#ifdef WIN32
    #ifdef BUILDING_FDF_LIB
        #define FDF_LIB __declspec(dllexport)
    #else
        #define FDF_LIB __declspec(dllimport)
    #endif
#else 
    #define FDF_LIB
#endif

#include <sys/stat.h>
#include <cxxabi.h>
#include <chrono>
#include <vector>
#include <opencv2/opencv.hpp>

#include "SuspiciousInfo.h"
#include "Configuration.h"

#if defined(WIN32) || defined(_WIN32) || defined(__CYGWIN__) 
    #define PATH_SEPARATOR "\\" 
#else 
    #define PATH_SEPARATOR "/" 
#endif

using namespace std;
using namespace cv;

class FDF_LIB FraudDetectAlgorithm
{
    protected:
        char* fileName;
        const char* method;
        Configuration* config;
    public:
        static inline bool isFileExists(const char *path) {
            struct stat fileStat; 
            return (stat(path, &fileStat) & !S_ISREG(fileStat.st_mode))? false : true;
        }

        static void appendMatToFile(const string& path, std::vector<Mat> images);
        static void readMatsFromFile(const string& path, std::vector<Mat>& images);
        static string getPath(string filename);
        static string getName(string filename);

        FraudDetectAlgorithm();
        FraudDetectAlgorithm(Configuration* config);
        const char *getMethod() const { return this->method; };
        const char *currentExceptionTypeName();
        virtual SuspiciousInfo *check(const char* filename) = 0;
};

#endif // FRAUD_LIB_H
