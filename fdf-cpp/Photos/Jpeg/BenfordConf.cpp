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

#include "BenfordConf.h"

using namespace rapidjson;
using namespace Photos::Jpeg;

int BenfordConf::getBenfordBlockSize()
{
    return this->benfordBlockSize;
}

double BenfordConf::getBenfordFakeThreshold()
{
    return this->benfordFakeThreshold;
}

const char *BenfordConf::parse()
{
    if (this->json && strlen(this->json))
    {
        Document document;
        document.Parse(this->json);
        if (document.IsObject() && document.HasMember("benfordBlockSize") && document["benfordBlockSize"].IsInt())
        {
            this->benfordBlockSize = document["benfordBlockSize"].GetInt();
        }
    }
    return "Ok";
}