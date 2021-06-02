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

#ifndef CNN_CONF_LIB_H
#define CNN_CONF_LIB_H

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
    class FDF_LIB CnnConf : public Configuration
    {
      protected:        
        double fakeBorder = 0.0006;
        string modelPath = "./fdf_graph.pb";
        string modelInput = "input";
        string modelOutput = "softmax_tensor";

      public:
        const int blockSize = 28;
        
        CnnConf(const char *json = "");
        virtual const char *parse();
        virtual ~CnnConf()
        { /* pass */
        }
        double getFakeBorder()
        {
          return this->fakeBorder;
        }
        void setFakeBorder(double fakeBorder)
        {
          this->fakeBorder = fakeBorder;
        }
        const char *getModelPath()
        {
          return this->modelPath.c_str();
        }
        void setModelPath(const char *modelPath)
        {
          this->modelPath = modelPath;
        }
        const char *getModelInput()
        {
          return this->modelInput.c_str();
        }
        void setModelInput(const char *modelInput)
        {
          this->modelInput = modelInput;
        }
        const char *getModelOutput()
        {
          return this->modelOutput.c_str();
        }
        void setModelOutput(const char *modelOutput)
        {
          this->modelOutput = modelOutput;
        }
      };
    } // namespace All
} // namespace Photos

#endif // CNN_CONF_LIB_H
