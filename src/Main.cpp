
#include "Header.h"

#include "VentureValues.h"
#include "VentureParser.h"
#include "Analyzer.h"
#include "Evaluator.h"
#include "XRP.h"

// Using for Windows: http://sourceware.org/pthreads-win32/
// (So requires the pthreadVC2.dll)
#include <pthread.h>

shared_ptr<VentureList> const NIL_INSTANCE = shared_ptr<VentureList>(new VentureNil());
gsl_rng* random_generator = 0;
set< shared_ptr<NodeXRPApplication> > random_choices;
size_t DIRECTIVE_COUNTER = 0;

shared_ptr<NodeEnvironment> global_environment;
shared_ptr<NodeEvaluation> last_directive_node;
vector< shared_ptr<NodeEvaluation> > directives;

int continuous_inference_status = 0; // NOT THREAD SAFE!

// http://cs.gmu.edu/~white/CS571/Examples/Pthread/create.c
void* ContinuousInference(void* arguments) {
  while (continuous_inference_status != 0) {
    cout << " <I> ";
    for (size_t i = 0; i < 1000000; i++) {}
    if (continuous_inference_status == 1) {
      MakeMHProposal();
    } else if (continuous_inference_status == 2) {
      continuous_inference_status = 3;
      while (continuous_inference_status == 3) {
        //cout << "SleepingC" << endl;
        struct timespec ts = {0, 1000};
        pthread_delay_np(&ts);
      }
    }
  }
  pthread_exit(0);
  return NULL;
}

bool need_to_return_inference;
void PauseInference() {
  if (continuous_inference_status == 1) {
    need_to_return_inference = true;
    continuous_inference_status = 2;
    while (continuous_inference_status != 3) {
      struct timespec ts = {0, 1000};
      pthread_delay_np(&ts);
    }
  } else {
    need_to_return_inference = false;
  }
}

void ReturnInferenceIfNecessary() {
  if (need_to_return_inference == true) {
    continuous_inference_status = 1;
  }
}

void InitRIPL() {
  global_environment = shared_ptr<NodeEnvironment>(new NodeEnvironment(shared_ptr<NodeEnvironment>()));
  BindToEnvironment(global_environment,
                    shared_ptr<VentureSymbol>(new VentureSymbol("flip")), // Make just via the std::string?
                    shared_ptr<VentureXRP>(new VentureXRP(shared_ptr<XRP>(new ERP__Flip()))));
  // WARNING: Deprecated:
  BindToEnvironment(global_environment,
                    shared_ptr<VentureSymbol>(new VentureSymbol("noise-negate")), // Make just via the std::string?
                    shared_ptr<VentureXRP>(new VentureXRP(shared_ptr<XRP>(new ERP__NoisyNegate()))));
  BindToEnvironment(global_environment,
                    shared_ptr<VentureSymbol>(new VentureSymbol("noisy-negate")), // Make just via the std::string?
                    shared_ptr<VentureXRP>(new VentureXRP(shared_ptr<XRP>(new ERP__NoisyNegate()))));
  BindToEnvironment(global_environment,
                    shared_ptr<VentureSymbol>(new VentureSymbol("condition-ERP")), // Make just via the std::string?
                    shared_ptr<VentureXRP>(new VentureXRP(shared_ptr<XRP>(new ERP__ConditionERP()))));
  BindToEnvironment(global_environment,
                    shared_ptr<VentureSymbol>(new VentureSymbol("normal")), // Make just via the std::string?
                    shared_ptr<VentureXRP>(new VentureXRP(shared_ptr<XRP>(new ERP__Normal()))));
  BindToEnvironment(global_environment,
                    shared_ptr<VentureSymbol>(new VentureSymbol("uniform-discrete")), // Make just via the std::string?
                    shared_ptr<VentureXRP>(new VentureXRP(shared_ptr<XRP>(new ERP__UniformDiscrete()))));
  BindToEnvironment(global_environment,
                    shared_ptr<VentureSymbol>(new VentureSymbol("uniform-continuous")), // Make just via the std::string?
                    shared_ptr<VentureXRP>(new VentureXRP(shared_ptr<XRP>(new ERP__UniformContinuous()))));
  
  BindToEnvironment(global_environment,
                    shared_ptr<VentureSymbol>(new VentureSymbol("list")), // Make just via the std::string?
                    shared_ptr<VentureXRP>(new VentureXRP(shared_ptr<XRP>(new Primitive__List()))));
  BindToEnvironment(global_environment,
                    shared_ptr<VentureSymbol>(new VentureSymbol("real+")), // Make just via the std::string?
                    shared_ptr<VentureXRP>(new VentureXRP(shared_ptr<XRP>(new Primitive__RealPlus()))));
  BindToEnvironment(global_environment,
                    shared_ptr<VentureSymbol>(new VentureSymbol("real*")), // Make just via the std::string?
                    shared_ptr<VentureXRP>(new VentureXRP(shared_ptr<XRP>(new Primitive__RealMultiply()))));
  BindToEnvironment(global_environment,
                    shared_ptr<VentureSymbol>(new VentureSymbol("real-power")), // Make just via the std::string?
                    shared_ptr<VentureXRP>(new VentureXRP(shared_ptr<XRP>(new Primitive__RealPower()))));
  BindToEnvironment(global_environment,
                    shared_ptr<VentureSymbol>(new VentureSymbol("count>=")), // Make just via the std::string?
                    shared_ptr<VentureXRP>(new VentureXRP(shared_ptr<XRP>(new Primitive__CountEqualOrGreater()))));

  last_directive_node = shared_ptr<NodeEvaluation>();
}

void DeleteRIPL() {
  directives.clear();
  if (last_directive_node != shared_ptr<NodeEvaluation>()) {
    DeleteBranch(last_directive_node);
  }
  last_directive_node = shared_ptr<NodeEvaluation>();
  global_environment->DeleteNode();
  global_environment = shared_ptr<NodeEnvironment>();
}

void clear_trace()
{
  PauseInference();
  continuous_inference_status = 0;

  DeleteRIPL();
  InitRIPL();
}

size_t execute_directive(const string& directive_code) {
  cout << directive_code << endl;
  PauseInference();
  if (last_directive_node == shared_ptr<NodeEvaluation>()) {
    last_directive_node = AnalyzeDirective(ReadCode(directive_code));
    Evaluator(last_directive_node, global_environment, shared_ptr<Node>(), shared_ptr<NodeEvaluation>());
  } else {
    shared_ptr<NodeEvaluation> new_directive_node = AnalyzeDirective(ReadCode(directive_code));
    Evaluator(new_directive_node, global_environment, shared_ptr<Node>(), shared_ptr<NodeEvaluation>());
    new_directive_node->earlier_evaluation_nodes = last_directive_node;
    last_directive_node = new_directive_node;
  }
  directives.push_back(last_directive_node);
  ReturnInferenceIfNecessary();
  return directives.size();
}

string report_value(size_t directive_id) {
  cout << "Pam1";
  PauseInference();
  cout << "Pam2";
  if (directives[directive_id - 1]->GetNodeType() == DIRECTIVE_ASSUME) {
    ReturnInferenceIfNecessary(); // Too early.
    cout << "Pam3";
    return dynamic_pointer_cast<NodeDirectiveAssume>(directives[directive_id - 1])->my_value->GetString();
  } else if (directives[directive_id - 1]->GetNodeType() == DIRECTIVE_PREDICT) {
    cout << "Pam4" << endl;
    cout << dynamic_pointer_cast<NodeDirectivePredict>(directives[directive_id - 1])->my_value->GetType() << "!" << endl;
    cout << dynamic_pointer_cast<NodeDirectivePredict>(directives[directive_id - 1])->my_value->GetString() << "!" << endl;
    string to_return = dynamic_pointer_cast<NodeDirectivePredict>(directives[directive_id - 1])->my_value->GetString();
    cout << "Pam4.5";
    ReturnInferenceIfNecessary();
    cout << "Pam4.8";
    return to_return;
  } else {
    ReturnInferenceIfNecessary();
    cout << "Pam5";
    throw std::exception("Attempt to report value neither for ASSUME nor PREDICT.");
  }
}

void forget_directive(size_t directive_id) {
  if (directive_id >= directives.size() ||
        directives[directive_id - 1] == shared_ptr<NodeEvaluation>()) {
    throw std::exception("There is no such directive.");
  }

  if (last_directive_node == directives[directive_id - 1] &&
        last_directive_node->earlier_evaluation_nodes == shared_ptr<NodeEvaluation>()) {
    PauseInference(); 
    if (last_directive_node == directives[directive_id - 1]) {
      last_directive_node = directives[directive_id - 1]->earlier_evaluation_nodes;
    }
    DeleteBranch(directives[directive_id - 1]);
    directives[directive_id - 1] = shared_ptr<NodeEvaluation>();
    ReturnInferenceIfNecessary();
  } else {
    clear_trace();
  }
}

static PyObject*
ForPython__execute_directive(PyObject *self, PyObject *args)
{
  char* directive_code_as_chars;
  if(!PyArg_ParseTuple(args, "s:execute_directive", &directive_code_as_chars))
    return NULL;
  return Py_BuildValue("i", execute_directive(directive_code_as_chars));
}
static PyObject*
ForPython__report_value(PyObject *self, PyObject *args)
{
  int directive_id;
  if(!PyArg_ParseTuple(args, "i:report_value", &directive_id))
    return NULL;
  string result = report_value(directive_id);
  if (IsInteger(result)) {
    return Py_BuildValue("i", boost::lexical_cast<int>(result));
  } else if (IsReal(result)) {
    return Py_BuildValue("d", boost::lexical_cast<double>(result));
  } else {
    return Py_BuildValue("s", result.c_str());
  }
}
static PyObject*
ForPython__clear(PyObject *self, PyObject *args)
{
  int directive_id;
  if(!PyArg_ParseTuple(args, ":clear"))
    return NULL;
  clear_trace();
  return Py_BuildValue("i", 0); // Make something better.
}
static PyObject*
ForPython__forget(PyObject *self, PyObject *args)
{
  int directive_id;
  if(!PyArg_ParseTuple(args, "i:forget", &directive_id))
    return NULL;
  forget_directive(directive_id);
  return Py_BuildValue("i", 0); // Make something better.
}
static PyObject*
ForPython__start_continuous_inference(PyObject *self, PyObject *args)
{
  int directive_id;
  if(!PyArg_ParseTuple(args, ":start_continuous_inference"))
    return NULL;
  pthread_t new_thread;
  if (continuous_inference_status == 0) {
    continuous_inference_status = 1;
    cout << "Starting thread" << endl;
    pthread_create(&new_thread, NULL, &ContinuousInference, NULL);
    cout << "Have started" << endl;
    return Py_BuildValue("i", 0); // Make something better.
  } else {
    throw std::exception("The continuous inference has been already started.");
  }
}
static PyObject*
ForPython__continuous_inference_status(PyObject *self, PyObject *args)
{
  int directive_id;
  if(!PyArg_ParseTuple(args, ":continuous_inference_status"))
    return NULL;
  if (continuous_inference_status == 0) {
    return Py_BuildValue("b", false);
  } else {
    return Py_BuildValue("b", true);
  }
}
static PyObject*
ForPython__NewVentureValue(PyObject *self, PyObject *args)
{
  char* venture_type;
  char* value_as_string;
  if(!PyArg_ParseTuple(args, "ss:NewVentureValue", &venture_type, &value_as_string))
    return NULL;
  if (venture_type == "count") {
    //shared_ptr<VentureValue> new_value = shared_ptr<VentureCount>(new VentureCount(value_as_string));
    return Py_BuildValue("b", true);
    //return PyCapsule_New((void*) &new_value, NULL, NULL);
  } else {
    throw std::exception("Undefined VentureType.");
  }
}
static PyMethodDef MethodsForPythons[] = {
    {"execute_directive", ForPython__execute_directive, METH_VARARGS,
     "... Write description ..."},
    {"report_value", ForPython__report_value, METH_VARARGS,
     "... Write description ..."},
    {"clear", ForPython__clear, METH_VARARGS,
     "... Write description ..."},
    {"forget", ForPython__forget, METH_VARARGS,
     "... Write description ..."},
    {"start_continuous_inference", ForPython__start_continuous_inference, METH_VARARGS,
     "... Write description ..."},
    {"continuous_inference_status", ForPython__continuous_inference_status, METH_VARARGS,
     "... Write description ..."},
    {"NewVentureValue", ForPython__NewVentureValue, METH_VARARGS,
     "... Write description ..."},
    {NULL, NULL, 0, NULL}
};

int main(int argc, char *argv[])
{
  random_generator = gsl_rng_alloc(gsl_rng_mt19937);
  gsl_rng_set(random_generator, abs(time(NULL)));

  InitRIPL();

  /*
  execute_directive("(ASSUME order (uniform-discrete c[0] c[4]))");
  execute_directive("(ASSUME noise (uniform-continuous r[0.1] r[1.0]))");
  execute_directive("(ASSUME c0 (if (count>= order c[0]) (normal r[0.0] r[10.0]) r[0.0]))");
  execute_directive("(ASSUME c1 (if (count>= order c[1]) (normal r[0.0] r[1]) r[0.0]))");
  execute_directive("(ASSUME c2 (if (count>= order c[2]) (normal r[0.0] r[0.1]) r[0.0]))");
  execute_directive("(ASSUME c3 (if (count>= order c[3]) (normal r[0.0] r[0.01]) r[0.0]))");
  execute_directive("(ASSUME c4 (if (count>= order c[4]) (normal r[0.0] r[0.001]) r[0.0]))");
  execute_directive("(ASSUME clean-func (lambda (x) (real+ c0 (real* c1 (real-power x r[1.0])) (real* c2 (real-power x r[2.0])) (real* c3 (real-power x r[3.0])) (real* c4 (real-power x r[4.0])))))");
  execute_directive("(PREDICT (list order c0 c1 c2 c3 c4 noise))");
  execute_directive("(OBSERVE (normal (clean-func (normal r[5.6] noise)) noise) r[1.8])");
  
  for (size_t attempt = 0; attempt < 1000; attempt++) {
    MakeMHProposal();
    cout << endl << report_value(9) << endl;
  }
  */

  cout << "See why: Or just NULL? does not work!" << endl;
  cout << "USING PYTHON25.LIB INSTEAD OF PYTHON25_D.LIB. NOT GOOD AT ALL!" << endl;

  cout << argv[0] << endl;

  Py_SetProgramName(argv[0]);  /* optional but recommended */
  Py_Initialize();
  Py_InitModule("venture_engine", MethodsForPythons);
  FILE *python_script = fopen("C:/Users/Yura Perov/workspace/VentureAlpha/src/RESTPython.py", "r");
  /*cout << python_script << endl;
  long len;
  char* buf;
  fseek(python_script, 0, SEEK_END); //go to end
  len = ftell(python_script); //get position at end (length)
  fseek(python_script, 0, SEEK_SET); //go to beg.
  buf = (char*) malloc(len); //malloc buffer
  fread(buf, len, 1, python_script); //read into buffer
  cout << buf << endl;*/
  PyRun_SimpleFile(python_script, "RESTPython.py");
  //PyRun_SimpleString(buf);
  // Why it does not work?: PyRun_SimpleFile(python_script, "RESTPython.py");
  fclose(python_script);
  //numargs = argc;
  //PyRun_SimpleString("import emb\n"
  //                   "print \"Number of arguments\", emb.numargs()");
  
  /*
  try {
    
    //execute_directive("(ASSUME cloudy (flip r[0.5]))");
    //execute_directive("(ASSUME sprinkler (flip (if cloudy r[0.1] r[0.5])))");
    //execute_directive("(ASSUME rain (flip (if cloudy r[0.8] r[0.2])))");
    //execute_directive("(ASSUME wet (flip (if sprinkler (if rain r[0.99] r[0.9]) (if rain r[0.9] r[0.01]))))");

    execute_directive("(ASSUME breast-cancer (flip r[0.01]))");
    execute_directive("(ASSUME positive-mammogram (lambda () (if breast-cancer (flip r[0.8]) (flip r[0.096]))))");
    execute_directive("(OBSERVE (noise-negate (positive-mammogram) r[0.00001]) b[true])");

    for (size_t attempt = 0; attempt < 1000; attempt++) {
      clock_t t = clock();

      double count = 0.0;
      double trues = 0.0;
      for (size_t index = 0; index < 100000; index++) {
        MakeMHProposal();
        //cout << global_environment->variables["wet"]->value->GetString() << endl;
        count++;
        //if (CompareValue(global_environment->variables["wet"]->value, shared_ptr<VentureBoolean>(new VentureBoolean(true)))) {
        //  trues++;
        //}
      }
      cout << (trues / count) << endl;

      t = clock() - t;
      cout << "Clicks: " << t << "; seconds: " << ((float)t)/CLOCKS_PER_SEC << endl;
    }
    
    //DrawGraph(last_directive_node);

    DeleteRIPL();
  } catch(std::exception& e) {
    cout << "Error: " << e.what() << endl;
  } catch(...) {
    cout << "Strange exception." << endl;
  }
  */

  // Delete the trace?

  Py_Finalize();
  return 0;
}
