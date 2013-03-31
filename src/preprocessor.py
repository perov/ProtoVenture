def preprocess(venture_input):
  if type(venture_input) != list:
    return LiteralPreprocessor(venture_input)
  elif len(venture_input) == 0:
    return ListPreprocessor(venture_input)
  
  procedure = venture_input[0]
  if procedure in sugars:
    return sugars[procedure](venture_input)
  else:
    return ListPreprocessor(venture_input)

class LiteralPreprocessor:
  def __init__(self, venture_input):
    self.value = venture_input
  
  def desugar(self):
    return self.value
  
  def resugar(self, location):
    if len(location) == 0:
      return location
    raise ResugarError("Invalid location")

class ListPreprocessor:
  def __init__(self, venture_input):
    self.args = map(preprocess, venture_input)
  
  def desugar(self):
    #return [preprocessor.desugar() for preprocessor in self.args]
    return map(lambda preprocessor: preprocessor.desugar(), self.args)
  
  def resugar(self, location):
    if len(location) == 0:
      return location
    return [location[0]] + self.args[location[0]-1].resugar(location[1:])


# resugar a dummy lambda expression
def resugarDummyLambda(location, exprPreprocessor):
  if len(location) == 0:
    return location
  
  index = location[0]
  if index == 0: # application of lambda -> body of expression
    return []
  elif index == 1: # 'lambda' -> body of expression
    return []
  elif index == 2: # '()' -> body of expression
    return []
  elif index == 3: # actual expression
    return exprPreprocessor.resugar(location[1:])
  raise ResugarError("Invalid location.")

class IfPreprocessor(ListPreprocessor):
  def __init__(self, venture_input):
    if len(venture_input) != 4:
      raise SyntaxError("'if' should have 3 arguments.")
    
    self.predicate = preprocess(venture_input[1])
    self.consequent = preprocess(venture_input[2])
    self.alternative = preprocess(venture_input[3])
  
  def desugar(self):
    lambda_for_consequent = ["lambda", [], self.consequent.desugar()]
    lambda_for_alternative = ["lambda", [], self.alternative.desugar()]
    
    return [["condition-ERP", self.predicate.desugar(), lambda_for_consequent, lambda_for_alternative]]
  
  def resugar(self, location):
    if len(location) == 0:
      return location
    elif len(location) == 1:
      if location[0] == 0: # outer application -> application of if
        return location
      elif location[0] == 1: # body of (condition-ERP ...) -> body of if
        return []
      raise ResugarError("Invalid location.")
    
    index = location[1]
    if index == 0: # application of (condition-ERP ...) -> application of if
      return location[1:]
    elif index == 1: # 'condition-ERP' -> 'if'
      return location[1:]
    elif index == 2: # predicate
      return [2] + self.predicate.desugar(location[2:])
    elif index == 3: # consequent
      return [3] + resugarDummyLambda(location[2:], self.consequent)
    elif index == 4: # alternative
      return [4] + resugarDummyLambda(location[2:], self.alternative)

class AndPreprocessor(IfPreprocessor):
  def __init__(self, venture_input):
    if len(venture_input) != 3:
      raise SyntaxError("'and' should have 2 arguments.")
    
    self.predicate = preprocess(venture_input[1])
    self.consequent = preprocess(venture_input[2])
    self.alternative = LiteralPreprocessor("b<false>")
  
  def resugar(self, location):
    location = IfPreprocessor.resugar(self, location)
    if len(location) == 0:
      return location
    
    if location[0] == 4: # 'false' -> 'and'
      location[0] = 1
    
    return location

class OrPreprocessor(IfPreprocessor):
  def __init__(self, venture_input):
    if len(venture_input) != 3:
      raise SyntaxError("'or' should have 2 arguments.")
    
    self.predicate = preprocess(venture_input[1])
    self.consequent = LiteralPreprocessor("b<true>")
    self.alternative = preprocess(venture_input[2])
  
  def resugar(self, location):
    location = IfPreprocessor.resugar(self, location)
    if len(location) == 0:
      return location
    
    if location[0] == 3: # 'true' -> 'or'
      location[0] = 1
    
    return location

class ResugarError(Exception):
  def __init__(self, msg):
    self.msg = msg

# dictionary of sugars
sugars = {'if' : IfPreprocessor, 'and' : AndPreprocessor, 'or' : OrPreprocessor}

if __name__ == '__main__':
  from lisp_parser import parse
  #venture_input = parse("(if a b c)")
  venture_input = ['if', 'a', ['and', 'b', 'c'], ['or', 'd', 'e']]
  preprocessor = preprocess(venture_input)
  print preprocessor.desugar()
  location = [1, 3, 3, 1, 3, 2]
  print preprocessor.resugar(location)
  

