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
  if index == 0: # application of lambda -> body of expr
    return []
  elif index == 1: # 'lambda'
    return []
  elif index == 2: # '()'
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

class AndPreprocessor:
  def __init__(self, venture_input):
    if len(venture_input) != 3:
      raise SyntaxError("'and' should have 2 arguments.")
    
    self.expr1 = preprocess(venture_input[1])
    self.expr2 = preprocess(venture_input[2])
  
  def desugar(self):
    lambda_for_expr2 = ["lambda", [], self.expr2.desugar()]
    lambda_for_false = ["lambda", [], "b<false>"]
    
    return [["condition-ERP", self.expr1.desugar(), lambda_for_expr2, lambda_for_false]]
  
  def resugar(self, location):
    if len(location) == 0:
      return location
    
    if len(location) == 1:
      if location[0] == 0: # outer application -> application of 'and'
        return [0]
      elif location[0] == 1: # body of (condition-ERP ...) -> body of 'and'
        return []
      raise ResugarError("Invalid location.")
    
    index = location[1]
    if index == 0: # application of (condition-ERP ...) -> application of 'and'
      return [0]
    elif index == 1: # 'condition-ERP' -> 'and'
      return [1]
    elif index == 2: # expr1
      return [2] + self.expr1.desugar(location[2:])
    elif index == 3: # expr2
      return [3] + resugarDummyLambda(location[2:], self.expr2)
    elif index == 4: # false -> 'and'
      return [1]

class OrPreprocessor:
  def __init__(self, venture_input):
    if len(venture_input) != 3:
      raise SyntaxError("'or' should have 2 arguments.")
    
    self.expr1 = preprocess(venture_input[1])
    self.expr2 = preprocess(venture_input[2])
  
  def desugar(self):
    lambda_for_expr2 = ["lambda", [], self.expr2.desugar()]
    lambda_for_true = ["lambda", [], "b<true>"]
    
    return [["condition-ERP", self.expr1.desugar(), lambda_for_true, lambda_for_expr2]]
  
  def resugar(self, location):
    if len(location) == 0:
      return location
    
    if len(location) == 1:
      if location[0] == 0: # outer application -> application of 'or'
        return [0]
      elif location[0] == 1: # body of (condition-ERP ...) -> body of 'or'
        return []
      raise ResugarError("Invalid location.")
    
    index = location[1]
    if index == 0: # application of (condition-ERP ...) -> application of 'or'
      return [0]
    elif index == 1: # 'condition-ERP' -> 'or'
      return [1]
    elif index == 2: # expr1
      return [2] + self.expr1.desugar(location[2:])
    elif index == 3: # true -> 'or'
      return [1]
    elif index == 4: # expr2
      return [3] + resugarDummyLambda(location[2:], self.expr2)

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
  

