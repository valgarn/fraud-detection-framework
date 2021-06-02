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

#include "../../../Photos/All/InfoConf.h"
#include "../../../Photos/All/PhotosInfoAlgorithm.h"

using namespace std;
using namespace boost::python;

namespace Photos
{
    namespace All
    {
        string PhotosInfoPython__str__( PhotosInfoAlgorithm const& algorithm )
        {
            stringstream output;
            output << "{ method: '" << algorithm.getMethod() << "' }";
            return output.str();
        }

        string PhotosInfoPython__repr__( PhotosInfoAlgorithm const& algorithm )
        {
            return "fdf: " + PhotosInfoPython__str__( algorithm );
        }

        dict PhotosInfoPython_ToDict( PhotosInfoAlgorithm const& algorithm )
        {
            dict res;
            res["method"] = algorithm.getMethod();
            return res;
        }

        void PhotosInfoPython_FromDict( PhotosInfoAlgorithm const& algorithm, dict const& src )
        {
            // readonly
        }

        class PhotosInfoPython
        {
            public:
                static boost::shared_ptr<PhotosInfoAlgorithm> init(const object &data)
                {
                    string json = extract<string>(data);
                    if(json.size() > 0) {
                        InfoConf *conf = new InfoConf(json.c_str());
                        return boost::shared_ptr<PhotosInfoAlgorithm>(new PhotosInfoAlgorithm(conf));
                    } else {
                        return boost::shared_ptr<PhotosInfoAlgorithm>(new PhotosInfoAlgorithm());
                    }
                };
        };
    } // namespace All
} // namespace Photos
