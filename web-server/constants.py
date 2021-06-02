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

REQUEST_TIMEOUT = (60, 180)

MESSAGE_TYPE_INFO = 5
MESSAGE_TYPE_WARNING = 4
MESSAGE_TYPE_ERROR = 3

DB_CONNECTION_STRING = 'postgresql://postgres:password@localhost:5432/fdf'

EXCEPTION_WAIT_SEC = 5

SETTING_STATUS_NAME = 'status'
SETTING_STATUS_PROCESSING = 'processing'
SETTING_STATUS_STOPPED = 'stopped'
SETTING_STATUS_RELOAD = 'reload'
SETTING_STATUS_CLEAN = 'clean'
SETTING_STATUS_PREPARING = 'preparing'
SETTING_STATUS_PREPARED = 'prepared'
SETTING_STATUS_PAUSED = 'paused'

SETTING_REFRESH_DATA_NAME = 'refreshData'
SETTING_REFRESH_DATA_TRUE = '1'
SETTING_REFRESH_DATA_FALSE = '0'

DATA_FOLDER = 'Data'

TF_LOG_LEVEL = "2"

TYPE_PHOTO_FRAUD_DETECTION = 1

FDF_PYD_PATH = "./fdf"

STATUS_NONE = 0
STATUS_COMPLETED = 1
