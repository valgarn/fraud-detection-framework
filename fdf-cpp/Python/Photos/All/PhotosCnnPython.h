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

#include <cmath>
#define _hypot hypot

#include <iostream>
#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/python.hpp>

#include "../../../Photos/All/CnnConf.h"
#include "../../../Photos/All/PhotosCnnAlgorithm.h"

using namespace std;
using namespace boost::python;

namespace Photos
{
    namespace All
    {
        string PhotosCnnPython__str__( PhotosCnnAlgorithm const& algorithm )
        {
            stringstream output;
            output << "{ method: '" << algorithm.getMethod() << "' }";
            return output.str();
        }

        string PhotosCnnPython__repr__( PhotosCnnAlgorithm const& algorithm )
        {
            return "fdf: " + PhotosCnnPython__str__( algorithm );
        }

        dict PhotosCnnPython_ToDict( PhotosCnnAlgorithm const& algorithm )
        {
            dict res;
            res["method"] = algorithm.getMethod();
            return res;
        }

        void PhotosCnnPython_FromDict( PhotosCnnAlgorithm const& algorithm, dict const& src )
        {
            // readonly
        }

        class PhotosCnnPython
        {
            public:
                static boost::shared_ptr<PhotosCnnAlgorithm> init(const object &data)
                {
                    string json = extract<string>(data);
                    if(json.size() > 0) {
                        CnnConf *conf = new CnnConf(json.c_str());
                        return boost::shared_ptr<PhotosCnnAlgorithm>(new PhotosCnnAlgorithm(conf));
                    } else {
                        return boost::shared_ptr<PhotosCnnAlgorithm>(new PhotosCnnAlgorithm());
                    }
                };
        };
    } // namespace All
} // namespace Photos
