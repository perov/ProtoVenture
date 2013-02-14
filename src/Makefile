
# Rewrite with automatic prerequesites generating:
# --- http://www.gnu.org/software/make/manual/make.html#Automatic-Prerequisites
# --- http://stackoverflow.com/questions/313778/generate-dependencies-for-a-makefile-for-a-project-in-c-c
# --- http://mad-scientist.net/make/autodep.html#noreexec

# Now "make" thinks sometimes that the "project" is up to date, although it is not up to date really.

# Delete?: -I/home/ec2-user/boost_1_52_0 -I/root/boost_1_52_0
# Delete?: -O2

CPP = g++
FLAGS = -I/usr/include/python2.6 -I/home/ec2-user/boost_1_52_0 -I/root/boost_1_52_0 -O2 -lpython2.6 -lgsl -lgslcblas -lpthread -lboost_system -lboost_thread
FILES_FOR_INCLUDE = Utilities.cpp VentureValues.cpp VentureParser.cpp Primitives.cpp Evaluator.cpp Main.cpp XRPCore.cpp XRPmem.cpp XRPs.cpp RIPL.cpp Analyzer.cpp ERPs.cpp MHProposal.cpp PythonProxy.cpp

venture:
	rm -f ./venture
	$(CPP) $(FILES_FOR_INCLUDE) $(FLAGS) -o venture