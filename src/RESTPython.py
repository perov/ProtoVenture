
def read(s):
    "Read a Scheme expression from a string."
    return read_from(tokenize(s))

parse = read

def tokenize(s):
    "Convert a string into a list of tokens."
    return s.replace('(',' ( ').replace(')',' ) ').split()

def read_from(tokens):
    "Read an expression from a sequence of tokens."
    if len(tokens) == 0:
        raise SyntaxError('unexpected EOF while reading')
    token = tokens.pop(0)
    if '(' == token:
        L = []
        while tokens[0] != ')':
            L.append(read_from(tokens))
        tokens.pop(0) # pop off ')'
        return L
    elif ')' == token:
        raise SyntaxError('unexpected )')
    else:
        return atom(token)

def atom(token):
    "Numbers become numbers; every other token is a symbol."
    try: return int(token)
    except ValueError:
        try: return float(token)
        except ValueError:
            return Symbol(token)
            
Symbol = str
          


          
import sys
sys.path.append("C:\\Python2.7.3\\DLLs")

print sys.path

import socket
socket._socket

print "Hello"



import venture_engine


# venture_engine.assume("a", parse("(uniform-continuous r[0.0] r[1.0])"))
# venture_engine.observe(parse("(normal a r[0.01])"), "r[0.7]")

# while True:
  # print venture_engine.report_value(1)
  # venture_engine.infer(1)


# venture_engine.assume("order", parse("(uniform-discrete c[0] c[4])"))
# venture_engine.assume("noise", parse("(uniform-continuous r[0.1] r[1.0])"))
# venture_engine.assume("c0", parse("(if (>= order c[0]) (normal r[0.0] r[10.0]) r[0.0])"))
# venture_engine.assume("c1", parse("(if (>= order c[1]) (normal r[0.0] r[1]) r[0.0])"))
# venture_engine.assume("c2", parse("(if (>= order c[2]) (normal r[0.0] r[0.1]) r[0.0])"))
# venture_engine.assume("c3", parse("(if (>= order c[3]) (normal r[0.0] r[0.01]) r[0.0])"))
# venture_engine.assume("c4", parse("(if (>= order c[4]) (normal r[0.0] r[0.001]) r[0.0])"))
# venture_engine.assume("clean-func", parse("(lambda (x) (+ c0 (* c1 (power x r[1.0])) (* c2 (power x r[2.0])) (* c3 (power x r[3.0])) (* c4 (power x r[4.0]))))"))
# venture_engine.predict(parse("(list order c0 c1 c2 c3 c4 noise)"))
# a = venture_engine.observe(parse("(normal (clean-func (normal r[5.6] noise)) noise)"), "r[1.8]")
# venture_engine.infer(1000);
# venture.forget(8)
# venture.forget(9)

# venture_engine.assume("power-law", parse("(lambda (prob x) (if (flip prob) x (power-law prob (+ x 1))))"))
# venture_engine.assume("a", parse("(power-law 0.3 1)"))
# (last_directive, _) = venture_engine.predict(parse("(< a 5)"))
# venture_engine.infer(1000)

import flask
from flask import request
from flask import make_response

# Why it disables the output for the flask?
import logging
flask_log = logging.getLogger("werkzeug")
flask_log.setLevel(logging.DEBUG) # For some reason WARNING and ERROR! still prints requests to the console.

try: # See for details: http://stackoverflow.com/questions/791561/python-2-5-json-module
    import json
except ImportError:
    import simplejson as json 

# Not necessary?: import util

def get_response(string):
    resp = make_response(string)
    #resp.status_code = 201
    #resp.data = json.dumps(directives)
    resp.headers['Access-Control-Allow-Origin'] = '*'
    return resp
    
global app
app = flask.Flask(__name__)

@app.route('/assume', methods=['POST'])
def assume():
  name_str = json.loads(request.form["name_str"])
  expr_lst = json.loads(request.form["expr_lst"])
  print expr_lst
  (directive_id, value) = venture_engine.assume(name_str, expr_lst)
  return get_response(json.dumps({"d_id": directive_id,
                                  "val": value}))
             
@app.route('/predict', methods=['POST'])
def predict():
  expr_lst = json.loads(request.form["expr_lst"])
  (directive_id, value) = venture_engine.predict(expr_lst)
  return get_response(json.dumps({"d_id": directive_id, "val": value}))
        
@app.route('/observe', methods=['POST'])
def observe():
  expr_lst = json.loads(request.form["expr_lst"])
  literal_val = json.loads(request.form["literal_val"])
  directive_id = venture_engine.observe(expr_lst, literal_val)
  return get_response(json.dumps({"d_id": directive_id}))

@app.route('/start_cont_infer', methods=['POST'])
def start_cont_infer():
  venture_engine.start_continuous_inference();
  return get_response(json.dumps({"started": True}))
  
@app.route('/cont_infer_status', methods=['GET'])
def cont_infer_status():
  print "Necessary to add additional information!"
  if venture_engine.continuous_inference_status():
    return get_response(json.dumps({"status": "on"}))
  else:
    return get_response(json.dumps({"status": "off"}))
  
# Check for DELETE!
@app.route('/', methods=['POST'])
def clear():
  venture_engine.clear();
  return get_response(json.dumps({"cleared": True}))
  
@app.route('/<int:directive_id>', methods=['GET'])
def report_value(directive_id):
  current_value = venture_engine.report_value(directive_id);
  return get_response(json.dumps({"val": current_value}))
  
@app.route('/', methods=['GET'])
def report_directives():
  directives = venture_engine.report_directives();
  return get_response(json.dumps({"val": directives}))
  
@app.route('/<int:directive_id>', methods=['POST'])
# Check for DELETE!
def forget(directive_id):
  venture_engine.forget(directive_id)
  return get_response(json.dumps({"okay": True}))

@app.route('/infer', methods=['POST'])
def infer():
    MHiterations = json.loads(request.form["MHiterations"])
    t = venture_engine.infer(MHiterations)
    return get_response(json.dumps({"time": -1}))

@app.errorhandler(Exception)
def special_exception_handler(error):
  print "Error: " + str(error)
  return get_response("Your query has invoked an error:\n" + str(error)), 500
  
  
#try:
app.config['DEBUG'] = False
app.config['TESTING'] = False
app.run(port=8081)
