
# Expected location: /home/ec2-user/for_IME/

import flask
from checker_functions import *

rerunner_server = flask.Flask(__name__)

@rerunner_server.route('/', methods=['GET'])
def just():
  return flask.make_response(str(PORT))

@rerunner_server.route('/restart_venture_server', methods=['GET'])
def restart_venture_server():
  RestartVentureServer('by_http_request')
  return flask.make_response("The Venture server has been restarted")
                                  
rerunner_server.run(port=5300, host='0.0.0.0')
