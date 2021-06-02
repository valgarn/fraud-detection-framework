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


class extApi(object):
    def __init__(self, api):
        self.api = api

    class healthCheck(object):
        cors = None

        def __init__(self, api):
            extApi.healthCheck.cors = api.cors_allow_all
            self.api = api

        def on_get(self, request, response):
            response.status = falcon.HTTP_200
            response.body = json.dumps({"status": "OK"})

    class getResult(object):
        cors = None

        def __init__(self, api):
            extApi.getResult.cors = api.cors_allow_all
            self.api = api

        def on_get(self, request, response, sessionId):
            error = None
            try:
                response.status = falcon.HTTP_200
                response.body = self.api.getResults(sessionId)
            except Exception as e:
                error = "/get_result error: {}".format(
                    MultiProcessingLog.exception2string(e)
                )
                logging.info(error)
                response.body = json.dumps({"error": error})

    class delResult(object):
        cors = None

        def __init__(self, api):
            extApi.delResult.cors = api.cors_allow_all
            self.api = api

        def on_get(self, request, response, sessionId):
            error = None
            try:
                if os.path.exists(self.api.results[sessionId]["photo"]):
                    os.remove(self.api.results[sessionId]["photo"])
                self.api.results.pop(sessionId)
                response.status = falcon.HTTP_200
                response.body = json.dumps({"removed": sessionId})
            except Exception as e:
                error = "/del_result error: {}".format(
                    MultiProcessingLog.exception2string(e)
                )
                logging.info(error)
                response.body = json.dumps({"error": error})

    class setPhoto(object):
        cors = None

        def __init__(self, api):
            extApi.setPhoto.cors = api.cors_allow_all
            self.api = api

        def observeDel(self, info):
            finished = True
            res = json.loads(self.api.getResults(info["id"]))
            for k in info["methods"]:
                if res.get(k, None) is None:
                    finished = False
                    break
            if finished and os.path.exists(info["photo"]):
                os.remove(info["photo"])

        def infoProc(self, info):
            try:
                obj = fdf.Photos.All.INFO()
                info["results"]["info"] = obj.check(info["photo"]).as_dict
                self.observeDel(info)
            except Exception as e:
                if not info.get("results", None):
                    info["results"]["error"] = MultiProcessingLog.exception2string(e)

        def cnnProc(self, info):
            try:
                obj = fdf.Photos.All.CNN(
                    json.dumps(
                        {
                            "modelPath": "{}/fdf/m88-1.pb".format(
                                os.path.dirname(os.path.abspath(__file__))
                            ),
                            "modelInput": "input_input",
                            "modelOutput": "softmax_tensor/Softmax",
                        }
                    )
                )
                info["results"]["cnn"] = obj.check(info["photo"]).as_dict
                self.observeDel(info)
            except Exception as e:
                if not info.get("results", None):
                    info["results"]["error"] = MultiProcessingLog.exception2string(e)

        def pcaProc(self, info):
            try:
                obj = fdf.Photos.All.PCA('{ "paranoidLevel": 1.0 }')
                info["results"]["pca"] = obj.check(info["photo"]).as_dict
                self.observeDel(info)
            except Exception as e:
                if not info.get("results", None):
                    info["results"]["error"] = MultiProcessingLog.exception2string(e)

        def benfordProc(self, info):
            try:
                obj = fdf.Photos.Jpeg.BENFORD('{ "paranoidLevel": 1.0 }')
                info["results"]["benford"] = obj.check(info["photo"]).as_dict
                self.observeDel(info)
            except Exception as e:
                if not info.get("results", None):
                    info["results"]["error"] = MultiProcessingLog.exception2string(e)

        def exifProc(self, info):
            try:
                obj = fdf.Photos.Jpeg.EXIF('{ "paranoidLevel": 1.0 }')
                info["results"]["exif"] = obj.check(info["photo"]).as_dict
                self.observeDel(info)
            except Exception as e:
                if not info.get("results", None):
                    info["results"]["error"] = MultiProcessingLog.exception2string(e)

        def qualityProc(self, info):
            try:
                obj = fdf.Photos.Jpeg.QUALITY('{ "paranoidLevel": 1.0 }')
                info["results"]["quality"] = obj.check(info["photo"]).as_dict
                self.observeDel(info)
            except Exception as e:
                if not info.get("results", None):
                    info["results"]["error"] = MultiProcessingLog.exception2string(e)
 
        def on_post(self, request, response, methods):
            error = None
            sessionId = str(uuid.uuid4())
            try:
                self.api.results[sessionId] = self.api.manager.dict()
                upload = cgi.FieldStorage(fp=request.stream, environ=request.env)
                destination = os.path.join(
                    os.path.dirname(os.path.abspath(__file__)),
                    self.api.dataFolder,
                    "{}_{}".format(sessionId, upload["photo"].filename),
                )
                info = {
                    "id": sessionId,
                    "photo": destination,
                    "methods": methods.split("_"),
                    "results": self.api.results[sessionId],
                }
                with open(destination, "wb") as output_file:
                    shutil.copyfileobj(upload["photo"].file, output_file)
                for m in info["methods"]:
                    self.api.results[sessionId][m] = None
                    if m == "info":
                        p = Process(target=self.infoProc, args=(info,))
                        p.start()
                    elif m == "cnn":
                        p = Process(target=self.cnnProc, args=(info,))
                        p.start()
                    elif m == "pca":
                        p = Process(target=self.pcaProc, args=(info,))
                        p.start()
                    elif m == "benford":
                        p = Process(target=self.benfordProc, args=(info,))
                        p.start()
                    elif m == "exif":
                        p = Process(target=self.exifProc, args=(info,))
                        p.start()
                    elif m == "quality":
                        p = Process(target=self.qualityProc, args=(info,))
                        p.start()
                    else:
                        pass
                response.body = json.dumps({"sessionId": str(sessionId)})
            except Exception as e:
                error = "/set_photo error: {}".format(
                    MultiProcessingLog.exception2string(e)
                )
                logging.info(error)
                response.body = json.dumps(
                    {"error": error, "sessionId": str(sessionId)}
                )
