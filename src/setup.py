
# From here:
# http://docs.python.org/2/extending/building.html#building

from distutils.core import setup, Extension

module1 = Extension('venture_engine',
                    define_macros = [('MAJOR_VERSION', '1'),
                                     ('MINOR_VERSION', '0')],
                    include_dirs = ['/usr/include/python2.6', '/home/ec2-user/boost_1_52_0'],
                    libraries = ['gsl', 'gslcblas'],
                    extra_compile_args = ['-O2', '-fpermissive'],
                    #library_dirs = ['/usr/local/lib'],
                    sources = ['Utilities.cpp', 'VentureValues.cpp', 'VentureParser.cpp', 'Primitives.cpp', 'Evaluator.cpp', 'Main.cpp', 'XRP.cpp', 'RIPL.cpp', 'Analyzer.cpp', 'ERPs.cpp', 'MHProposal.cpp', 'PythonProxy.cpp'])
#-lpython2.6 -lgsl -lgslcblas

setup (name = 'Venture engine',
       version = '1.0',
       description = 'Testing',
       author = 'MIT.PCP',
       author_email = 'yura.perov@gmail.com',
       url = 'TBA',
       long_description = '''
TBA.
''',
       ext_modules = [module1])
