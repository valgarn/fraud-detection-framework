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

#include <math.h>
#include <string>
#include <sstream>
#include <unordered_map>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "JpegBenfordAlgorithm.h"

using namespace std;
using namespace Photos::Jpeg;

void _libJpegErrorExit(j_common_ptr cinfo)
{
    char buffer[JMSG_LENGTH_MAX];
    // cinfo->err really points to a my_error_mgr struct, so coerce pointer
    libJpegErrorPtr error = (libJpegErrorPtr)cinfo->err;
    (*cinfo->err->format_message)(cinfo, buffer);
    //printf("%s\n", buffer);
    longjmp(error->setjmp_buffer, 1);
}

void _libJpegOutputMessage(j_common_ptr cinfo)
{
    char buffer[JMSG_LENGTH_MAX];
    (*cinfo->err->format_message)(cinfo, buffer);
    //printf("%s\n", buffer);
}

constexpr double JpegBenfordAlgorithm::benfordsCoefs[];

void JpegBenfordAlgorithm::printDctCoef(j_decompress_ptr cinfo, jvirt_barray_ptr *coeffs, int ci)
{
    jpeg_component_info *ci_ptr = &cinfo->comp_info[ci];
    JBLOCKARRAY buf =
        (cinfo->mem->access_virt_barray)(
            (j_common_ptr)cinfo,
            coeffs[ci],
            0,
            ci_ptr->v_samp_factor,
            FALSE);

    int sf;
    for (sf = 0; (JDIMENSION)sf < ci_ptr->height_in_blocks; ++sf)
    {
        JDIMENSION b;
        for (b = 0; b < ci_ptr->width_in_blocks; ++b)
        {
            //printf("\n** DCT block, channel %d **\n", ci);
            int j;
            for (j = 0; j < 64; j++)
            {
                int dc = buf[sf][b][j];
                dc = max(-128, dc);
                dc = min(128, dc);
                printf("%6d", dc);
                if (j % 8 == 7)
                {
                    puts("");
                }
            }
        }
    }
}

int JpegBenfordAlgorithm::getFirstDigit(int value)
{
    // Fastest way
    if (value < 10)
        return (int)value;
    if (value < 100)
        return (int)(value / 10);
    if (value < 1000)
        return (int)(value / 100);
    if (value < 10000)
        return (int)(value / 1000);
    if (value < 100000)
        return (int)(value / 10000);
    if (value < 1000000)
        return (int)(value / 100000);
    if (value < 10000000)
        return (int)(value / 1000000);
    if (value < 100000000)
        return (int)(value / 10000000);
    if (value < 1000000000)
        return (int)(value / 100000000);
    return (int)(value / 1000000000);
}

SuspiciousInfo *JpegBenfordAlgorithm::check(const char *filename)
{
    SuspiciousInfo *result = new SuspiciousInfo(this->getMethod());
    try
    {
        int benfordBlockSize = this->config->getBenfordBlockSize();
        double benfordFakeThreshold = this->config->getBenfordFakeThreshold();
        if (JpegBenfordAlgorithm::isFileExists(filename))
        {
            FILE *infile;
            struct jpeg_decompress_struct cinfo;
            struct LibJpegErrorMgr jerr;

            if ((infile = fopen(filename, "rb")) == NULL)
            {
                result->setProbability(0.5);
                result->setErrorCode(SuspiciousInfo::ERROR_OPENNING_FILE);
                result->setComments("ERROR_OPENNING_FILE");
                return result->generateResult();
            }

            /* set up the normal JPEG error routines, then override error_exit. */
            cinfo.err = jpeg_std_error(&jerr.pub);
            jerr.pub.error_exit = _libJpegErrorExit;
            jerr.pub.output_message = _libJpegOutputMessage;

            if (setjmp(jerr.setjmp_buffer))
            {
                jpeg_destroy_decompress(&cinfo);
                fclose(infile);
                result->setProbability(0.5);
                result->setErrorCode(SuspiciousInfo::ERROR_BENFORD_JPEG_DECOMPRESSION);
                result->setComments("ERROR_BENFORD_JPEG_DECOMPRESSION");
                return result->generateResult();
            }

            jpeg_create_decompress(&cinfo);
            jpeg_stdio_src(&cinfo, infile);
            jpeg_save_markers(&cinfo, JPEG_COM, 0xFFFF);
            jpeg_read_header(&cinfo, TRUE);

            JDIMENSION heightInBlocks = 0;
            JDIMENSION widthInBlocks = 0;

            // 10 - length of each features array 1..9, 0 - unnecessary
            // Collect digits for each mcu block
            unordered_map<int, int *> data;
            JDIMENSION dataSize = 0;

            double quadraticDeviation = 0.0;

            jvirt_barray_ptr *coeffs = jpeg_read_coefficients(&cinfo);
            for (int ci = 0; ci < cinfo.num_components; ci++)
            {
                jpeg_component_info *ci_ptr = &cinfo.comp_info[ci];

                if (heightInBlocks == 0)
                {
                    widthInBlocks = ci_ptr->width_in_blocks;
                    heightInBlocks = ci_ptr->height_in_blocks;
                }
                else if (widthInBlocks != ci_ptr->width_in_blocks || heightInBlocks != ci_ptr->height_in_blocks)
                {
                    continue;
                }

                JBLOCKARRAY buf =
                    (cinfo.mem->access_virt_barray)(
                        (j_common_ptr)&cinfo,
                        coeffs[ci],
                        0,
                        ci_ptr->v_samp_factor,
                        FALSE);
                for (int y = 0; (JDIMENSION)y < heightInBlocks; ++y)
                {
                    int start = y * widthInBlocks;
                    JDIMENSION x;
                    for (x = 0; x < widthInBlocks; ++x)
                    {
                        if (data.find(start + x) == data.end())
                        {
                            data.insert({start + x, new int[10]()}); // array of zeros
                        }
                        int *d = data[start + x];
                        for (int j = 0; j < 64; j++)
                        {
                            int dig = this->getFirstDigit(abs(buf[y][x][j]));
                            d[dig]++;
                        }
                    }
                    if (start + x - 1 > dataSize)
                    {
                        dataSize = start + x - 1;
                    }
                }
            }

            (void)jpeg_finish_decompress(&cinfo);
            jpeg_destroy_decompress(&cinfo);
            fclose(infile);

            if (heightInBlocks > 0 && widthInBlocks > 0)
            {
                // temporary storage of ten digits counts
                int digits[10];
                for (JDIMENSION y = 0; y < heightInBlocks - benfordBlockSize; y++)
                {
                    for (JDIMENSION x = 0; x < widthInBlocks - benfordBlockSize; x++)
                    {
                        fill(digits, digits + 10, 0);
                        for (int dx = 0; dx < benfordBlockSize; dx++)
                        {
                            for (int dy = 0; dy < benfordBlockSize; dy++)
                            {
                                int pos = x + dx + (y + dy) * widthInBlocks;
                                if (data.find(pos) != data.end())
                                {
                                    int *d = data[pos];
                                    for (int dig = 1; dig < 10; dig++)
                                    {
                                        digits[dig] += d[dig];
                                    }
                                }
                            }
                        }
                    }
                }

                for (int dig = 1; dig < 10; dig++)
                {
                    if (digits[1] == 0) {
                        quadraticDeviation += JpegBenfordAlgorithm::benfordsCoefs[dig];
                    } else {
                        quadraticDeviation += pow((((double)digits[dig] / (double)digits[1]) * JpegBenfordAlgorithm::benfordsCoefs[1] -
                                            JpegBenfordAlgorithm::benfordsCoefs[dig]) / JpegBenfordAlgorithm::benfordsCoefs[dig], 2);
                    }
                }
            }
            for (auto kv : data)
            {
                delete[] kv.second;
            }
            data.clear();
            // Normalization [0, inf) - > [0, 1); benfordFakeThreshold => 0.5
            result->setProbability(this->config->getParanoidLevel() * 0.5 *
                pow(2.0 * (1.0 - (benfordFakeThreshold / (benfordFakeThreshold + quadraticDeviation))), 3));
        }
        else
        {
            result->setProbability(0.5);
            result->setErrorCode(SuspiciousInfo::ERROR_MISSING_FILE);
            result->setComments("ERROR_MISSING_FILE");
        }
    }
    catch (const std::runtime_error &re)
    {
        result->setProbability(0.5);
        result->setErrorCode(SuspiciousInfo::ERROR_UNKNOWN);
        result->setComments(re.what());
    }
    catch (const std::exception &ex)
    {
        result->setProbability(0.5);
        result->setErrorCode(SuspiciousInfo::ERROR_UNKNOWN);
        result->setComments(ex.what());
    }
    catch (...)
    {
        result->setProbability(0.5);
        result->setErrorCode(SuspiciousInfo::ERROR_UNKNOWN);
        result->setComments(currentExceptionTypeName());
    }
    return result->generateResult();
}
