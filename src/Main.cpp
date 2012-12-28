
#include "Header.h"

#include "VentureValues.h"
#include "VentureParser.h"
#include "Analyzer.h"
#include "Evaluator.h"
#include "XRPCore.h"
#include "XRPs.h"
#include "RIPL.h"
#include "ERPs.h"
#include "Primitives.h"
#include "PythonProxy.h"

/*
   NOTICES:
   1) Booleans should be as NIL!
   2) When some C++ function throws exception,
      we should firstly delete Python objects.
*/

shared_ptr<VentureList> const NIL_INSTANCE = shared_ptr<VentureList>(new VentureNil());
gsl_rng* random_generator = 0;
set< shared_ptr<NodeXRPApplication> > random_choices;
size_t DIRECTIVE_COUNTER = 0;

shared_ptr<NodeEnvironment> global_environment;
size_t last_directive_id;
map<size_t, directive_entry> directives;

int continuous_inference_status = 0; // NOT THREAD SAFE!

int next_gensym_atom = 0; // Should become better for the multithread version.
                          // Also check for "limits::max"

bool need_to_return_inference;

// It is not thread-safe.
// It should be implemented as parameter (or member of parameter) for all Evaluators!
size_t TMP_number_of_created_XRPSamplers;

void InitGSL() {
  random_generator = gsl_rng_alloc(gsl_rng_mt19937);
  unsigned long seed = time(NULL); // time(NULL)
  //cout << "WARNING: RANDOM SEED is not random!" << endl;
  gsl_rng_set(random_generator, seed);
}
  
PyMODINIT_FUNC initventure_engine(void) {
  InitGSL();
  InitRIPL();
  PyRun_SimpleString("execfile(\"/home/picloud/venture/__PythonModule.py\")"); // FIXME: absolute path.
  Py_InitModule("venture_engine", MethodsForPythons);
}

int main(int argc, char *argv[])
{
#ifdef _MSC_VER
  _CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

  InitGSL();
  InitRIPL();
  
  cout << "See why: Or just NULL? does not work!" << endl;
  cout << "Notice: There should not be the 'NIL' type! Only the 'LIST' type!" << endl;
  cout << "Notice: get rid of TMP_number_of_created_XRPSamplers!" << endl;
  cout << "IMPORTANT! Delete touched_nodes_copy!" << endl;
  
  int port = 8081;
  cout << argv[0] << endl;
  if (argc > 1) {
    port = boost::lexical_cast<int>(argv[1]);
  }

  Py_SetProgramName(argv[0]); // Optional but recommended.
  Py_Initialize();
  Py_InitModule("venture_engine", MethodsForPythons);

  // PyRun_SimpleFile(...) does not work in Release configuration (works in debug).
  // Read here: http://docs.python.org/2/faq/windows.html#pyrun-simplefile-crashes-on-windows-but-not-on-unix-why
  // It seems it is necessary to recompile pythonXY.lib and *.dll.
  // Now using this variant:
#ifdef _MSC_VER
  PyRun_SimpleString("execfile(\"C:/Users/Yura Perov/workspace/VentureAlpha/src/RESTPython.py\")");
#else
  PyRun_SimpleString("execfile(\"/usr/venture/RESTPython.py\")");
#endif

  PyRun_SimpleString(("app.run(port=" + boost::lexical_cast<string>(port) + ", host='0.0.0.0')").c_str());

  Py_Finalize();
  return 0;
}
