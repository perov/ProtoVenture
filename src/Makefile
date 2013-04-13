
CXX = g++

python_version_full := $(wordlist 2,4,$(subst ., ,$(shell python --version 2>&1)))
python_version_major := $(word 1,${python_version_full})
python_version_minor := $(word 2,${python_version_full})
python_version_patch := $(word 3,${python_version_full})
python_version_include := ${python_version_major}.${python_version_minor}

CXXFLAGS = -c -O2 -I/usr/include/python${python_version_include}
CPP_SOURCE_FILES = $(wildcard *.cpp)

# "withzmq=true make" adds external XRP support.
ifneq ($(withzmq),)
  ADDITION_LIBRARIES = -lzmq
  CXX := $(CXX) -DVENTURE__FLAG__COMPILE_WITH_ZMQ
else
  ADDITION_LIBRARIES = 
  CPP_SOURCE_FILES := $(filter-out %ExternalXRPInterface.cpp, $(CPP_SOURCE_FILES))
endif

# "enablegdb=true make" adds -g flag for "g++" (enables GDB).
ifneq ($(enablegdb),)
  GDB_DEBUG = -g
else
  GDB_DEBUG = 
endif

OBJECTS = $(patsubst %.cpp,%.o,$(CPP_SOURCE_FILES))

DEPENDS = ${OBJECTS:.o=.d}
EXEC = venture_binary

${EXEC} : ${OBJECTS}
	${CXX} ${OBJECTS} ${GDB_DEBUG} -o ${EXEC} -lpython${python_version_include} ${ADDITION_LIBRARIES} -lgsl -lgslcblas -lpthread -lboost_thread-mt -lboost_system-mt

%.o:%.cpp
	${CXX} -MM -MF $*.d $*.cpp
	${CXX} ${CXXFLAGS} $*.cpp

clean:
	rm -f ${OBJECTS} ${DEPENDS} ${EXEC}

-include ${DEPENDS}
