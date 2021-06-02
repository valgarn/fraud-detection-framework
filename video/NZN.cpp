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

#include "NZN.h"

NZN::NZN()
{
    deserialize("resources/shape_predictor_68_face_landmarks.dat") >> this->predictor;
    this->testInfo = new PhotosInfoAlgorithm();
    this->testPca = new PhotosPcaAlgorithm();
    this->testCnn = new PhotosCnnAlgorithm(new CnnConf(R"({
                    "modelPath": "./resources/m88-1.pb", 
                    "modelInput": "input_input", 
                    "modelOutput": "softmax_tensor/Softmax" })"));
    this->testQuality = new JpegQualityAlgorithm();
}

void NZN::cutFacesToMapsFile(string path)
{
    struct dirent *entry = nullptr;
    DIR *dp = nullptr;
    dp = opendir(path.c_str());
    if (dp != nullptr)
    {
        std::vector<string> files;
        while ((entry = readdir(dp)))
        {
            if (entry->d_type == DT_REG)
            {
                //cout << entry->d_name << endl;
                files.push_back(entry->d_name);
            }
        }
        std::vector<Mat> faces;
        for (int i = 0; i < files.size(); i++)
        {
            Mat image = imread(path + PATH_SEPARATOR + files.at(i), CV_LOAD_IMAGE_COLOR);
            //cout << path + PATH_SEPARATOR + files.at(i) << endl;
            image = this->cropFace(image);
            if (!image.empty())
            {
                faces.push_back(image);
            }
        }
        string filename = path;
        string::size_type i = filename.find_last_of(PATH_SEPARATOR);
        if (i != string::npos)
        {
            filename = filename.substr(i + 1);
        }
        FraudDetectAlgorithm::appendMatToFile(path + PATH_SEPARATOR + filename + ".maps", faces);
    }
    closedir(dp);
}

Mat NZN::cropFace(Mat image)
{
    frontal_face_detector detector = get_frontal_face_detector();
    cv_image<bgr_pixel> img(image);
    std::vector<dlib::rectangle> dets = detector(img);
    if (dets.size() == 1)
    {
        std::vector<full_object_detection> shapes;
        full_object_detection shape = this->predictor(img, dets[0]);
        if (shape.num_parts() == 68)
        {
            Mat croppedImg;
            int x = dets[0].left() - (double)dets[0].width() * 0.1;
            int y = dets[0].top() - (double)dets[0].height() * 0.35;
            int w = (double)dets[0].width() * 1.2;
            int h = (double)dets[0].height() * 1.5;
            Rect r((x < 0 ? 0 : x), (y < 0 ? 0 : y),
                   (x + w > image.cols ? image.cols - x : w),
                   (y + h > image.rows ? image.rows - y : h));
            image(r).copyTo(croppedImg);
            //imwrite(s.c_str(), croppedImg);
            return croppedImg;
        }
    }
    return Mat();
}

double NZN::getMean(std::vector<double> sortedValues)
{
    return accumulate(sortedValues.rbegin(), sortedValues.rend(), (double)0) / (double)sortedValues.size();
}

double NZN::getMode(std::vector<double> sortedValues)
{
    const double DELTA = 1E-3;
    double key = sortedValues[0];
    int value = 1;
    double maxKey = key;
    int maxValue = 0;

    for (int i = 1; i < sortedValues.size(); i++)
    {
        if (abs(key - sortedValues[i]) <= DELTA)
        {
            value++;
        }
        else
        {
            if (value > maxValue)
            {
                maxKey = sortedValues[i - 1];
                maxValue = value;
            }
            key = sortedValues[i];
            value = i;
        }
    }
    return maxKey;
}

double NZN::getMedian(std::vector<double> sortedValues)
{
    int n = sortedValues.size();
    return n % 2 ? sortedValues[n / 2] : (sortedValues[n / 2] + sortedValues[n / 2 - 1]) / 2.0;
}

// Total fakes score
double NZN::getScore(std::vector<double> sortedValues, double threshold /*= 0.5*/)
{
    int i = 0;
    for (; i < sortedValues.size() && sortedValues[i] <= threshold; i++)
        ;
    return (1.0 - (double)i / (double)sortedValues.size());
}

void NZN::printStats(int step, int totalFrames, string path, std::vector<double> scoresBlur, std::vector<double> scoresPca, 
                                            std::vector<double> scoresCnn, std::vector<double> scoresInterestPointsQuality, 
                                                                                    std::vector<double> scoresWindowsQuality)
{
    if (scoresPca.size() > 0 && scoresCnn.size() > 0 && scoresInterestPointsQuality.size() > 0 && scoresWindowsQuality.size() > 0)
    {
        sort(scoresPca.begin(), scoresPca.end());
        sort(scoresCnn.begin(), scoresCnn.end());
        sort(scoresInterestPointsQuality.begin(), scoresInterestPointsQuality.end());
        sort(scoresWindowsQuality.begin(), scoresWindowsQuality.end());

        cout << "iteration: { \"step\": " << step << ", \"frames\": " << scoresPca.size()
             << " \"total\": " << totalFrames << ", \"path\": \"" << path << "\", " << endl;

        cout << "\"blur\": {" << endl;
        cout << "\t\"score\": " << this->getScore(scoresBlur, 69) << ", " << endl;
        cout << "\t\"min\": " << scoresBlur[0] << ", " << endl;
        cout << "\t\"max\": " << scoresBlur[scoresPca.size() - 1] << ", " << endl;
        cout << "\t\"median\": " << this->getMedian(scoresBlur) << ", " << endl;
        cout << "\t\"mean\": " << this->getMean(scoresBlur) << ", " << endl;
        cout << "\t\"mode\": " << this->getMode(scoresBlur) << "}, " << endl;

        cout << "\"pca\": {" << endl;
        cout << "\t\"score\": " << this->getScore(scoresPca) << ", " << endl;
        cout << "\t\"min\": " << scoresPca[0] << ", " << endl;
        cout << "\t\"max\": " << scoresPca[scoresPca.size() - 1] << ", " << endl;
        cout << "\t\"median\": " << this->getMedian(scoresPca) << ", " << endl;
        cout << "\t\"mean\": " << this->getMean(scoresPca) << ", " << endl;
        cout << "\t\"mode\": " << this->getMode(scoresPca) << "}, " << endl;

        cout << "\"cnn\": {" << endl;
        cout << "\t\"score\": " << this->getScore(scoresCnn) << ", " << endl;
        cout << "\t\"min\": " << scoresCnn[0] << ", " << endl;
        cout << "\t\"max\": " << scoresCnn[scoresCnn.size() - 1] << ", " << endl;
        cout << "\t\"median\": " << this->getMedian(scoresCnn) << ", " << endl;
        cout << "\t\"mean\": " << this->getMean(scoresCnn) << ", " << endl;
        cout << "\t\"mode\": " << this->getMode(scoresCnn) << "}, " << endl;

        cout << "\"ipq\": {" << endl; //InterestPointsQuality
        cout << "\t\"score\": " << this->getScore(scoresInterestPointsQuality) << ", " << endl;
        cout << "\t\"min\": " << scoresInterestPointsQuality[0] << ", " << endl;
        cout << "\t\"max\": " << scoresInterestPointsQuality[scoresInterestPointsQuality.size() - 1] << ", " << endl;
        cout << "\t\"median\": " << this->getMedian(scoresInterestPointsQuality) << ", " << endl;
        cout << "\t\"mean\": " << this->getMean(scoresInterestPointsQuality) << ", " << endl;
        cout << "\t\"mode\": " << this->getMode(scoresInterestPointsQuality) << "}, " << endl;

        cout << "wq: {" << endl; //WindowsQuality
        cout << "\t\"score\": " << this->getScore(scoresWindowsQuality) << ", " << endl;
        cout << "\t\"min\": " << scoresWindowsQuality[0] << ", " << endl;
        cout << "\t\"max\": " << scoresWindowsQuality[scoresWindowsQuality.size() - 1] << ", " << endl;
        cout << "\t\"median\": " << this->getMedian(scoresWindowsQuality) << ", " << endl;
        cout << "\t\"mean\": " << this->getMean(scoresWindowsQuality) << ", " << endl;
        cout << "\t\"mode\": " << this->getMode(scoresWindowsQuality) << "}}, " << endl;
    }
}

void NZN::process(string mapPath)
{
    this->processingStartTime = high_resolution_clock::now();
    std::vector<Mat> frames;
    FraudDetectAlgorithm::readMatsFromFile(mapPath, frames);
    const int n = frames.size();
    if (n > 0 && n < MAX_FRAMES)
    {
        std::vector<double> scoresBlur;
        std::vector<double> scoresPca;
        std::vector<double> scoresCnn;
        std::vector<double> scoresInterestPointsQuality;
        std::vector<double> scoresWindowsQuality;

        int steps = 0;
        bitset<MAX_FRAMES> indexes;
        indexes.reset();
        int p = 1 + 2 * n / 3;
        bool timeout = false;
        int step = 0;
        while (steps < n && !timeout)
        {
            for (int i = 0; i < n; i += p)
            {
                if (indexes[i] == 0)
                {
                    steps++;
                    indexes[i] = 1;
                    Mat image = frames.at(i);
                    scoresBlur.push_back(this->testInfo->getBlur(image));
                    scoresPca.push_back(this->testPca->getScore(image));
                    scoresCnn.push_back(this->testCnn->getScore(image));
                    scoresInterestPointsQuality.push_back(this->testQuality->scoreInterestPointsQuality(image, 100));
                    scoresWindowsQuality.push_back(this->testQuality->scoreWindowsQuality(image, 100));

                    elapsed = high_resolution_clock::now() - processingStartTime;
                    double step = elapsed.count() / (double)(steps + 1);
                    if (elapsed.count() + step > limitBySeconds)
                    {
                        timeout = true;
                        break;
                    }
                }
            }
            p = 2 * p / 3;
            printStats(++step, n, FraudDetectAlgorithm::getName(mapPath), scoresBlur, scoresPca, scoresCnn, scoresInterestPointsQuality, scoresWindowsQuality);
        }
        /*elapsed = high_resolution_clock::now() - processingStartTime;
        cout << "Frames: " << n << endl;
        cout << "Processed seconds: " << elapsed.count() << endl;
        cout << "Steps count: " << steps << endl;
        cout << "Seconds per step: " << elapsed.count() / (double)(steps + 1) << endl;*/
    }
    else
    {
        cout << "SOMETHING WRONG" << endl;
    }
}

void NZN::loadFaces(string filename)
{
    string name = FraudDetectAlgorithm::getName(filename);
    string path = FraudDetectAlgorithm::getPath(filename);
    std::vector<Mat> frames;
    FraudDetectAlgorithm::readMatsFromFile(filename, frames);
    const int n = frames.size();
    if (n > 0 && n < MAX_FRAMES)
    {
        for (int i = 0; i < n; i++)
        {
            string s = path + name + PATH_SEPARATOR + to_string(i) + ".png";
            imwrite(s.c_str(), frames.at(i));
        }
    }
}
