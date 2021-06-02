# Copyright 2021 The Fraud Detection Framework Authors.  All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND< either express or implied.  See the
# License for the specific language governing permissions and limitations under
# the License.

from multiprocessing import Process, Queue
from MultiProcessingLog import MultiProcessingLog

import os
import sys
import json
import shutil
import uuid
import cgi
import falcon

import logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

import constants

sys.path.append(constants.FDF_PYD_PATH)
os.environ["PATH"] += ";" + constants.FDF_PYD_PATH
import fdf

class extApiV2(object):
    def __init__(self, api):
        self.api = api

    class healthCheck(object):
        cors = None

        def __init__(self, api):
            extApiV2.healthCheck.cors = api.cors_allow_all
            self.api = api

        def on_get(self, request, response):
                response.status = falcon.HTTP_200
                response.body = json.dumps({
                    "status": "OK"
                })

    class getResult(object):
        cors = None

        def __init__(self, api):
            extApiV2.getResult.cors = api.cors_allow_all
            self.api = api

        def on_get(self, request, response):
            error = None
            try:
                result = dict()
                for k in self.api.methods.keys():
                    if not isinstance(self.api.methods[k], dict) and not self.api.methods[k].empty():
                        result[k] = self.api.methods[k].get_nowait()
                        self.api.methods[k] =  result[k]
                    elif isinstance(self.api.methods[k], dict):
                        result[k] = self.api.methods[k]
                    else:
                        result[k] = None
                response.status = falcon.HTTP_200
                response.body = json.dumps(result)
            except Exception as e:
                error = "/get_result error: {}".format(MultiProcessingLog.exception2string(e))
                logging.info(error)
                response.body = json.dumps({
                    "error": error
                })

    class setPhoto(object):
        cors = None

        def __init__(self, api):
            extApiV2.setPhoto.cors = api.cors_allow_all
            self.api = api

        def infoProc(self, photo, result):
            obj = fdf.Photos.All.INFO()
            result.put(obj.check(photo).as_dict)

        def cnnProc(self, photo, result):
            obj = fdf.Photos.All.CNN('{{ "modelPath": "{}/fdf/fdf_graph.pb", "paranoidLevel": 1.0 }}'.format(os.path.dirname(os.path.abspath(__file__))))
            result.put(obj.check(photo).as_dict)

        def pcaProc(self, photo, result):
            obj = fdf.Photos.All.PCA('{ "paranoidLevel": 1.0 }')
            result.put(obj.check(photo).as_dict)

        def benfordProc(self, photo, result):
            obj = fdf.Photos.Jpeg.BENFORD('{ "paranoidLevel": 1.0 }')
            result.put(obj.check(photo).as_dict)

        def exifProc(self, photo, result):
            obj = fdf.Photos.Jpeg.EXIF('{ "paranoidLevel": 1.0 }')
            result.put(obj.check(photo).as_dict)

        def qualityProc(self, photo, result):
            obj = fdf.Photos.Jpeg.QUALITY('{ "paranoidLevel": 1.0 }')
            result.put(obj.check(photo).as_dict)

        def on_post(self, request, response, methods):
            self.methods = methods.split("_")
            error = None
            try:
                    upload = cgi.FieldStorage(fp=request.stream, environ=request.env)
                    destination = os.path.join(os.path.dirname(os.path.abspath(__file__)), self.api.dataFolder, \
                                    "{}_{}".format(str(uuid.uuid4()), upload['photo'].filename))
                    with open(destination, 'wb') as output_file:
                        shutil.copyfileobj(upload['photo'].file, output_file)
                    for m in self.methods:
                        if m=="info":
                            self.api.methods["info"] = Queue()
                            p = Process(target=self.infoProc, args=(destination,self.api.methods["info"],))
                            p.start()
                        elif m=="cnn":
                            self.api.methods["cnn"] = Queue()
                            p = Process(target=self.cnnProc, args=(destination,self.api.methods["cnn"],))
                            p.start()
                        elif m=="pca":
                            self.api.methods["pca"] = Queue()
                            p = Process(target=self.pcaProc, args=(destination,self.api.methods["pca"],))
                            p.start()
                        elif m=="benford":
                            self.api.methods["benford"] = Queue()
                            p = Process(target=self.benfordProc, args=(destination,self.api.methods["benford"],))
                            p.start()
                        elif m=="exif":
                            self.api.methods["exif"] = Queue()
                            p = Process(target=self.exifProc, args=(destination,self.api.methods["exif"],))
                            p.start()
                        elif m=="quality":
                            self.api.methods["quality"] = Queue()
                            p = Process(target=self.qualityProc, args=(destination,self.api.methods["quality"],))
                            p.start()
                        else:
                            pass
                    response.body = json.dumps({
                        "sessionId": str(self.api.sessionId)
                    })
            except Exception as e:
                error = "/set_photo error: {}".format(MultiProcessingLog.exception2string(e))
                logging.info(error)
                response.body = json.dumps({
                    "error": error,
                    "sessionId": str(self.api.sessionId)
                })
