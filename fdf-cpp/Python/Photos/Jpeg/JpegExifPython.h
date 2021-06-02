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

#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/python.hpp>

#include "../../../Photos/Jpeg/ExifConf.h"
#include "../../../Photos/Jpeg/JpegExifAlgorithm.h"

using namespace boost::python;

namespace Photos
{
    namespace Jpeg
    {
        string JpegExifPython__str__( JpegExifAlgorithm const& algorithm )
        {
            stringstream output;
            output << "{ method: '" << algorithm.getMethod() << "' }";
            return output.str();
        }

        string JpegExifPython__repr__( JpegExifAlgorithm const& algorithm )
        {
            return "fdf: " + JpegExifPython__str__( algorithm );
        }

        dict JpegExifPython_ToDict( JpegExifAlgorithm const& algorithm )
        {
            dict res;
            res["method"] = algorithm.getMethod();
            return res;
        }

        void JpegExifPython_FromDict( JpegExifAlgorithm const& algorithm, dict const& src )
        {
            // readonly
        }

        class JpegExifPython
        {
            public:
                static boost::shared_ptr<JpegExifAlgorithm> init(const object &data)
                {
                    string json = extract<string>(data);
                    if(json.size() > 0) {
                        ExifConf *conf = new ExifConf(json.c_str());
                        return boost::shared_ptr<JpegExifAlgorithm>(new JpegExifAlgorithm(conf));
                    } else {
                        return boost::shared_ptr<JpegExifAlgorithm>(new JpegExifAlgorithm());
                    }
                };
        };
    } // namespace All
} // namespace Photos
