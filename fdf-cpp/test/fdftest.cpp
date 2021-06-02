
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "../Service/SuspiciousInfo.h"
#include "../Photos/Jpeg/JpegQualityAlgorithm.h"
#include "../Photos/Jpeg/JpegExifAlgorithm.h"
#include "../Photos/Jpeg/JpegBenfordAlgorithm.h"
#include "../Photos/All/PhotosPcaAlgorithm.h"
#include "../Photos/All/CnnConf.h"
#include "../Photos/All/InfoConf.h"
#include "../Photos/Jpeg/BenfordConf.h"
#include "../Photos/All/PhotosCnnAlgorithm.h"
#include "../Photos/All/PhotosInfoAlgorithm.h"

using namespace std;
using namespace Photos::All;
using namespace Photos::Jpeg;

int main(int argc, char *argv[])
{
        SuspiciousInfo *s = 0;

        bool info = true;
        bool quality = true;
        bool pca = true;
        bool exif = true;
        bool benford = true;
        bool cnn = true;        
        PhotosInfoAlgorithm testInfo;
        JpegQualityAlgorithm testQuality;
        PhotosPcaAlgorithm testPca;
        JpegExifAlgorithm testExif;
        JpegBenfordAlgorithm testBenford;
        PhotosCnnAlgorithm testCnn(new CnnConf("{ \"modelPath\": \"../Service/fdf_graph.pb\" }"));

        while(true) {
                s = testCnn.check(argv[1]);
                double p = s->getProbability();
                delete s;
        }

        ofstream outfile;
        string goodLogPath = "/Users/val/Projects/FDF/_REPOSITORY/fdf-cpp/good.log";
        string badLogPath = "/Users/val/Projects/FDF/_REPOSITORY/fdf-cpp/bad.log";
        ostringstream o;
        double maxProbability = 0;
        double p;

        o << "{ \"Filename\": \"" << argv[1] << "\", \"Results\": [";        

        if (info)
        {
                s = testInfo.check(argv[1]);
                p = s->getProbability();
                cout << "{ \"Name\": \"INFO\", " << "\"Probability\": " << p << ", " << "\"Comments\": \"" << s->getComments() << "\"}, " << endl;
                o << "{ \"Name\": \"INFO\", " << "\"Probability\": " << p << ", " << "\"Comments\": \"" << s->getComments() << "\"}, " << endl;
                delete s;
        }

        if (quality)
        {
                s = testQuality.check(argv[1]);
                p = s->getProbability();
                if (p > maxProbability)
                {
                        maxProbability = p;
                }
                cout << "{ \"Name\": \"QUALITY\", " << "\"Probability\": " << p << ", " << "\"Comments\": \"" << s->getComments() << "\"}, " << endl;
                o << "{ \"Name\": \"QUALITY\", " << "\"Probability\": " << p << ", " << "\"Comments\": \"" << s->getComments() << "\"}, " << endl;
                delete s;
        }

        if (pca)
        {
                s = testPca.check(argv[1]);
                p = s->getProbability();
                if (p > maxProbability)
                {
                        maxProbability = p;                
                }
                cout << "{ \"Name\": \"PCA\", " << "\"Probability\": " << p << ", " << "\"Comments\": \"" << s->getComments() << "\"}, " << endl;
                o << "{ \"Name\": \"PCA\", " << "\"Probability\": " << p << ", " << "\"Comments\": \"" << s->getComments() << "\"}, " << endl;
                delete s;
        }

        if (exif)
        {
                s = testExif.check(argv[1]);
                p = s->getProbability();
                if (p > maxProbability)
                {
                        maxProbability = p;
                }
                cout << "{ \"Name\": \"EXIF\", " << "\"Probability\": " << p << ", " << "\"Comments\": \"" << s->getComments() << "\"}, " << endl;
                o << "{ \"Name\": \"EXIF\", " << "\"Probability\": " << p << ", " << "\"Comments\": \"" << s->getComments() << "\"}, " << endl;
                delete s;
        }

        if (benford)
        {
                s = testBenford.check(argv[1]);
                p = s->getProbability();
                if (p > maxProbability)
                {
                        maxProbability = p;                
                }
                cout << "{ \"Name\": \"BENFORD\", " << "\"Probability\": " << p << ", " << "\"Comments\": \"" << s->getComments() << "\"}, " << endl;
                o << "{ \"Name\": \"BENFORD\", " << "\"Probability\": " << p << ", " << "\"Comments\": \"" << s->getComments() << "\"}, " << endl;
                delete s;
        }

        if (cnn)
        {
                s = testCnn.check(argv[1]);
                p = s->getProbability();
                if (p > maxProbability)
                {
                        maxProbability = p;                
                }
                cout << "{ \"Name\": \"CNN\", " << "\"Probability\": " << p << ", " << "\"Comments\": \"" << s->getComments() << "\"}, " << endl;
                o << "{ \"Name\": \"CNN\", " << "\"Probability\": " << p << ", " << "\"Comments\": \"" << s->getComments() << "\"}, " << endl;
                delete s;
        }

        o << "] }" << endl;
        
        if (badLogPath.length() > 0 && maxProbability > 0.5)
        {
                outfile.open(badLogPath.c_str(), ios::app);
                outfile << o.str().c_str();
                outfile.close();
        }
        else if (goodLogPath.length() > 0 && maxProbability <= 0.5)
        {
                outfile.open(goodLogPath.c_str(), ios::app);
                outfile << o.str().c_str();
                outfile.close();
        }
        return 0;
}
