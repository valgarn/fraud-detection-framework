
from multiprocessing import Manager
import uuid
import json
import extApi

class api(object):
    def __init__(self, cors_allow_all=None):
        self.manager = Manager()
        self.results = self.manager.dict()
        self.host = '0.0.0.0'
        self.port = 8080
        self.dataFolder = "Data"
        self.cors_allow_all = cors_allow_all
        self.extApi = extApi.extApi(self)

    def getResults(self, sessionId):
        return json.dumps(self.results[sessionId].copy())
