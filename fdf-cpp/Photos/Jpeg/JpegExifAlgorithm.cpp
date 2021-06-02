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

#include <iostream>
#include <map>

#include <sys/stat.h>
#include <time.h>

#include "../Jpeg/JpegTools.h"
#include "JpegExifAlgorithm.h"

using namespace std;
using namespace Photos::Jpeg;

void JpegExifAlgorithm::checkTags(ExifData *ed)
{
    map<const char *, int> tags;
    tags["EXIF_TAG_IMAGE_WIDTH"] = EXIF_TAG_IMAGE_WIDTH;
    tags["EXIF_TAG_IMAGE_LENGTH"] = EXIF_TAG_IMAGE_LENGTH;
    tags["EXIF_TAG_BITS_PER_SAMPLE"] = EXIF_TAG_BITS_PER_SAMPLE;
    tags["EXIF_TAG_SAMPLES_PER_PIXEL"] = EXIF_TAG_SAMPLES_PER_PIXEL;
    tags["EXIF_TAG_COMPRESSION"] = EXIF_TAG_COMPRESSION;
    tags["EXIF_TAG_X_RESOLUTION"] = EXIF_TAG_X_RESOLUTION;
    tags["EXIF_TAG_Y_RESOLUTION"] = EXIF_TAG_Y_RESOLUTION;
    tags["EXIF_TAG_DATE_TIME"] = EXIF_TAG_DATE_TIME;
    tags["EXIF_TAG_RELATED_IMAGE_WIDTH"] = EXIF_TAG_RELATED_IMAGE_WIDTH;
    tags["EXIF_TAG_RELATED_IMAGE_LENGTH"] = EXIF_TAG_RELATED_IMAGE_LENGTH;
    tags["EXIF_TAG_TIME_ZONE_OFFSET"] = EXIF_TAG_TIME_ZONE_OFFSET;
    tags["EXIF_TAG_DATE_TIME_ORIGINAL"] = EXIF_TAG_DATE_TIME_ORIGINAL;
    tags["EXIF_TAG_DATE_TIME_DIGITIZED"] = EXIF_TAG_DATE_TIME_DIGITIZED;
    tags["EXIF_TAG_GPS_DATE_STAMP"] = EXIF_TAG_GPS_DATE_STAMP;

    char resbuf[JpegExifAlgorithm::BUFFER_SIZE];
    for (map<const char *, int>::iterator iter = tags.begin(); iter != tags.end(); ++iter)
    {
        const char *k = iter->first;
        int v = iter->second;
        ExifEntry *entry = exif_content_get_entry(ed->ifd[EXIF_IFD_0], (ExifTag)v);
        if (entry)
        {
            fill(resbuf, resbuf + JpegExifAlgorithm::BUFFER_SIZE, 0);
            exif_entry_get_value(entry, resbuf, JpegExifAlgorithm::BUFFER_SIZE - 1);
            if (*resbuf)
            {
                //string str(resbuf);
                //transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return toupper(c); });
                cout << k << ": " << resbuf << endl;
            }
        }
    }
}

void JpegExifAlgorithm::checkImage(Mat image, SuspiciousInfo *result)
{
    if (image.channels() != 3)
    {
        result->setProbability(0.6);
        string str = "Wrong channels number: " + to_string(image.channels());
        result->setComments(str.c_str());
    }
    else if (this->config->getScreenResolutions().find(image.cols) == this->config->getScreenResolutions().end() ||
             this->config->getScreenResolutions().find(image.rows) == this->config->getScreenResolutions().end())
    {
        result->setProbability(0.6);
        string str = "Wrong photo size: " + to_string(image.rows) + " x " + to_string(image.cols);
        result->setComments(str.c_str());
    }
    else
    {
        double ratio = (double)max(image.cols, image.rows) / min(image.cols, image.rows);
        set<double> ratios = this->config->getScreenRatios();
        for (set<double>::iterator iter = ratios.begin(); iter != ratios.end(); ++iter)
        {
            if (abs(ratio - *iter) < 0.003)
            {
                ratio = -1;
                break;
            }
        }

        if (ratio > 0)
        {
            result->setProbability(0.6);
            string str = "Wrong photo ratio: " + to_string(ratio);
            result->setComments(str.c_str());
        }
    }
}

void JpegExifAlgorithm::checkExif(const char *filename, Mat image, SuspiciousInfo *result)
{
    ifstream ifs(filename, ios::binary | ios::ate);
    // Will we use very big jpegs?
    int length = ifs.tellg();
    unsigned char *jpeg = new unsigned char[length];
    ifs.seekg(0, ios::beg);
    ifs.read((char *)jpeg, length);
    string message = "";
    int quality = JpegTools::getQuality(jpeg, length, message);
    delete[] jpeg;

    if (quality == 0)
    {
        result->setProbability(0.5);
        string str = "Exif analize is imposible.";
        result->setComments(str.c_str());
    }

    if (result->getProbability() == 0.0)
    {
        vector<string> editors = this->config->getEditors();
        // FIXME: memory-leak. ~188 bytes per call. Probably to use openCV::exif instead.
        JPEGData *pJPEGData = jpeg_data_new_from_file(filename);
        ExifData *ed = jpeg_data_get_exif_data(pJPEGData);
        if (!ed)
        {
            result->setProbability(0.75);
            result->setErrorCode(SuspiciousInfo::ERROR_EXIF_NO);
            result->setComments("ERROR_EXIF_NO");
        }
        else
        {
            char resbuf[JpegExifAlgorithm::BUFFER_SIZE];
            ExifEntry *entry = exif_content_get_entry(ed->ifd[EXIF_IFD_0], EXIF_TAG_IMAGE_WIDTH);
            if (entry)
            {
                fill(resbuf, resbuf + JpegExifAlgorithm::BUFFER_SIZE, 0);
                exif_entry_get_value(entry, resbuf, JpegExifAlgorithm::BUFFER_SIZE - 1);
                if (*resbuf)
                {
                    if (atoi(resbuf) != image.cols && atoi(resbuf) != image.rows)
                    {
                        result->setProbability(0.9);
                        string str = "Odd EXIF width: " + to_string(atoi(resbuf));
                        result->setComments(str.c_str());
                    }
                }
            }

            if (result->getProbability() == 0.0)
            {
                entry = exif_content_get_entry(ed->ifd[EXIF_IFD_0], EXIF_TAG_IMAGE_LENGTH);
                if (entry)
                {
                    fill(resbuf, resbuf + JpegExifAlgorithm::BUFFER_SIZE, 0);
                    exif_entry_get_value(entry, resbuf, JpegExifAlgorithm::BUFFER_SIZE - 1);
                    if (*resbuf)
                    {
                        if (atoi(resbuf) != image.rows && atoi(resbuf) != image.cols)
                        {
                            result->setProbability(0.9);
                            string str = "Odd EXIF height: " + to_string(atoi(resbuf));
                            result->setComments(str.c_str());
                        }
                    }
                }
            }

            if (result->getProbability() == 0.0)
            {
                entry = exif_content_get_entry(ed->ifd[EXIF_IFD_0], EXIF_TAG_COPYRIGHT);
                if (entry)
                {
                    fill(resbuf, resbuf + JpegExifAlgorithm::BUFFER_SIZE, 0);
                    exif_entry_get_value(entry, resbuf, JpegExifAlgorithm::BUFFER_SIZE - 1);
                    if (*resbuf)
                    {
                        string str(resbuf);
                        transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return toupper(c); });
                        for (vector<string>::size_type i = 0; i < editors.size(); i++)
                        {
                            size_t found = str.find(editors[i]);
                            if (found != std::string::npos)
                            {
                                result->setProbability(0.9);
                                str = "Found `" + editors[i] + "` in `" + str + "` copyright tag.";
                                result->setComments(str.c_str());
                                break;
                            }
                        }
                    }
                }
            }

            if (result->getProbability() == 0.0)
            {
                entry = exif_content_get_entry(ed->ifd[EXIF_IFD_0], EXIF_TAG_SOFTWARE);
                if (entry)
                {
                    fill(resbuf, resbuf + JpegExifAlgorithm::BUFFER_SIZE, 0);
                    exif_entry_get_value(entry, resbuf, JpegExifAlgorithm::BUFFER_SIZE - 1);
                    if (*resbuf)
                    {
                        string str(resbuf);
                        transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return toupper(c); });
                        for (vector<string>::size_type i = 0; i < editors.size(); i++)
                        {
                            size_t found = str.find(editors[i]);
                            if (found != std::string::npos)
                            {
                                result->setProbability(0.9);
                                str = "Found `" + editors[i] + "` in `" + str + "` software tag.";
                                result->setComments(str.c_str());
                                break;
                            }
                        }
                    }
                }
            }

            if (result->getProbability() == 0.0)
            {
                entry = exif_content_get_entry(ed->ifd[EXIF_IFD_0], EXIF_TAG_USER_COMMENT);
                if (entry)
                {
                    fill(resbuf, resbuf + JpegExifAlgorithm::BUFFER_SIZE, 0);
                    exif_entry_get_value(entry, resbuf, JpegExifAlgorithm::BUFFER_SIZE - 1);
                    if (*resbuf)
                    {
                        string str(resbuf);
                        transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return toupper(c); });
                        for (vector<string>::size_type i = 0; i < editors.size(); i++)
                        {
                            size_t found = str.find(editors[i]);
                            if (found != std::string::npos)
                            {
                                result->setProbability(0.9);
                                str = "Found `" + editors[i] + "` in `" + str + "` user comments tag.";
                                result->setComments(str.c_str());
                                break;
                            }
                        }
                    }
                }
            }

            if (result->getProbability() == 0.0)
            {
                entry = exif_content_get_entry(ed->ifd[EXIF_IFD_0], EXIF_TAG_XP_COMMENT);
                if (entry)
                {
                    fill(resbuf, resbuf + JpegExifAlgorithm::BUFFER_SIZE, 0);
                    exif_entry_get_value(entry, resbuf, JpegExifAlgorithm::BUFFER_SIZE - 1);
                    if (*resbuf)
                    {
                        string str(resbuf);
                        transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return toupper(c); });
                        for (vector<string>::size_type i = 0; i < editors.size(); i++)
                        {
                            size_t found = str.find(editors[i]);
                            if (found != std::string::npos)
                            {
                                result->setProbability(0.9);
                                str = "Found `" + editors[i] + "` in `" + str + "` comment tag.";
                                result->setComments(str.c_str());
                                break;
                            }
                        }
                    }
                }
            }

            if (result->getProbability() == 0.0)
            {
                entry = exif_content_get_entry(ed->ifd[EXIF_IFD_0], EXIF_TAG_DATE_TIME);
                if (entry)
                {
                    fill(resbuf, resbuf + JpegExifAlgorithm::BUFFER_SIZE, 0);
                    exif_entry_get_value(entry, resbuf, JpegExifAlgorithm::BUFFER_SIZE - 1);
                    if (*resbuf)
                    {
                        struct tm exifTime;
                        if (strptime(resbuf, "%Y:%m:%d %H:%M:%S", &exifTime) == NULL)
                        {
                            result->setProbability(0.8);
                            string str(resbuf);
                            str = "Wrong EXIF creation time: " + str;
                            result->setComments(str.c_str());
                        }
                        else
                        {
                            struct stat t_stat;
                            stat(filename, &t_stat);
                            struct tm modificationTime = *gmtime(&t_stat.st_mtime);
                            if (difftime(mktime(&modificationTime), mktime(&exifTime)) < 0)
                            {
                                result->setProbability(0.8);
                                string str("Wrong file modification time");
                                result->setComments(str.c_str());
                            }
                            else
                            {
                                struct tm changeTime = *gmtime(&t_stat.st_ctime);
                                if (difftime(mktime(&modificationTime), mktime(&changeTime)) > 0)
                                {
                                    result->setProbability(0.8);
                                    string str("Wrong file change time");
                                    result->setComments(str.c_str());
                                }
                                else
                                {
                                    struct tm accessTime = *gmtime(&t_stat.st_atime);
                                    if (difftime(mktime(&accessTime), mktime(&changeTime)) < 0)
                                    {
                                        result->setProbability(0.8);
                                        string str("Wrong file access time");
                                        result->setComments(str.c_str());
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        exif_data_unref(ed);
        jpeg_data_unref(pJPEGData);
    }
}

SuspiciousInfo *JpegExifAlgorithm::check(const char *filename)
{
    SuspiciousInfo *result = new SuspiciousInfo(this->getMethod());
    try
    {
        if (JpegExifAlgorithm::isFileExists(filename))
        {
            Mat image = imread(filename, CV_LOAD_IMAGE_COLOR);
            this->checkImage(image, result);
            if (result->getProbability() == 0.0)
            {
                this->checkExif(filename, image, result);
            }
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
