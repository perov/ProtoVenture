CXX = g++

python_version_full := $(wordlist 2,4,$(subst ., ,$(shell python --version 2>&1)))
python_version_major := $(word 1,${python_version_full})
python_version_minor := $(word 2,${python_version_full})
python_version_patch := $(word 3,${python_version_full})
python_version_include := ${python_version_major}.${python_version_minor}

CXXFLAGS = -c -O2 -I/usr/include/python${python_version_include}
OBJECTS = $(patsubst %.cpp,%.o,$(wildcard *.cpp))
DEPENDS = ${OBJECTS:.o=.d}
EXEC = venture

${EXEC} : ${OBJECTS}
	${CXX} ${OBJECTS} -o ${EXEC} -lpython${python_version_include} -lgsl -lgslcblas -lpthread -lboost_thread-mt -lboost_system-mt

%.o:%.cpp
	${CXX} -MM -MF $*.d $*.cpp
	${CXX} ${CXXFLAGS} $*.cpp

clean:
	rm -f ${OBJECTS} ${DEPENDS} ${EXEC}

-include ${DEPENDS}

