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

#ifndef CONFIGURATION_LIB_H
#define CONFIGURATION_LIB_H

#ifdef WIN32
    #ifdef BUILDING_FDF_LIB
        #define FDF_LIB __declspec(dllexport)
    #else
        #define FDF_LIB __declspec(dllimport)
    #endif
#else 
    #define FDF_LIB
#endif

#include "../include/rapidjson/document.h"

class FDF_LIB Configuration
{
    protected:
        const char* json;
        double paranoidLevel = 1.0;        
    public:
        const char* VERSION = "0.2.2";
        Configuration(const char* json = "");
        const char* getJson() { return this->json; };
        double getParanoidLevel()
        {
            return this->paranoidLevel;
        }
        void setParanoidLevel(double paranoidLevel)
        {
            this->paranoidLevel = paranoidLevel;
        }
        virtual const char* parse() = 0;
        virtual ~Configuration() { /* pass */ };        
};

#endif // CONFIGURATION_LIB_H
