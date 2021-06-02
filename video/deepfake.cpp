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

#if defined(WIN32) || defined(_WIN32) || defined(__CYGWIN__) 
    #define PATH_SEPARATOR "\\" 
#else 
    #define PATH_SEPARATOR "/" 
#endif

int main(int argc, char *argv[])
{
    /*string path = argv[1];
    string filename = path;
    string::size_type i = filename.find_last_of(PATH_SEPARATOR);
    if (i != string::npos)
    {
        filename = filename.substr(i + 1);
    }
    i = filename.find_last_of('.');
    if (i != string::npos && i != 0)
    {
        filename = filename.substr(0, i);
    }*/
    //cout << filename << endl;

    NZN *nzn = new NZN();
    //nzn->loadFaces(path);
    //nzn->cutFacesToMapsFile(path);
    nzn->process(argv[1]);
    // deepfake /Users/val/Projects/FDF/DEEPFAKES/maps/test_fake_maps/2084869_2005778_C_000.maps

    return 0;
}
