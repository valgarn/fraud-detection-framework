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

#ifndef INFO_CONF_LIB_H
#define INFO_CONF_LIB_H

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
#include <locale>
#include "../../include/rapidjson/document.h"

#include "../../Service/Configuration.h"

using namespace std;

namespace Photos
{
  namespace All
  {
        class FDF_LIB InfoConf : public Configuration
        {

        public:
            InfoConf(const char* json = "");
            virtual const char *parse();
            virtual ~InfoConf() { /* pass */ };
        };
    } // namespace All
} // namespace Photos

#endif // PCA_CONF_LIB_H
