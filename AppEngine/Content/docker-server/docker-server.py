
import logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

import falcon
from falcon_cors import CORS
from waitress import serve

import api

from MultiProcessingLog import MultiProcessingLog
logging.getLogger().addHandler(MultiProcessingLog(None, 'a', 0, 0))

falconApi = None
fdfApi = None

cors_allow_all = CORS(allow_all_origins=True,
                      allow_all_headers=True,
                      allow_all_methods=True)
falconApi = falcon.API(middleware=[cors_allow_all.middleware])
fdfApi = api.api(cors_allow_all)
falconApi.add_route('/liveness_check/', fdfApi.extApi.healthCheck(fdfApi))
falconApi.add_route('/readiness_check/', fdfApi.extApi.healthCheck(fdfApi))
falconApi.add_route('/api/v2int/get_result/{sessionId}', fdfApi.extApi.getResult(fdfApi))
falconApi.add_route('/api/v2int/del_result/{sessionId}', fdfApi.extApi.delResult(fdfApi))
falconApi.add_route('/api/v2int/set_photo/{methods}', fdfApi.extApi.setPhoto(fdfApi))
falconApi.add_route('/api/v2int/set_photo/{methods}', fdfApi.extApi.setPhoto(fdfApi))
serve(falconApi, host=fdfApi.host, port=fdfApi.port)
