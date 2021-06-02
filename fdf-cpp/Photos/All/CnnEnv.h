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

#ifndef CNN_ENV_LIB_H
#define CNN_ENV_LIB_H

#ifdef WIN32
  #ifdef BUILDING_FDF_LIB
  #define FDF_LIB __declspec(dllexport)
  #else
  #define FDF_LIB __declspec(dllimport)
  #endif
#else 
    #define FDF_LIB
#endif

#include <tensorflow/c_api.h>

#include "CnnConf.h"

using namespace Photos::All;

class CnnEnv
{
public:
    static CnnEnv &getInstance(CnnConf *config)
    {
        static CnnEnv instance(config);
        return instance;
    }

    TF_Status *getStatus();
    TF_Buffer *getGraphDef();
    TF_Graph *getGraph();
    TF_ImportGraphDefOptions *getGraphDefOpts();
    TF_SessionOptions *getSessOpts();
    TF_Session *getSession();

private:
    TF_Status *status = TF_NewStatus();
    TF_Buffer *graph_def = 0;
    TF_Graph *graph = 0;
    TF_ImportGraphDefOptions *graph_opts = 0;
    TF_SessionOptions *sess_opts = 0;
    TF_Session *session = 0;

    CnnEnv(CnnConf *config);
    ~CnnEnv();
    CnnEnv(const CnnEnv &) = delete;
    CnnEnv &operator=(const CnnEnv &) = delete;

    TF_Buffer *readFile(const char *filename);
};

#endif // CNN_ENV_LIB_H
