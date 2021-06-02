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

#include <sstream>
#include <iostream>

#include <boost/python/module.hpp>
#include <boost/python/class.hpp>
#include <boost/python/scope.hpp>
#include <boost/python.hpp>

#include "../Service/Configuration.h"
#include "../Service/SuspiciousInfo.h"
#include "../Photos/Jpeg/QualityConf.h"
#include "../Photos/Jpeg/JpegQualityAlgorithm.h"
#include "../Photos/Jpeg/ExifConf.h"
#include "../Photos/Jpeg/JpegExifAlgorithm.h"
#include "../Photos/Jpeg/BenfordConf.h"
#include "../Photos/Jpeg/JpegBenfordAlgorithm.h"
#include "../Photos/All/PcaConf.h"
#include "../Photos/All/PhotosPcaAlgorithm.h"
#include "../Photos/All/CnnConf.h"
#include "../Photos/All/CnnEnv.h"
#include "../Photos/All/PhotosCnnAlgorithm.h"
#include "../Photos/All/InfoConf.h"
#include "../Photos/All/PhotosInfoAlgorithm.h"

#include "Photos/All/PhotosInfoPython.h"
#include "Photos/All/PhotosCnnPython.h"
#include "Photos/All/PhotosPcaPython.h"

#include "Photos/Jpeg/JpegBenfordPython.h"
#include "Photos/Jpeg/JpegExifPython.h"
#include "Photos/Jpeg/JpegQualityPython.h"

using namespace std;
using namespace boost::python;

string Suspicious__str__( SuspiciousInfo const& info )
{
    stringstream output;
    output << "{ method: '" << info.getMethod() << "', probability: " << info.getProbability()
                    << ", processingSeconds: " << info.getProcessingSeconds() << ", comments: '" << info.getComments() << "' }";
    return output.str();
}

string Suspicious__repr__( SuspiciousInfo const& info )
{
    return "Suspicious: " + Suspicious__str__( info );
}

dict Suspicious_ToDict( SuspiciousInfo const& info )
{
    dict res;
    res["method"] = info.getMethod();
    res["probability"] = info.getProbability();
    res["processingSeconds"] = info.getProcessingSeconds();
    res["comments"] = info.getComments();
    return res;
}

void Suspicious_FromDict( SuspiciousInfo const& info, dict const& src )
{
    // readonly
}

class DummyPhotos{};
class DummyAll{};
class DummyJpeg{};

BOOST_PYTHON_MODULE( fdf )
{
    class_<SuspiciousInfo>( "Suspicious", init<const char*>( args( "method" ) ))
        .add_property( "method", &SuspiciousInfo::getMethod )
        .add_property( "probability", &SuspiciousInfo::getProbability )
        .add_property( "processingSeconds", &SuspiciousInfo::getProcessingSeconds )
        .add_property( "comments", &SuspiciousInfo::getComments )
        .def( "__str__", Suspicious__str__ )
        .def( "__repr__", Suspicious__repr__ )
        .add_property( "as_dict", Suspicious_ToDict, Suspicious_FromDict )
    ;

    scope Photos
        = class_<DummyPhotos>("Photos")
        ;
    {
        scope All
            = class_<DummyAll>("All")
            ;

            class_<Photos::All::PhotosInfoAlgorithm, boost::noncopyable> ("INFO", no_init)
                .def("__init__", make_constructor(&Photos::All::PhotosInfoPython::init, default_call_policies(), (boost::python::arg("json")="")))
                .def("check", &Photos::All::PhotosInfoAlgorithm::check, return_value_policy<reference_existing_object>())
                .def( "__str__", Photos::All::PhotosInfoPython__str__ )
                .def( "__repr__", Photos::All::PhotosInfoPython__repr__ )
                .add_property( "as_dict", Photos::All::PhotosInfoPython_ToDict, Photos::All::PhotosInfoPython_FromDict )
                ;

            class_<Photos::All::PhotosCnnAlgorithm, boost::noncopyable> ("CNN", no_init)
                .def("__init__", make_constructor(&Photos::All::PhotosCnnPython::init, default_call_policies(), (boost::python::arg("json")="")))
                .def("check", &Photos::All::PhotosCnnAlgorithm::check, return_value_policy<reference_existing_object>())
                .def( "__str__", Photos::All::PhotosCnnPython__str__ )
                .def( "__repr__", Photos::All::PhotosCnnPython__repr__ )
                .add_property( "as_dict", Photos::All::PhotosCnnPython_ToDict, Photos::All::PhotosCnnPython_FromDict )
                ;

            class_<Photos::All::PhotosPcaAlgorithm, boost::noncopyable> ("PCA", no_init)
                .def("__init__", make_constructor(&Photos::All::PhotosPcaPython::init, default_call_policies(), (boost::python::arg("json")="")))
                .def("check", &Photos::All::PhotosPcaAlgorithm::check, return_value_policy<reference_existing_object>())
                .def( "__str__", Photos::All::PhotosPcaPython__str__ )
                .def( "__repr__", Photos::All::PhotosPcaPython__repr__ )
                .add_property( "as_dict", Photos::All::PhotosPcaPython_ToDict, Photos::All::PhotosPcaPython_FromDict )
                ;
    }

    {
        scope Jpeg
            = class_<DummyJpeg>("Jpeg")
            ;

            class_<Photos::Jpeg::JpegBenfordAlgorithm, boost::noncopyable> ("BENFORD", no_init)
                .def("__init__", make_constructor(&Photos::Jpeg::JpegBenfordPython::init, default_call_policies(), (boost::python::arg("json")="")))
                .def("check", &Photos::Jpeg::JpegBenfordAlgorithm::check, return_value_policy<reference_existing_object>())
                .def( "__str__", Photos::Jpeg::JpegBenfordPython__str__ )
                .def( "__repr__", Photos::Jpeg::JpegBenfordPython__repr__ )
                .add_property( "as_dict", Photos::Jpeg::JpegBenfordPython_ToDict, Photos::Jpeg::JpegBenfordPython_FromDict )
                ;

            class_<Photos::Jpeg::JpegExifAlgorithm, boost::noncopyable> ("EXIF", no_init)
                .def("__init__", make_constructor(&Photos::Jpeg::JpegExifPython::init, default_call_policies(), (boost::python::arg("json")="")))
                .def("check", &Photos::Jpeg::JpegExifAlgorithm::check, return_value_policy<reference_existing_object>())
                .def( "__str__", Photos::Jpeg::JpegExifPython__str__ )
                .def( "__repr__", Photos::Jpeg::JpegExifPython__repr__ )
                .add_property( "as_dict", Photos::Jpeg::JpegExifPython_ToDict, Photos::Jpeg::JpegExifPython_FromDict )
                ;

            class_<Photos::Jpeg::JpegQualityAlgorithm, boost::noncopyable> ("QUALITY", no_init)
                .def("__init__", make_constructor(&Photos::Jpeg::JpegQualityPython::init, default_call_policies(), (boost::python::arg("json")="")))
                .def("check", &Photos::Jpeg::JpegQualityAlgorithm::check, return_value_policy<reference_existing_object>())
                .def( "__str__", Photos::Jpeg::JpegQualityPython__str__ )
                .def( "__repr__", Photos::Jpeg::JpegQualityPython__repr__ )
                .add_property( "as_dict", Photos::Jpeg::JpegQualityPython_ToDict, Photos::Jpeg::JpegQualityPython_FromDict )
                ;
    }
}
