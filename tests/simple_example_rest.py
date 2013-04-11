
import venture.client

Port = 8082
MyRIPL = venture.client.RemoteRIPL("http://127.0.0.1:" + str(Port))

# Tricky coin
generative_model = """
[CLEAR]
[ASSUME is-tricky (bernoulli 0.1)]
[ASSUME head-probability (if is-tricky (beta 1.0 1.0) 0.5)]
"""
MyRIPL.load(generative_model)

print(venture.client.directives_to_string(MyRIPL.report_directives()))                                            
                                             
for i in range(9):
    MyRIPL.observe("(bernoulli head-probability)", 'true')

(is_tricky, _) = MyRIPL.predict("is-tricky") # It could be done through the first ASSUME,
                                             # but we are using the PREDICT just to
                                             # check its operability.
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
