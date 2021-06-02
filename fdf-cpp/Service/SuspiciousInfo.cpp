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
#include <chrono>
#include <sstream>
#include <iomanip>

#include "SuspiciousInfo.h"

using namespace std;

SuspiciousInfo::SuspiciousInfo(const char *method)
{
    this->processingStartTime = chrono::high_resolution_clock::now();
    this->setMethod(method);
}

void SuspiciousInfo::setMethod(const char *method)
{
    this->method = string(method);
}

const char *SuspiciousInfo::getMethod() const
{
    return strdup(this->method.c_str());
}

void SuspiciousInfo::setProbability(double probability)
{
    this->probability = probability > 1 ? 1 : (probability < 0 ? 0 : probability);
}

double SuspiciousInfo::getProbability() const
{
    return this->probability;
}

void SuspiciousInfo::setComments(const char *comments)
{
    this->comments = string(comments);
}

const char *SuspiciousInfo::getComments() const
{
    return strdup(this->comments.c_str());
}

const char *SuspiciousInfo::getComments4json() const
{
    return strdup(this->escape4json(this->comments).c_str());
}

void SuspiciousInfo::setProcessingSeconds(double processingSeconds)
{
    this->processingSeconds = processingSeconds;
}

double SuspiciousInfo::getProcessingSeconds() const
{
    return this->processingSeconds;
}

void SuspiciousInfo::setErrorCode(int errorCode)
{
    this->errorCode = errorCode;
}

int SuspiciousInfo::getErrorCode() const
{
    return this->errorCode;
}

SuspiciousInfo *SuspiciousInfo::generateResult()
{
    chrono::duration<double> elapsed = chrono::high_resolution_clock::now() - this->processingStartTime;
    this->processingSeconds = elapsed.count();
    return this;
}

string SuspiciousInfo::escape4json(const string &s) const
{
    ostringstream o("");
    for (auto c = s.cbegin(); c != s.cend(); c++)
    {
        switch (*c)
        {
        case '"':
            o << "\\\"";
            break;
        case '\\':
            o << "\\\\";
            break;
        case '\b':
            o << "\\b";
            break;
        case '\f':
            o << "\\f";
            break;
        case '\n':
            o << "\\n";
            break;
        case '\r':
            o << "\\r";
            break;
        case '\t':
            o << "\\t";
            break;
        default:
            if ('\x00' <= *c && *c <= '\x1f')
            {
                o << "\\u" << hex << setw(4) << setfill('0') << (int)*c;
            }
            else
            {
                o << *c;
            }
        }
    }
    return o.str();
}
