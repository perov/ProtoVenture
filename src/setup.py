# From here:
# http://docs.python.org/2/extending/building.html#building

# Just build as a Python library: python setup.py build
# Build and install to the system: sudo python setup.py build install

import sys

from distutils.core import setup, Extension

venture_libraries = ['gsl', 'gslcblas', 'pthread', 'boost_system-mt', 'boost_thread-mt'] # 'profiler'
venture_extra_compile_args = ['-O2']

# venture_libraries += ['profiler']
# venture_extra_compile_args += ['-D_VENTURE_USE_GOOGLE_PROFILER']

module1 = Extension('venture_engine',
                    define_macros = [('MAJOR_VERSION', '1'),
                                     ('MINOR_VERSION', '0')],
                    include_dirs = ['/usr/include/python' + str(sys.version_info[0]) + '.' + str(sys.version_info[1])],
                    libraries = venture_libraries,
                    extra_compile_args = venture_extra_compile_args,
                    #library_dirs = ['/usr/local/lib'],
                    sources = ['Utilities.cpp', 'VentureValues.cpp', 'VentureParser.cpp', 'Primitives.cpp', 'Evaluator.cpp', 'Main.cpp', 'XRPCore.cpp', 'XRPmem.cpp', 'XRPs.cpp', 'RIPL.cpp', 'Analyzer.cpp', 'ERPs.cpp', 'MHProposal.cpp', 'PythonProxy.cpp'])
#-lpython2.6 -lgsl -lgslcblas

setup (name = 'Venture engine',
       version = '1.0',
       description = 'Testing',
       author = 'MIT Probabilistic Computing Project',
       author_email = 'venture-dev@lists.csail.mit.edu',
       url = 'TBA',
       long_description = '''
TBA.
''',
       ext_modules = [module1])
