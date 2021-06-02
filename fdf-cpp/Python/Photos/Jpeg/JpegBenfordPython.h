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

#include "../../../Photos/Jpeg/BenfordConf.h"
#include "../../../Photos/Jpeg/JpegBenfordAlgorithm.h"

using namespace boost::python;

namespace Photos
{
    namespace Jpeg
    {
        string JpegBenfordPython__str__( JpegBenfordAlgorithm const& algorithm )
        {
            stringstream output;
            output << "{ method: '" << algorithm.getMethod() << "' }";
            return output.str();
        }

        string JpegBenfordPython__repr__( JpegBenfordAlgorithm const& algorithm )
        {
            return "fdf: " + JpegBenfordPython__str__( algorithm );
        }

        dict JpegBenfordPython_ToDict( JpegBenfordAlgorithm const& algorithm )
        {
            dict res;
            res["method"] = algorithm.getMethod();
            return res;
        }

        void JpegBenfordPython_FromDict( JpegBenfordAlgorithm const& algorithm, dict const& src )
        {
            // readonly
        }

        class JpegBenfordPython
        {
            public:
                static boost::shared_ptr<JpegBenfordAlgorithm> init(const object &data)
                {
                    string json = extract<string>(data);
                    if(json.size() > 0) {
                        BenfordConf *conf = new BenfordConf(json.c_str());
                        return boost::shared_ptr<JpegBenfordAlgorithm>(new JpegBenfordAlgorithm(conf));
                    } else {
                        return boost::shared_ptr<JpegBenfordAlgorithm>(new JpegBenfordAlgorithm());
                    }
                };
        };
    } // namespace All
} // namespace Photos
