rm ./venture
g++ Utilities.cpp VentureValues.cpp VentureParser.cpp Primitives.cpp Evaluator.cpp Main.cpp XRPCore.cpp XRPmem.cpp XRPs.cpp RIPL.cpp Analyzer.cpp ERPs.cpp MHProposal.cpp PythonProxy.cpp -o venture -I/usr/include/python2.6 -I/home/ec2-user/boost_1_52_0 -fpermissive -lpython2.6 -lgsl -lgslcblas -O2
