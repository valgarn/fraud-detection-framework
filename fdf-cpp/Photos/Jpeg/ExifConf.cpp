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

#include "ExifConf.h"

using namespace rapidjson;
using namespace Photos::Jpeg;

ExifConf::ExifConf(const char *json) : Configuration(json)
{
    this->parse();
}

set<double> ExifConf::getScreenRatios()
{
    return this->screenRatios;
}

void ExifConf::getScreenRatios(set<double> ratios)
{
    this->screenRatios = ratios;
}

set<int> ExifConf::getScreenResolutions()
{
    return this->screenResolutions;
}

void ExifConf::getScreenResolutions(set<int> resolutions)
{
    this->screenResolutions = resolutions;
}

vector<string> ExifConf::getEditors()
{
    return this->editors;
}

void ExifConf::setEditors(vector<string> editors)
{
    this->editors.clear();
    for (SizeType i = 0; i < editors.size(); i++)
    {
        transform(editors[i].begin(), editors[i].end(), editors[i].begin(), [](unsigned char c) { return toupper(c); });
        this->editors.push_back(editors[i]);
    }
}

const char *ExifConf::parse()
{
    if (this->json && strlen(this->json))
    {
        Document document;
        document.Parse(this->json);
        if (document.IsObject() && document.HasMember("editors"))
        {
            const Value &editors = document["editors"];
            if (editors.IsArray())
            {
                vector<string> vec;
                for (SizeType i = 0; i < editors.Size(); i++)
                {
                    vec.push_back(editors[i].GetString());
                }
                this->setEditors(vec);
            }
        }
    }
    return "OK";
}
