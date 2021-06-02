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

import os
import json
import shutil
import uuid
import cgi

import logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

import falcon

import pg
import constants

from MultiProcessingLog import MultiProcessingLog

class extApiV1(object):
    def __init__(self, api):
        self.api = api

    class getSourcesQueueSize(object):
        cors = None
        def __init__(self, api):
            extApiV1.getSourcesQueueSize.cors = api.cors_allow_all
            self.api = api
        def on_get(self, _, response):
            error = None
            try:
                if pg.getSetting(constants.SETTING_STATUS_NAME) == constants.SETTING_STATUS_PROCESSING:
                    size = pg.getSourcesQueueSize()
                    response.status = falcon.HTTP_200
                    response.body = json.dumps({
                        "size": size,
                        constants.SETTING_STATUS_NAME: pg.getSetting(constants.SETTING_STATUS_NAME),
                        "sessionId": str(self.api.sessionId)
                    })
                    return
            except Exception as e:
                error = "/api/v1/get_result error: {}".format(MultiProcessingLog.exception2string(e))
                logging.info(error)
                if pg.getSetting(constants.SETTING_STATUS_NAME) != constants.SETTING_STATUS_CLEAN:
                    pg.addMessage(error, constants.MESSAGE_TYPE_ERROR, -1)
            response.body = json.dumps({
                "error": error
            })

    class getResult(object):
        cors = None
        def __init__(self, api):
            extApiV1.getResult.cors = api.cors_allow_all
            self.api = api
        def on_get(self, request, response, photo_id):
            error = None
            try:
                if pg.getSetting(constants.SETTING_STATUS_NAME) == constants.SETTING_STATUS_PROCESSING:
                    (result, userId) = pg.getSourceResult(photo_id)[0:2]
                    if not userId is None and not result is None:
                        if request.context["user"] == userId:
                            response.status = falcon.HTTP_200
                            response.body = result
                            return
                        else:
                            error = "Something happened. The admin has been notified."
                if error is None:
                    response.body = json.dumps({
                        constants.SETTING_STATUS_NAME: pg.getSetting(constants.SETTING_STATUS_NAME),
                        "sessionId": str(self.api.sessionId)
                    })
                    return
            except Exception as e:
                error = "/api/v1/get_result error: {}".format(MultiProcessingLog.exception2string(e))
                logging.info(error)
                if pg.getSetting(constants.SETTING_STATUS_NAME) != constants.SETTING_STATUS_CLEAN:
                    pg.addMessage(error, constants.MESSAGE_TYPE_ERROR, -1)
            response.body = json.dumps({
                "error": error
            })

    class getPhoto(object):
        cors = None
        def __init__(self, api):
            extApiV1.getPhoto.cors = api.cors_allow_all
            self.api = api
        def on_get(self, request, response, photo_id):
            error = None
            try:
                if pg.getSetting(constants.SETTING_STATUS_NAME) == constants.SETTING_STATUS_PROCESSING:
                    (photo, userId) = pg.getSource(photo_id)[0:2]
                    if not userId is None:
                        if request.context["user"] == userId:
                            response.status = falcon.HTTP_200
                            response.content_type = 'image/*'
                            with open(photo, 'br') as f:
                                response.body = f.read()
                            return
                        else:
                            error = "Something happened. The admin has been notified."
                if error is None:
                    response.body = json.dumps({
                        constants.SETTING_STATUS_NAME: pg.getSetting(constants.SETTING_STATUS_NAME),
                        "sessionId": str(self.api.sessionId)
                    })
                    return
            except Exception as e:
                error = "/api/v1/get_photo error: {}".format(
                    MultiProcessingLog.exception2string(e))
                logging.info(error)
                if pg.getSetting(constants.SETTING_STATUS_NAME) != constants.SETTING_STATUS_CLEAN:
                    pg.addMessage(error, constants.MESSAGE_TYPE_ERROR, -1)
            response.body = json.dumps({
                "error": error,
                constants.SETTING_STATUS_NAME: pg.getSetting(constants.SETTING_STATUS_NAME),
                "sessionId": str(self.api.sessionId)
            })

    class setPhoto(object):
        cors = None
        def __init__(self, api):
            extApiV1.setPhoto.cors = api.cors_allow_all
            self.api = api
        def on_post(self, request, response, methods=None):
            self.methods = methods.split("_") if methods else None
            error = None
            try:
                if pg.getSetting(constants.SETTING_STATUS_NAME) == constants.SETTING_STATUS_PROCESSING and not request.context["user"] is None:
                    upload = cgi.FieldStorage(fp=request.stream, environ=request.env)
                    filename = upload['photo'].filename.encode('ascii', 'ignore').decode()
                    destination = os.path.join(self.api.dataPath, "{}_{}".format(str(uuid.uuid4()), filename))
                    with open(destination, 'wb') as output_file:
                        shutil.copyfileobj(upload['photo'].file, output_file)
                    identity = pg.addSource(destination, request.context["user"], constants.TYPE_PHOTO_FRAUD_DETECTION, json.dumps(self.methods))
                    response.body = json.dumps({
                        "identity": identity,
                        constants.SETTING_STATUS_NAME: pg.getSetting(constants.SETTING_STATUS_NAME),
                        "sessionId": str(self.api.sessionId)
                    })
                    return
                if error is None:
                    response.body = json.dumps({
                        constants.SETTING_STATUS_NAME: pg.getSetting(constants.SETTING_STATUS_NAME),
                        "sessionId": str(self.api.sessionId)
                    })
                    return
            except Exception as e:
                error = "/api/v1/set_photo error: {}".format(MultiProcessingLog.exception2string(e))
                logging.info(error)
                if pg.getSetting(constants.SETTING_STATUS_NAME) != constants.SETTING_STATUS_CLEAN:
                    pg.addMessage(error, constants.MESSAGE_TYPE_ERROR, -1)
            response.body = json.dumps({
                "error": error,
                constants.SETTING_STATUS_NAME: pg.getSetting(constants.SETTING_STATUS_NAME),
                "sessionId": str(self.api.sessionId)
            })

    class login(object):
        cors = None
        def __init__(self, api):
            extApiV1.login.cors = api.cors_allow_all
            self.api = api
        def on_get(self, _, response):
            error = None
            try:
                response.status = falcon.HTTP_200
                if pg.getSetting(constants.SETTING_STATUS_NAME) == constants.SETTING_STATUS_PROCESSING:
                    response.body = json.dumps({
                        constants.SETTING_STATUS_NAME: pg.getSetting(constants.SETTING_STATUS_NAME),
                        "sessionId": str(self.api.sessionId)
                    })
                    return
                if error is None:
                    response.body = json.dumps({
                        constants.SETTING_STATUS_NAME: pg.getSetting(constants.SETTING_STATUS_NAME),
                        "sessionId": str(self.api.sessionId)
                    })
                    return
            except Exception as e:
                error = "/api/v1/login error: {}".format(MultiProcessingLog.exception2string(e))
                logging.info(error)
                if pg.getSetting(constants.SETTING_STATUS_NAME) != constants.SETTING_STATUS_CLEAN:
                    pg.addMessage(error, constants.MESSAGE_TYPE_ERROR, -1)
            response.body = json.dumps({
                "error": error,
                constants.SETTING_STATUS_NAME: pg.getSetting(constants.SETTING_STATUS_NAME),
                "sessionId": str(self.api.sessionId)
            })
