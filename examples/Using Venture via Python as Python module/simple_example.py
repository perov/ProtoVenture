from venture_engine_requirements import *
import venture_engine

MyRIPL = venture_engine

# Tricky coin

generative_model = """
[CLEAR]
[ASSUME is-tricky (bernoulli 0.1)]
[ASSUME head-probability (if is-tricky (beta 1.0 1.0) 0.5)]
"""
loadToRIPL(MyRIPL, generative_model)

(is_tricky, _) = MyRIPL.predict("is-tricky") # It could be done through the first ASSUME,
                                             # but we are using the PREDICT just to
                                             # check its operability.
                                
for i in range(9):
    MyRIPL.observe(['bernoulli', 'head-probability'], 'true')

print MyRIPL.report_value(is_tricky)

HowManyTimesIsTricky = 0
NumberOfSamples = 100
IntermediateMHIterations = 100

for sample_number in range(NumberOfSamples):
  MyRIPL.infer(IntermediateMHIterations)
  sample = MyRIPL.report_value(is_tricky)
  print "Sample #" + str(sample_number) + " = " + str(sample)
  if sample is True:
    HowManyTimesIsTricky = HowManyTimesIsTricky + 1

print "Empirical prob.: " + str(float(HowManyTimesIsTricky) / NumberOfSamples) + " versus the theoretical =appr.= 0.850498339"
