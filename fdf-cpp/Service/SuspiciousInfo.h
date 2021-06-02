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

#ifndef SUSPICIOUS_LIB_H
#define SUSPICIOUS_LIB_H

#ifdef WIN32
#ifdef BUILDING_FDF_LIB
#define FDF_LIB __declspec(dllexport)
#else
#define FDF_LIB __declspec(dllimport)
#endif
#else
#define FDF_LIB
#endif

#include <string>
#include <cstring>
#include <chrono>
#include <ctime>

using namespace std;

class FDF_LIB SuspiciousInfo
{
protected:
    string method;
    double probability = 0.0;
    string comments;
    double processingSeconds = 0.0;
    chrono::time_point<std::chrono::high_resolution_clock> processingStartTime;
    int errorCode = 0;

public:
    static const int ERROR_NO = 0;
    static const int ERROR_MISSING_FILE = 1;
    static const int ERROR_OPENNING_FILE = 2;
    static const int ERROR_CNN_MISSING_GRAPH_FILE = 3;
    static const int ERROR_CNN_GRAPH_IMPORT = 4;
    static const int ERROR_CNN_CREATE_SESSION = 5;
    static const int ERROR_CNN_RUN_SESSION = 6;
    static const int ERROR_BENFORD_JPEG_DECOMPRESSION = 7;
    static const int ERROR_EXIF_NO = 8;
    static const int ERROR_QUALITY_WRONG_JPEG = 9;
    static const int ERROR_UNKNOWN = 10;

    SuspiciousInfo(const char *method = "unknown");
    void setMethod(const char *method);
    const char *getMethod() const;
    void setProbability(double probability);
    double getProbability() const;
    void setComments(const char *comments = "");
    const char *getComments() const;
    const char *getComments4json() const;
    void setProcessingSeconds(double processingSeconds);
    double getProcessingSeconds() const;
    void setErrorCode(int errorCode);
    int getErrorCode() const;
    SuspiciousInfo *generateResult();
    string escape4json(const string &s) const;
    virtual ~SuspiciousInfo(){/* pass */};
};

#endif // SUSPICIOUS_LIB_H
