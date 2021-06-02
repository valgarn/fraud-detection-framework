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

#ifndef BENFORD_LIB_H
#define BENFORD_LIB_H

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
#include <cctype>
#include <locale>
#include <functional>

#include <jerror.h>
#include <jpeglib.h>
#include <setjmp.h>

#include "../../Service/FraudDetectAlgorithm.h"
#include "BenfordConf.h"

namespace Photos
{
  namespace Jpeg
  {
        struct LibJpegErrorMgr
        {
            struct jpeg_error_mgr pub;
            jmp_buf setjmp_buffer;
        };

        typedef struct LibJpegErrorMgr *libJpegErrorPtr;

        class FDF_LIB JpegBenfordAlgorithm : public FraudDetectAlgorithm
        {
        private:
            BenfordConf *config;
            static constexpr double benfordsCoefs[10] = {0, 0.301029996, 0.176091259, 0.124938737, 0.096910013, 0.079181246,
                                                        0.06694679, 0.057991947, 0.051152522, 0.045757491};

        public:
            JpegBenfordAlgorithm() : FraudDetectAlgorithm()
            {
                this->method = "BENFORD";
                this->config = new BenfordConf();
            };

            JpegBenfordAlgorithm(BenfordConf *config) : FraudDetectAlgorithm((Configuration *)config)
            {
                this->method = "BENFORD";
                this->config = config;
            };
            int getFirstDigit(int value);
            int loadJpg(char *filename);
            void printDctCoef(j_decompress_ptr cinfo, jvirt_barray_ptr *coeffs, int ci);
            virtual SuspiciousInfo *check(const char *filename);
            virtual ~JpegBenfordAlgorithm()
            {
                delete this->config;
            };
        };
    } // namespace Jpeg
} // namespace Photos

#endif // BENFORD_LIB_H
