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

#include "QualityConf.h"

using namespace rapidjson;
using namespace Photos::Jpeg;

QualityConf::QualityConf(const char *json) : Configuration(json)
{
    // x[4] ?>?<? x[0] * d[0] + x[1] * d[1] + x[2] * d[2] + d[3]
    vector<double> v{40.77067222, 22.92322718, -36.0671806, 0.23663144, 0.25};
    this->equation = v;
    this->parse();
}

const char *QualityConf::parse()
{
    if (this->json && strlen(this->json))
    {
        Document document;
        document.Parse(this->json);
        if (document.IsObject())
        {
            /*if (document.HasMember("equation") && document["equation"].IsArray())
            {
                const Value& equation = document["equation"];
                double eq[] = new double[equation.Size()];
                for (SizeType i = 0; i < equation.Size(); i++) {
                    eq[i] = equation[i].GetDouble();
                }
                this->setEquation(eq);
            }*/
            if (document.HasMember("fakedJpegRealError") && document["fakedJpegRealError"].IsDouble())
            {
                this->fakedJpegRealError = document["fakedJpegRealError"].GetDouble();
            }
            if (document.HasMember("fakedQuality") && document["fakedQuality"].IsInt())
            {
                this->fakedQuality = document["fakedQuality"].GetInt();
            }
        }
    }
    return "OK";
}
