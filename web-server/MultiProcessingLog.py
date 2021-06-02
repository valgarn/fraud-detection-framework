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

import sys
import time
import logging
from logging.handlers import RotatingFileHandler
import multiprocessing
import threading
import traceback

class MultiProcessingLog(logging.Handler):
    def __init__(self, name, mode, maxsize, rotate):
        logging.Handler.__init__(self)
        self._handler = RotatingFileHandler(name, mode, maxsize, rotate)
        self.queue = multiprocessing.Queue(-1)
        t = threading.Thread(target=self.receive)
        t.daemon = True
        t.start()

    @staticmethod
    def exception2string(exception):
        stack = traceback.extract_stack()[:-3] + traceback.extract_tb(exception.__traceback__)
        pretty = traceback.format_list(stack)
        return ''.join(pretty) + '\n  {} {}'.format(exception.__class__, exception)

    def setFormatter(self, fmt):
        logging.Handler.setFormatter(self, fmt)
        self._handler.setFormatter(fmt)

    def receive(self):
        while True:
            try:
                record = self.queue.get()
                self._handler.emit(record)
            except (KeyboardInterrupt, SystemExit):
                raise
            except EOFError:
                break
            except:
                traceback.print_exc(file=sys.stderr)

    def send(self, s):
        self.queue.put(s, False)

    def _format_record(self, record):
        # ensure that exc_info and args
        # have been stringified.  Removes any chance of
        # unpickleable things inside and possibly reduces
        # message size sent over the pipe
        if record.args:
            record.msg = record.msg % record.args
            record.args = None
        if record.exc_info:
            dummy = self.format(record)
            record.exc_info = None

        record.msg = "{} - {}".format(time.strftime("%Y %b %d %H:%M:%S"), record.msg)
        return record

    def emit(self, record):
        try:
            s = self._format_record(record)
            self.send(s)
        except (KeyboardInterrupt, SystemExit):
            raise
        except:
            self.handleError(record)

    def close(self):
        self._handler.close()
        logging.Handler.close(self)
        