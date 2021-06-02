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

#include "FraudDetectAlgorithm.h"

FraudDetectAlgorithm::FraudDetectAlgorithm(){

};

FraudDetectAlgorithm::FraudDetectAlgorithm(Configuration *config)
{
    this->config = config;
}

const char *FraudDetectAlgorithm::currentExceptionTypeName()
{
    int status;
    return abi::__cxa_demangle(abi::__cxa_current_exception_type()->name(), 0, 0, &status);
}

void FraudDetectAlgorithm::appendMatToFile(const string &path, std::vector<Mat> images)
{
    ofstream ofs(path, ios::binary);
    for (std::vector<Mat>::iterator image = images.begin(); image != images.end(); ++image)
    {
        int rows = static_cast<int>((*image).rows);
        int cols = static_cast<int>((*image).cols);
        int type = (*image).type();
        ofs.write((const char *)(&rows), sizeof(int));
        ofs.write((const char *)(&cols), sizeof(int));
        ofs.write((const char *)(&type), sizeof(int));
        ofs.write((const char *)((*image).data), (*image).elemSize() * (*image).total());
    }
}

void FraudDetectAlgorithm::readMatsFromFile(const string &path, vector<Mat> &images)
{
    ifstream ifs(path, ios::binary);
    int rows, cols, type;
    while (ifs.good() && !ifs.eof())
    {
        rows = -1;
        ifs.read((char *)(&rows), sizeof(int));
        if (rows > 0)
        {
            ifs.read((char *)(&cols), sizeof(int));
            ifs.read((char *)(&type), sizeof(int));
            Mat image(rows, cols, type);
            ifs.read((char *)(image.data), image.elemSize() * image.total());
            images.push_back(image);
        }
    }
}


string FraudDetectAlgorithm::getName(string filename)
{
    string name = filename;
    string::size_type i = filename.find_last_of(PATH_SEPARATOR);
    if (i != string::npos)
    {
        name = name.substr(i + 1);
    }
    i = name.find_last_of(".");
    if (i != string::npos && i != 0)
    {
        name = name.substr(0, i);
    }
    return name;
}

string FraudDetectAlgorithm::getPath(string filename)
{
    string::size_type i = filename.find_last_of(PATH_SEPARATOR);
    if (i != string::npos)
    {
        return filename.substr(0, i+1);
    }
    return filename;
}
