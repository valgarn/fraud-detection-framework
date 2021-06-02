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

#include "JpegTools.h"

using namespace std;
using namespace Photos::Jpeg;

int JpegTools::getQuality(unsigned char *jpeg, int size, string& message)
{
    vector<unsigned char *> *dqt = JpegTools::decode(jpeg, size, message);
    if (dqt)
    {
        if (dqt->size() != 2 && dqt->size() != 3)
        {
            message = "Wrong number of Quantization Tables!";
            JpegTools::clearVector(dqt);
            return 0;
        }

        double Y = 0;
        for (int i = 1; i < 64; i++)
        {
            Y += dqt->at(0)[i];
        }
        Y /= 63.0;

        double Cr = 0;
        for (int i = 1; i < 64; i++)
        {
            Cr += dqt->at(1)[i];
        }
        Cr /= 63.0;

        double Cb = 0;
        if (dqt->size() == 2)
        {
            Cb = Cr;
        }
        else
        {
            for (int i = 1; i < 64; i++)
            {
                Cb += dqt->at(2)[i];
            }
            Cb /= 63.0;
        }

        JpegTools::clearVector(dqt);
        int q = (int)ceil(100.0 - (Y + Cr + Cb) / 3.0 + (abs(Cr - Y) + abs(Cb - Y)) / 2.0);
        return q > 100 ? 100 : q;
    }
    return 0;
}

string JpegTools::type2str(int type)
{
    string r;

    uchar depth = type & CV_MAT_DEPTH_MASK;
    uchar chans = 1 + (type >> CV_CN_SHIFT);

    switch (depth)
    {
    case CV_8U:
        r = "CV_8U";
        break;
    case CV_8S:
        r = "CV_8S";
        break;
    case CV_16U:
        r = "CV_16U";
        break;
    case CV_16S:
        r = "CV_16S";
        break;
    case CV_32S:
        r = "CV_32S";
        break;
    case CV_32F:
        r = "CV_32F";
        break;
    case CV_64F:
        r = "CV_64F";
        break;
    default:
        r = "CV_User";
        break;
    }

    r += "C";
    r += (chans + '0');

    return r;
}

double JpegTools::getDiff(Mat bitmap1, Mat bitmap2)
{
    double n = bitmap1.cols * bitmap1.rows;
    double diff = 0;
    double tempDiff = 0;

    bitmap1.convertTo(bitmap1, CV_8UC3);
    bitmap2.convertTo(bitmap2, CV_8UC3);

    for (int i = 0; i < n; i++)
    {
        Vec3b c1 = bitmap1.data[i];
        Vec3b c2 = bitmap2.data[i];
        tempDiff += (abs(c1[0] - c2[0]) + abs(c1[1] - c2[1]) + abs(c1[2] - c2[2])) / (3.0 * 255.0);
        if (tempDiff > n)
        {
            diff += tempDiff / n;
            tempDiff = 0;
        }
    }
    return diff + tempDiff / n;
}

void JpegTools::clearVector(vector<unsigned char *> *v)
{
    while (v->size())
    {
        unsigned char *t = v->back();
        v->pop_back();
        delete t;
    }
    delete v;
}

vector<unsigned char *> *JpegTools::getDQT(unsigned char *jpeg, int &pos, int size, string& message)
{
    int length = (int)(jpeg[pos] << 8) | (int)jpeg[pos + 1];

    vector<unsigned char *> *qt = new vector<unsigned char *>();

    int i;
    pos += 2;
    length -= 2;

    while (length >= 65)
    {
        unsigned char *t = new unsigned char[64];
        if (pos > size - 2)
        {
            message = "JPEG buffer overflow error";
            JpegTools::clearVector(qt);
            return 0;
        }
        i = jpeg[pos++];
        if ((i & 0xFC) != 0)
        {
            message = "DQT parsing error";
            JpegTools::clearVector(qt);
            return 0;
        }

        for (i = 0; i < 64; ++i)
        {
            t[i] = jpeg[pos + i + 1];
        }
        qt->push_back(t);

        pos += 64;
        length -= 65;
    }

    if (length != 0)
    {
        message = "Wrong DQT length";
        JpegTools::clearVector(qt);
        return 0;
    }

    return qt;
}

vector<unsigned char *> *JpegTools::decode(unsigned char *jpeg, int size, string& message)
{
    if (((jpeg[0] ^ 0xFF) | (jpeg[1] ^ 0xD8)) != 0)
    {
        message = "Wrong Jpeg file!";
        return 0;
    }

    int pos = 2;
    char ok = 1;

    vector<unsigned char *> *dqt = new vector<unsigned char *>();
    vector<unsigned char *> *tbl = 0;

    while (ok)
    {
        pos += 2;

        switch (jpeg[pos - 1])
        {
        case 0xC0:
            // SOF;
            pos += (int)(jpeg[pos] << 8) | (int)jpeg[pos + 1];
            break;
        case 0xC2:
            //SOF;
            pos += (int)(jpeg[pos] << 8) | (int)jpeg[pos + 1];
            break;
        case 0xC4:
            //DHT
            pos += (int)(jpeg[pos] << 8) | (int)jpeg[pos + 1];
            break;
        case 0xDB:
            tbl = JpegTools::getDQT(jpeg, pos, size, message);
            if (tbl)
            {
                while (tbl->size())
                {
                    unsigned char *t = tbl->back();
                    tbl->pop_back();
                    dqt->push_back(t);
                }
                delete tbl;
            }
            else
            {
                JpegTools::clearVector(dqt);
                return 0;
            }
            break;
        case 0xDD:
            //DRI
            pos += (int)(jpeg[pos] << 8) | (int)jpeg[pos + 1];
            break;
        case 0xDA:
            //Scan
            pos += (int)(jpeg[pos] << 8) | (int)jpeg[pos + 1];
            ok = false;
            break;
        case 0xFE:
            //Skip Marker
            pos += (int)(jpeg[pos] << 8) | (int)jpeg[pos + 1];
            break;
        default:
            if ((jpeg[pos - 1] & 0xF0) == 0xE0)
            {
                pos += (int)(jpeg[pos] << 8) | (int)jpeg[pos + 1];
            }
            else
            {
                message = "Unsupported Jpeg format!";
                JpegTools::clearVector(dqt);
                return 0;
            }
            break;
        }
    }
    return dqt;
}
