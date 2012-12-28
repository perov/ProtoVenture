
import cloud
cloud.setkey(5529, api_secretkey='d492275c1761470b0add06d6ece2c89d406482d7')
cloud_environment = 'venture-2-6'
from venture_engine_requirements import *
import time

NumberOfSamples = 20
IntermediateMHIterations = 100

def sample_once(void_argument):
  import venture_engine
  MyRIPL = venture_engine
  # Tricky coin
  MyRIPL.clear() # To delete previous sessions data.
  MyRIPL.assume("is-tricky", ['bernoulli', 0.1])
  # You can either write the Church code via the Python lists:
  # MyRIPL.assume("head-probability", ['if', 'is-tricky', ['beta', 1.0, 1.0], 0.5])
  # Or to parse it from the string:
  MyRIPL.assume("head-probability", parse("(if is-tricky (beta 1.0 1.0) 0.5)"))
  for i in range(9):
    MyRIPL.observe(['bernoulli', 'head-probability'], True)
  (is_tricky, _) = MyRIPL.predict("is-tricky") # It could be done through the first ASSUME,
                                               # but we are using the PREDICT just to
                                               # check its operability.
  MyRIPL.infer(IntermediateMHIterations)
  return MyRIPL.report_value(is_tricky)
  
def run_test():
  HowManyTimesIsTricky = 0
  for sample_number in range(NumberOfSamples):
    sample = sample_once(0)
    if sample is True:
      HowManyTimesIsTricky = HowManyTimesIsTricky + 1
  print "** Time: " + str()
  return "Empirical prob.: " + str(float(HowManyTimesIsTricky) / NumberOfSamples) + " versus the theoretical =appr.= 0.850498339"

start_time = time.time()
print "Without mapping:"
jid = cloud.call(run_test, _env=cloud_environment, _type='c1')
result = cloud.result(jid)
print "** Time: " + str(time.time() - start_time)
print "** " + result

print ""
print ""
print ""

print "With mapping:"
start_time = time.time()
jids = cloud.map(sample_once, range(NumberOfSamples), _env=cloud_environment, _type='c1')
result = cloud.result(jids)
HowManyTimesIsTricky = 0
for sample in result:
  if sample is True:
    HowManyTimesIsTricky = HowManyTimesIsTricky + 1
print "** Time: " + str(time.time() - start_time)
print "** " + "Empirical prob.: " + str(float(HowManyTimesIsTricky) / NumberOfSamples) + " versus the theoretical =appr.= 0.850498339"
