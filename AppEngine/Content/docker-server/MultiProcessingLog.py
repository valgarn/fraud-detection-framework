
import sys
import _io
import time
import logging
from logging.handlers import RotatingFileHandler
import multiprocessing
import threading
import traceback

class MultiProcessingLog(logging.Handler):
    def __init__(self, name, mode, maxsize, rotate):
        logging.Handler.__init__(self)
        if name is None:
            self._handler = sys.stdout
        else:
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
                print(type(self._handler))
                if isinstance(self._handler, _io.TextIOWrapper):
                    print(record)
                else:
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
        