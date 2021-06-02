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
import sys
import threading
import time

import json

import logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

import pg
import constants

sys.path.append(constants.FDF_PYD_PATH)
os.environ["PATH"] += ";" + constants.FDF_PYD_PATH
import fdf

from MultiProcessingLog import MultiProcessingLog
logging.getLogger().addHandler(MultiProcessingLog("check-server.txt", "a", 0, 0))

class CheckServer(object):
    def __init__(self):
        self.info = fdf.Photos.All.INFO()
        self.cnn = fdf.Photos.All.CNN(json.dumps({
                    "modelPath": "./fdf/m88-1.pb", 
                    "modelInput": "input_input", 
                    "modelOutput": "softmax_tensor/Softmax" }))
        self.pca = fdf.Photos.All.PCA()
        self.benford = fdf.Photos.Jpeg.BENFORD()
        self.exif = fdf.Photos.Jpeg.EXIF()
        self.quality = fdf.Photos.Jpeg.QUALITY()

    def check(self):
        while True:
            task = (None, None, None, None)
            try:
                if os.path.exists("server.stop"):
                    exit()
                task = pg.getTask()
                if not task[0] is None:
                    methods = json.loads(task[4]) if not task[4] is None and task[4].lower() != "null" else ["info", "cnn", "pca", "benford", "exif", "quality"]
                    result = dict()
                    for m in methods:
                        if m == "info": 
                            result[m] = self.info.check(task[0]).as_dict
                        elif m == "cnn": 
                            result[m] = self.cnn.check(task[0]).as_dict
                        elif m == "pca": 
                            result[m] = self.pca.check(task[0]).as_dict
                        elif m == "benford": 
                            result[m] = self.benford.check(task[0]).as_dict
                        elif m == "exif": 
                            result[m] = self.exif.check(task[0]).as_dict
                        elif m == "quality": 
                            result[m] = self.quality.check(task[0]).as_dict
                    pg.setTaskResult(task[2], json.dumps(result), constants.STATUS_COMPLETED)
            except Exception as e:
                message = "CHECK EXCEPTION: {}".format(json.dumps({
                    "exception": MultiProcessingLog.exception2string(e),
                    "user_id": task[3],
                    "source_id": task[1],
                    "task_id": task[2]
                    }))
                logging.info(message)
                pg.addMessage(message, constants.MESSAGE_TYPE_ERROR, task[3])
            finally:
                time.sleep(3)

    def run(self):
        checkThread = threading.Thread(target=self.check, args=())
        checkThread.start()

if __name__ == "__main__":
    CheckServer().run()
