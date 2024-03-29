
def loadToRIPL(ripl, generative_model_string): # Jay Baxter, March 04 2013
    import re
    """Convert a Church generative model string into a sequence of Venture directives."""
    lines = re.findall(r'\[(.*?)\]', generative_model_string, re.DOTALL)
    for line in lines:
        arguments = line.split(" ", 1)
        directive_name = arguments[0].lower()
        if (directive_name == "assume"):
          name_and_expression = arguments[1].split(" ", 1)
          ripl.assume(name_and_expression[0], parse(name_and_expression[1]))
        elif (directive_name == "predict"):
          name_and_expression = arguments[1].split(" ", 1)
          ripl.predict(parse(arguments[1]))
        elif (directive_name == "observe"):
          expression_and_literal_value = arguments[1].rsplit(" ", 1)
          ripl.observe(parse(expression_and_literal_value[0]), expression_and_literal_value[1])
        elif (directive_name == "infer"):
          ripl.infer(int(arguments[1]))
        elif (directive_name == "forget"):
          ripl.forget(int(arguments[1]))
        elif (directive_name == "report"):
          ripl.report_value(int(arguments[1]))
        elif (directive_name == "clear"):
          ripl.clear()
        else:
          raise Exception("Unknown directive")

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
          
def process_sugars(venture_input):
  if type(venture_input) == list:
    if len(venture_input) == 0:
      return venture_input
    else:
      # "and" is implemented as in Clojure (FIXME: and is how in Scheme?)
      if venture_input[0] == "and":
        if len(venture_input) == 1:
          return True
        elif len(venture_input) == 2:
          return venture_input[1]
        else:
          current_element = venture_input[1]
          del venture_input[1]
          return process_sugars(["if", current_element, process_sugars(venture_input), False])
      # "or" is implemented almost as in Clojure (FIXME: and is how in Scheme?)
      elif venture_input[0] == "or":
        if len(venture_input) == 1:
          return False
        elif len(venture_input) == 2:
          return venture_input[1]
        else:
          current_element = venture_input[1]
          del venture_input[1]
          return process_sugars(["if", current_element, True, process_sugars(venture_input)])
      # (if predicate consequent alternative) is sugar for
      # ( (condition-ERP predicate (lambda () consequent) (lambda () alternative)) )
      elif venture_input[0] == "if":
        if len(venture_input) == 3:
          venture_input[3] = False
          
        if len(venture_input) == 4:
          predicate = venture_input[1]
          consequent = venture_input[2]
          alternative = venture_input[3]
          lambda_for_consequent = ["lambda", [], consequent]
          lambda_for_alternative = ["lambda", [], alternative]
          return [ ["condition-ERP", predicate, lambda_for_consequent, lambda_for_alternative] ]
        else:
          raise SyntaxError("'if' should have 3 or 4 arguments.")
      else:
        return venture_input
  else:
    return venture_input
