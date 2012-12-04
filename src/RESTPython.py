
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




class VentureValueBaseClass:
  def CreateCapsule(self, value_as_string):
    self.capsule_object = venture_engine.NewVentureValue(self.venture_type, str(value_as_string))

  def GetCapsuleObject(self):
    return self.capsule_object
    
  def __del__(self):
    print "Hello"
    # venture_engine.DeleteVentureValue(self.capsule_object)
    
class VentureCount(VentureValueBaseClass):
  def __init__(self, value_as_string):
    self.venture_type = "count";
    self.CreateCapsule(value_as_string)
    
class VentureSymbol(VentureValueBaseClass):
  def __init__(self, value_as_string):
    self.venture_type = "symbol";
    self.CreateCapsule(value_as_string)
    
class VentureList(VentureValueBaseClass):
  def __init__(self):
    self.venture_type = "list";
    self.CreateCapsule("nil")
  def AddToList(venture_value):
    venture_engine.AddToList(self.capsule_object, venture_value.GetCapsuleObject())
    
def ProcessTokens(expression):
  if isInstance(expression, list):
    new_list = VentureList()
    for element in expression:
      new_list.AddToList(ProcessTokens(element)) # This function is now implemented not efficiently
                                                 # in Venture, because to add a new element
                                                 # it goes up to the end of the list.
                                                 # (i.e. it is O(n))
    return new_list
  else:
    if element[:2] == "c[":
      return VentureCount(element[2:-1])
    elif element[:2] == "s[":
      return VentureSmoothed(element[2:-1])
    else:
      return VentureSymbol(element) # Not very wisely.
                                    # Though the Venture will make
                                    # the check for VentureSymbol's name
                                    # restrictions.
                                    
# VentureCount("5")    
  
venture_engine.NewVentureValue("count", "5")

import flask
from flask import request
from flask import make_response

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
    
# Back parser
# Very stupid :), but for the present
def ParseElementBack(element):
  if not isinstance(element, list):
    return str(element)
  else:
    output = "("
    first = True
    for subelement in element:
      if first == False:
        output += " "
      first = False
      output += ParseElementBack(subelement)
    output += ")"
    return output

global app
app = flask.Flask(__name__)

@app.route('/assume', methods=['POST'])
def assume():
  name_str = ParseElementBack(json.loads(request.form["name_str"]))
  expr_lst = ParseElementBack(json.loads(request.form["expr_lst"]))
  directive_code = "(ASSUME " + name_str + " " + expr_lst + ")"
  directive_id = venture_cpp.execute_directive(directive_code)
  val = venture_cpp.report_value(directive_id)
  return get_response(json.dumps({"d_id": directive_id,
                                  "val": val}))
                     
@app.route('/observe', methods=['POST'])
def observe():
  expr_lst = ParseElementBack(json.loads(request.form["expr_lst"]))
  literal_val = ParseElementBack(json.loads(request.form["literal_val"]))
  directive_code = "(OBSERVE " + expr_lst + " " + literal_val + ")"
  directive_id = venture_cpp.execute_directive(directive_code)
  return get_response(json.dumps({"d_id": directive_id}))

@app.route('/predict', methods=['POST'])
def predict():
  expr_lst = ParseElementBack(json.loads(request.form["expr_lst"]))
  directive_code = "(PREDICT " + expr_lst + ")"
  directive_id = venture_cpp.execute_directive(directive_code)
  val = venture_cpp.report_value(directive_id)
  return get_response(json.dumps({"d_id": directive_id, "val": val}))

@app.route('/start_cont_infer', methods=['POST'])
def start_cont_infer():
  venture_cpp.start_continuous_inference();
  return get_response(json.dumps({"started": True}))
  
@app.route('/cont_infer_status', methods=['GET'])
def cont_infer_status():
  print "Necessary to add additional information!"
  if venture_cpp.continuous_inference_status():
    return get_response(json.dumps({"status": "on"}))
  else:
    return get_response(json.dumps({"status": "off"}))
  
# Check for DELETE!
@app.route('/', methods=['POST'])
def clear():
  venture_cpp.clear();
  return get_response(json.dumps({"cleared": True}))
  
@app.route('/<int:directive_id>', methods=['GET'])
def report_value(directive_id):
  current_value = venture_cpp.report_value(directive_id);
  return get_response(json.dumps({"val": parse(current_value)}))
  
@app.route('/<int:directive_id>', methods=['POST'])
# Check for DELETE!
def forget(directive_id):
  try:
    venture_cpp.forget(directive_id)
    return get_response(json.dumps({"okay": "okay"}))
  except Exception, err:
    print "Error has happened: " + err

  
#try:
app.run(port=8081)
  
  
  