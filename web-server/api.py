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

import uuid

import extApiV1
import extApiV2

class api(object):
    def __init__(self, cors_allow_all=None):
        self.methods = dict()
        self.host = '0.0.0.0'
        self.port = 5001
        self.dataPath = "./Data/"
        self.sessionId = uuid.uuid4()
        self.cors_allow_all = cors_allow_all
        self.extApiV1 = extApiV1.extApiV1(self)
        self.extApiV2 = extApiV2.extApiV2(self)

