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

import json

import logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

import falcon
from falcon_cors import CORS
from waitress import serve
from falcon_auth import FalconAuthMiddleware, BasicAuthBackend
from falcon.http_status import HTTPStatus

import pg
import api

from MultiProcessingLog import MultiProcessingLog
logging.getLogger().addHandler(MultiProcessingLog('web-server.txt', 'a', 0, 0))

def get_all_routes(api):
    routes_list = []

    def get_children(node):
        if len(node.children):
            for child_node in node.children:
                get_children(child_node)
        else:
            routes_list.append((node.uri_template, node.resource))
    [get_children(node) for node in api._router._roots]
    return routes_list

falconApi = None
fdfApi = None

cors_allow_all = CORS(allow_all_origins=True,
                      allow_all_headers=True,
                      allow_all_methods=True)

# should be commented for V2 docker version
auth_backend = BasicAuthBackend(pg.checkLogin)
auth_middleware = FalconAuthMiddleware(auth_backend, exempt_methods=['HEAD', 'OPTIONS'])

falconApi = falcon.API(middleware=[cors_allow_all.middleware, auth_middleware])
fdfApi = api.api(cors_allow_all)

falconApi.add_route('/api/v1/get_result/{photo_id:int}', fdfApi.extApiV1.getResult(fdfApi))
falconApi.add_route('/api/v1/get_photo/{photo_id:int}', fdfApi.extApiV1.getPhoto(fdfApi))
falconApi.add_route('/api/v1/get_sources_queue_size/', fdfApi.extApiV1.getSourcesQueueSize(fdfApi))
falconApi.add_route('/api/v1/set_photo/{methods}', fdfApi.extApiV1.setPhoto(fdfApi))
falconApi.add_route('/api/v1/set_photo/', fdfApi.extApiV1.setPhoto(fdfApi))
falconApi.add_route('/api/v1/login/', fdfApi.extApiV1.login(fdfApi))

falconApi.add_route('/liveness_check/', fdfApi.extApiV2.healthCheck(fdfApi))
falconApi.add_route('/readiness_check/', fdfApi.extApiV2.healthCheck(fdfApi))
falconApi.add_route('/api/v2int/get_result/', fdfApi.extApiV2.getResult(fdfApi))
falconApi.add_route('/api/v2int/set_photo/{methods}', fdfApi.extApiV2.setPhoto(fdfApi))

falconApi.req_options.strip_url_path_trailing_slash = True

serve(falconApi, host=fdfApi.host, port=fdfApi.port)
