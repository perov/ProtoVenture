
from venture._engine import *

# import sys
# current_module = sys.modules[__name__]
# del current_module.sys

def load(generative_model_string):
  import venture.utils
  return venture.utils.load_to_RIPL(venture._engine, generative_model_string)

def sample(expression):
  import venture.utils
  return venture.utils.sample(venture._engine, expression)

def force(expression):
  import venture.utils
  return venture.utils.force(venture._engine, expression)
