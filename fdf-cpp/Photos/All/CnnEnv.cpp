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

#include "CnnEnv.h"
#include "PhotosCnnAlgorithm.h"

using namespace Photos::All;

void TF_FreeBuffer(void *data, size_t length)
{
    free(data);
}

TF_Buffer *CnnEnv::readFile(const char *file)
{
    FILE *f = fopen(file, "rb");
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET); //same as rewind(f);
    void *data = malloc(fsize);
    fread(data, fsize, 1, f);
    fclose(f);
    TF_Buffer *buf = TF_NewBuffer();
    buf->data = data;
    buf->length = fsize;
    buf->data_deallocator = TF_FreeBuffer;
    return buf;
}

CnnEnv::CnnEnv(CnnConf *config)
{
    const char *modelPath = config->getModelPath();
    if (PhotosCnnAlgorithm::isFileExists(modelPath))
    {
        // Use read_file to get graph_def as TF_Buffer*
        this->graph_def = this->readFile(modelPath);
        this->graph = TF_NewGraph();
        // Import graph_def into graph
        this->status = TF_NewStatus();
        this->graph_opts = TF_NewImportGraphDefOptions();
        TF_GraphImportGraphDef(this->graph, this->graph_def, this->graph_opts, this->status);
        if (TF_GetCode(status) == TF_OK)
        {
            this->sess_opts = TF_NewSessionOptions();
            this->session = TF_NewSession(this->graph, this->sess_opts, this->status);
        }
    }
}

CnnEnv::~CnnEnv()
{
    if (this->session)
    {
        TF_CloseSession(this->session, this->status);
        TF_DeleteSession(this->session, this->status);
    }
    if (this->sess_opts)
    {
        TF_DeleteSessionOptions(this->sess_opts);
    }
    if (this->graph_opts)
    {
        TF_DeleteImportGraphDefOptions(this->graph_opts);
    }
    if (this->graph)
    {
        TF_DeleteGraph(this->graph);
    }
    if (this->graph_def)
    {
        TF_DeleteBuffer(this->graph_def);
    }
    TF_DeleteStatus(this->status);
}

TF_Status *CnnEnv::getStatus()
{
    return this->status;
}

TF_Buffer *CnnEnv::getGraphDef()
{
    return this->graph_def;
}

TF_Graph *CnnEnv::getGraph()
{
    return this->graph;
}

TF_ImportGraphDefOptions *CnnEnv::getGraphDefOpts()
{
    return this->graph_opts;
}

TF_SessionOptions *CnnEnv::getSessOpts()
{
    return this->sess_opts;
}

TF_Session *CnnEnv::getSession()
{
    return this->session;
}
