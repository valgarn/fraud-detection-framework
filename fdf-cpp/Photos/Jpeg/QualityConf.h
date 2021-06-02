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

#ifndef QUALITY_CONF_LIB_H
#define QUALITY_CONF_LIB_H

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
  namespace Jpeg
  {
        class FDF_LIB QualityConf : public Configuration
        {
        protected:
            // x[4] ?>?<? x[0] * d[0] + x[1] * d[1] + x[2] * d[2] + d[3] 
            vector<double> equation;

            double fakedJpegRealError = 0.00225;
            int fakedQuality = 75;

        public:
            QualityConf(const char *json = "");
            
            double getFakedJpegRealError()
            {
                return this->fakedJpegRealError;
            }
            void setFakedJpegRealError(double fakedJpegRealError)
            {
                this->fakedJpegRealError = fakedJpegRealError;
            }

            int getFakedQuality()
            {
                return this->fakedQuality;
            }
            void setFakedQuality(int fakedQuality)
            {
                this->fakedQuality = fakedQuality;
            }

            vector<double> getEquation()
            {
                return this->equation;
            }
            void setEquation(vector<double> equation)
            {
                //delete this->equation;
                this->equation = equation;
            }

            virtual const char *parse();
            virtual ~QualityConf(){/* pass */};
        };
    } // namespace Jpeg
} // namespace Photos

#endif // QUALITY_CONF_LIB_H
