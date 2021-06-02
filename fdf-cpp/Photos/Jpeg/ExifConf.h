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

#ifndef EXIF_CONF_LIB_H
#define EXIF_CONF_LIB_H

#ifdef WIN32
#ifdef BUILDING_FDF_LIB
#define FDF_LIB __declspec(dllexport)
#else
#define FDF_LIB __declspec(dllimport)
#endif
#else
#define FDF_LIB
#endif

#include <set>
#include <vector>
#include <string>
#include <cctype>
#include <locale>
#include <algorithm>
#include <functional>
#include "../../include/rapidjson/document.h"

#include "../../Service/Configuration.h"

using namespace std;

namespace Photos
{
namespace Jpeg
{
class FDF_LIB ExifConf : public Configuration
{
protected:
  set<double> screenRatios = {1.25, 1.3, 1.333333333, 1.336, 1.338843, 1.5, 1.776, 1.77777777, 1.85, 1.9};
  set<int> screenResolutions = {1080, 1180, 1200, 1440, 1536, 1600, 1680, 1920, 1936, 1998,
    2008, 2048, 2160, 2240, 2304, 2448, 2560, 2592, 3032, 3072, 3264, 3840, 4000, 4096, 4320, 4480, 4912, 5304, 5360,
    5368, 5792, 6000, 6720, 7360, 7680, 7712, 7728, 7752, 7816, 7952, 8640, 8688, 10320, 10380, 15360, 34560, 61440};
  vector<string> editors = { "TAGGER", "VIDEOMAGIC", "FOTOCANVAS", "FOTOSLATE", "ARTPLUS", "FOTOANGELO", "ATMOSPHERE", 
    "EXIFPRO", "PIXBURNER", "ASHAMPOO", "PHOTOTHECA", "IDIMAGER", "IMAGESKILL", "CARTOON", "PICAJET", "GELANALYZER", 
    "GEOSETTER", "PICMETA", "PHOTOTRACKER", "OMNIFORMAT", "NKREMOTE", "ARTSONG", "PHOTOMOD", "GEOMOSAIC", "VIRTGUARD", 
    "DIGIFISHVIEWER", "ASTROART", "FASTPICTUREVIEWER", "VINTASOFTIMAGING", "DBPIXMULTIRIP", "MULTIRIP", "MAXMEDIA", 
    "XPICTURE", "WATERMARK", "PENNOCK", "ARTWEAVER", "AUTODESK", "BARCO", "CREATOR", "DARKTABLE", "DAZZLE", 
    "ABILITY", "ACDSEE", "ADOBE", "AFFINITY", "ANIMATION", "APERTURE", "APPLE APP", "ARCSOFT", "ARTIPIC", "ARTRAGE", 
    "BIBBLE", "BLOOM", "BRILLIANCE", "BRUSH", "CAPTURE", "CHASYS", "CODEDCOLOR", "COLOR IT", "COREL", "COSMIGO", 
    "DEGAS", "DELUXE", "DENEBA", "DESIGNER", "DIGIKAM", "DOGWAFFLE", "DRAW", "EDITOR", "ERDAS", "FASTSTONE", "ZONER",
    "FIREWORKS", "FOTOGRAFIX", "F-SPOT", "IMAGEMAGICK", "IMAGEREADY", "MAKER", "MANGA", "MECHANIC", "PHOTODELUXE", 
    "FUNKY", "GIMP", "G'MIC", "GRAFX2", "GRAPHICCONVERTER", "GRAPHICSMAGICK", "GTHUMB", "HELICON", "HUGIN", "IMAGEJ", 
    "IMAGINE", "IPHOTO", "IRFANVIEW", "IRIDIENT", "KID", "KRITA", "LEONARDO", "LIGHTROOM", "LIGHTZONE", "LVIEW", 
    "MIRAFX", "MOOTIF", "NAKED", "NEOCHROME", "OFFICE", "OPENCANVAS", "PAINT", "PANORAMA", "PENCIL2D", "PHATCH", 
    "PHOTOFINISH", "PICMONKEY", "PICNIK", "PICSART", "QUANTEL", "RASTER", "RAWTHERAPEE", "XARA", "XNVIEW", "XRES", 
    "PHOTOGENICS", "PHOTOIMPACT", "PHOTOLINE", "PHOTON", "PHOTOPERFECT", "PHOTOPLUS", "PHOTOS", "PICASA", "PICMASTER", 
    "PINTA", "PIXBUILDER", "PIXELMATOR", "PIXELSTYLE", "PIXEN", "PIXIA", "PIXLR", "PMVIEW", "PORTRAIT", "QFX", 
    "SEASHORE", "SERIF", "SHOTWELL", "SKETCH", "STROKES", "STUDIO", "TEXTURE", "TUX", "UFRAW", "ULEAD", "WINIMAGES"};

public:
  ExifConf(const char *json = "");
  set<double> getScreenRatios();
  void getScreenRatios(set<double> ratios);
  set<int> getScreenResolutions();
  void getScreenResolutions(set<int> resolutions);
  vector<string> getEditors();
  void setEditors(vector<string> editors);
  virtual const char *parse();
  virtual ~ExifConf()
  {
    this->editors.clear();
  }
};
} // namespace Jpeg
} // namespace Photos

#endif // EXIF_CONF_LIB_H
