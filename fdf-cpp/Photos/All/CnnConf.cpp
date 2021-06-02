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

#include "CnnConf.h"

using namespace rapidjson;
using namespace Photos::All;

CnnConf::CnnConf(const char *json) : Configuration(json)
{
    this->parse();
}

const char *CnnConf::parse()
{
    if (this->json && strlen(this->json))
    {
        Document document;
        document.Parse(this->json);
        if (document.IsObject())
        {
            if (document.HasMember("fakeBorder") && document["fakeBorder"].IsDouble())
            {
                this->fakeBorder = document["fakeBorder"].GetDouble();
            }
            if (document.HasMember("modelPath") && document["modelPath"].IsString())
            {
                this->modelPath = document["modelPath"].GetString();
            }
            if (document.HasMember("modelInput") && document["modelInput"].IsString())
            {
                this->modelInput = document["modelInput"].GetString();
            }
            if (document.HasMember("modelOutput") && document["modelOutput"].IsString())
            {
                this->modelOutput = document["modelOutput"].GetString();
            }
        }
    }
    return "OK";
}
