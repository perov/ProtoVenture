CXX = g++
CXXFLAGS = -c -O2 -I/usr/include/python2.7
OBJECTS = $(patsubst %.cpp,%.o,$(wildcard *.cpp))
DEPENDS = ${OBJECTS:.o=.d}
EXEC = venture

${EXEC} : ${OBJECTS}
	${CXX} ${OBJECTS} -o ${EXEC} -lpython2.7 -lgsl -lgslcblas -lpthread -lboost_thread-mt -lboost_system-mt

%.o:%.cpp
	${CXX} -MM -MF $*.d $*.cpp
	${CXX} ${CXXFLAGS} $*.cpp

clean:
	rm -f ${OBJECTS} ${DEPENDS} ${EXEC}

-include ${DEPENDS}

