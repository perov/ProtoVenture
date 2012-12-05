
#include "Header.h"

#include "VentureValues.h"
#include "VentureParser.h"
#include "Analyzer.h"
#include "Evaluator.h"
#include "XRP.h"
#include "RIPL.h"
#include "ERPs.h"
#include "Primitives.h"

// Booleans should be as NIL!

// Using for Windows: http://sourceware.org/pthreads-win32/
// (So requires the pthreadVC2.dll)
#include <pthread.h>

shared_ptr<VentureList> const NIL_INSTANCE = shared_ptr<VentureList>(new VentureNil());
gsl_rng* random_generator = 0;
set< shared_ptr<NodeXRPApplication> > random_choices;
size_t DIRECTIVE_COUNTER = 0;

shared_ptr<NodeEnvironment> global_environment;
size_t last_directive_id;
map<size_t, directive_entry> directives;

int continuous_inference_status = 0; // NOT THREAD SAFE!

/* Python notices:
   1) When some C++ function throws exception,
      we should firstly delete Python objects.
*/

// http://cs.gmu.edu/~white/CS571/Examples/Pthread/create.c
void* ContinuousInference(void* arguments) {
  while (continuous_inference_status != 0) {
    //cout << " <I> ";
    if (continuous_inference_status == 1) {
      try {
        MakeMHProposal();
      } catch (std::exception& e) {
        cout << "Exception has been raised during continuous inference: " << e.what() << endl;
        return NULL;
      }
    } else if (continuous_inference_status == 2) {
      continuous_inference_status = 3;
      while (continuous_inference_status == 3) {
        //cout << "SleepingC" << endl;
        struct timespec ts = {0, 1}; // 1 ns -- is it okay? :)
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
    clock_t start_time = clock();
    while (continuous_inference_status != 3) {
      if ((( static_cast<float>(clock() - start_time)) / CLOCKS_PER_SEC) > 3.0) {
        throw std::exception("Cannot stop the inference for more than 3 seconds.");
      }
      struct timespec ts = {0, 1}; // 1 ns -- is it okay? :)
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
  BindToEnvironment(global_environment,
                    shared_ptr<VentureSymbol>(new VentureSymbol("bernoulli")), // Make just via the std::string?
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
                    shared_ptr<VentureSymbol>(new VentureSymbol("beta")), // Make just via the std::string?
                    shared_ptr<VentureXRP>(new VentureXRP(shared_ptr<XRP>(new ERP__Beta()))));
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
                    shared_ptr<VentureSymbol>(new VentureSymbol("+")), // Make just via the std::string?
                    shared_ptr<VentureXRP>(new VentureXRP(shared_ptr<XRP>(new Primitive__RealPlus()))));
  BindToEnvironment(global_environment,
                    shared_ptr<VentureSymbol>(new VentureSymbol("*")), // Make just via the std::string?
                    shared_ptr<VentureXRP>(new VentureXRP(shared_ptr<XRP>(new Primitive__RealMultiply()))));
  BindToEnvironment(global_environment,
                    shared_ptr<VentureSymbol>(new VentureSymbol("power")), // Make just via the std::string?
                    shared_ptr<VentureXRP>(new VentureXRP(shared_ptr<XRP>(new Primitive__RealPower()))));
  BindToEnvironment(global_environment,
                    shared_ptr<VentureSymbol>(new VentureSymbol(">=")), // Make just via the std::string?
                    shared_ptr<VentureXRP>(new VentureXRP(shared_ptr<XRP>(new Primitive__RealEqualOrGreater()))));
  BindToEnvironment(global_environment,
                    shared_ptr<VentureSymbol>(new VentureSymbol("<=")), // Make just via the std::string?
                    shared_ptr<VentureXRP>(new VentureXRP(shared_ptr<XRP>(new Primitive__RealEqualOrLesser()))));
  BindToEnvironment(global_environment,
                    shared_ptr<VentureSymbol>(new VentureSymbol(">")), // Make just via the std::string?
                    shared_ptr<VentureXRP>(new VentureXRP(shared_ptr<XRP>(new Primitive__RealEqualOrGreater()))));
  BindToEnvironment(global_environment,
                    shared_ptr<VentureSymbol>(new VentureSymbol("<")), // Make just via the std::string?
                    shared_ptr<VentureXRP>(new VentureXRP(shared_ptr<XRP>(new Primitive__RealLesser()))));
  BindToEnvironment(global_environment,
                    shared_ptr<VentureSymbol>(new VentureSymbol("=")), // Make just via the std::string?
                    shared_ptr<VentureXRP>(new VentureXRP(shared_ptr<XRP>(new Primitive__RealEqual()))));

  last_directive_id = 0;
  DIRECTIVE_COUNTER = 0; // For "myorder".
}

void DeleteRIPL() {
  // Reconsider this function.
  for (map<size_t, directive_entry>::iterator iterator = directives.begin(); iterator != directives.end(); iterator++) {
    DeleteBranch(iterator->second.directive_node);
  }

  if (global_environment != shared_ptr<NodeEnvironment>()) { // It should not be in a good way?
    global_environment->DeleteNode();
  }
  global_environment = shared_ptr<NodeEnvironment>();

  last_directive_id = 0;
  directives.clear();
}

void clear_trace()
{
  continuous_inference_status = 0;

  DeleteRIPL();
  InitRIPL();
}

shared_ptr<VentureValue> report_value(size_t directive_id) {
  if (directives.count(directive_id) == 0) {
    throw std::exception("Attempt to report value neither for non-existent directive.");
  }
  if (directives[directive_id].directive_node->GetNodeType() == DIRECTIVE_ASSUME) {
    return dynamic_pointer_cast<NodeDirectiveAssume>(directives[directive_id].directive_node)->my_value;
  } else if (directives[directive_id].directive_node->GetNodeType() == DIRECTIVE_PREDICT) {
    return dynamic_pointer_cast<NodeDirectivePredict>(directives[directive_id].directive_node)->my_value;
  } else {
    throw std::exception("Attempt to report value neither for ASSUME nor PREDICT.");
  }
}

void forget_directive(size_t directive_id) {
  if (directives.count(directive_id) == 0) {
    throw std::exception("There is no such directive.");
  }

  if (directives.size() == 1)
  {
    clear_trace();
  } else {
    // Check for ASSUME.
    DeleteBranch(directives[directive_id].directive_node);
    directives.erase(directive_id);
  }
}

size_t execute_directive(string& directive_as_string,
                         shared_ptr<NodeEvaluation> directive_node) {
  shared_ptr<NodeEvaluation> last_directive_node;
  if (directives.size() > 0) {
    last_directive_node = GetLastDirectiveNode();
  }
  Evaluator(directive_node, global_environment, shared_ptr<Node>(), shared_ptr<NodeEvaluation>());
  directive_node->earlier_evaluation_nodes = last_directive_node;

  last_directive_id++;
  directives.insert(pair<size_t, directive_entry>(last_directive_id, directive_entry(directive_as_string, directive_node)));
  return last_directive_id;
}

string PythonObjectAsString(PyObject* python_object) {
  PyObject* type = NULL;
  PyObject* pyString =  NULL;
  string result;
  if (python_object != NULL &&
       (pyString=PyObject_Str(python_object))!=NULL && 
       (PyString_Check(pyString))) {
    result = PyString_AsString(pyString);
  } else {
    return string("<Python cannot stringify this object>");
  }
  Py_XDECREF(pyString);
  return result;
}

bool ConvertPythonObjectToVentureValue
  (PyObject* python_object,
   shared_ptr<VentureValue>* pointer_to_shared_pointer)
{
  if (PyString_Check(python_object)) {
    char* string_as_chars = PyString_AsString(python_object);
    *pointer_to_shared_pointer = ProcessAtom(string_as_chars);
  } else if (PyUnicode_Check(python_object)) {
    PyObject* encoded_string = PyUnicode_AsUTF8String(python_object);
    char* string_as_chars = PyString_AsString(encoded_string);
    *pointer_to_shared_pointer = ProcessAtom(string_as_chars);
    Py_XDECREF(encoded_string);
  } else if (PyBool_Check(python_object)) {
    if (python_object == Py_True) {
      *pointer_to_shared_pointer = shared_ptr<VentureBoolean>(new VentureBoolean(true));
    } else if (python_object == Py_False) {
      *pointer_to_shared_pointer = shared_ptr<VentureBoolean>(new VentureBoolean(false));
    } else {
      throw std::exception("Unidentified Python boolean value.");
    }
  } else if (PyInt_Check(python_object)) {
    // Not safe, because Python returns long, not int!
    *pointer_to_shared_pointer = shared_ptr<VentureCount>(new VentureCount(PyInt_AS_LONG(python_object)));
  } else if (PyFloat_Check(python_object)) {
    // Not safe in general, because Python returns double, while we use typedef "real"
    // (which is now "double", though)!
    *pointer_to_shared_pointer = shared_ptr<VentureReal>(new VentureReal(PyFloat_AS_DOUBLE(python_object)));
  } else if (PyList_Check(python_object)) {
    *pointer_to_shared_pointer = NIL_INSTANCE;
    shared_ptr<VentureList> last_cons = NIL_INSTANCE;
    for (Py_ssize_t index = 0; index < PyList_Size(python_object); index++) {
      shared_ptr<VentureValue> next_element;
      ConvertPythonObjectToVentureValue(PyList_GetItem(python_object, index), &next_element);
      if (*pointer_to_shared_pointer == NIL_INSTANCE) { // First element.
        last_cons = shared_ptr<VentureList>(new VentureList(next_element));
        *pointer_to_shared_pointer = last_cons;
      } else {
        last_cons->cdr = shared_ptr<VentureList>(new VentureList(next_element));
        last_cons = last_cons->cdr;
      }
    }
  } else {
    throw std::exception(("Unidentified Python object (its type: '" + PythonObjectAsString(PyObject_Type(python_object)) + "'): '" + PythonObjectAsString(python_object) + "'.").c_str());
    return false;
    // http://docs.python.org/release/1.5.2p2/ext/parseTuple.html
    // "The returned status should be 1 for a successful conversion and 0 if the conversion
    // has failed. When the conversion fails, the converter function should raise an exception."
    // How to understand these two issues at the same time?
  }
  return true;
}

static PyObject*
ForPython__report_value(PyObject *self, PyObject *args)
{ try {
  PauseInference();
  int directive_id;
  if(!PyArg_ParseTuple(args, "i:report_value", &directive_id)) {
    PyErr_SetString(PyExc_TypeError, "report_value: wrong arguments.");
    return NULL; // ReturnInferenceIfNecessary(); ?
  }
  PyObject* returning_python_object = report_value(directive_id)->GetAsPythonObject();
  ReturnInferenceIfNecessary();
  return returning_python_object;
} catch(std::exception& e) { PyErr_SetString(PyExc_Exception, e.what()); return NULL; } }

static PyObject*
ForPython__report_directives(PyObject *self, PyObject *args)
{ try {
  PauseInference();
  if(!PyArg_ParseTuple(args, ":report_directives")) {
    PyErr_SetString(PyExc_TypeError, "report_directives: wrong arguments.");
    return NULL; // ReturnInferenceIfNecessary(); ?
  }
  
  PyObject* returning_list = PyList_New(directives.size());
  size_t index = 0;
  for (map<size_t, directive_entry>::iterator iterator = directives.begin(); iterator != directives.end(); iterator++) {
    PyObject* directive_dictionary = PyDict_New();
    PyDict_SetItemString(directive_dictionary, "directive-id", Py_BuildValue("d", iterator->first));
    PyDict_SetItemString(directive_dictionary, "directive-expression", Py_BuildValue("s", iterator->second.directice_as_string.c_str()));
    if (iterator->second.directive_node->GetNodeType() == DIRECTIVE_ASSUME) {
      PyDict_SetItemString(directive_dictionary,
                           "value",
                           dynamic_pointer_cast<NodeDirectiveAssume>(iterator->second.directive_node)->my_value->GetAsPythonObject());
    }
    if (iterator->second.directive_node->GetNodeType() == DIRECTIVE_PREDICT) {
      PyDict_SetItemString(directive_dictionary,
                           "value",
                           dynamic_pointer_cast<NodeDirectivePredict>(iterator->second.directive_node)->my_value->GetAsPythonObject());
    }
    PyList_SetItem(returning_list, index, directive_dictionary);
    index++;
  }

  ReturnInferenceIfNecessary();
  return returning_list;
} catch(std::exception& e) { PyErr_SetString(PyExc_Exception, e.what()); return NULL; } }

static PyObject*
ForPython__clear(PyObject *self, PyObject *args)
{ try {
  PauseInference();
  if(!PyArg_ParseTuple(args, ":clear")) {
    PyErr_SetString(PyExc_TypeError, "clear: wrong arguments.");
    return NULL; // ReturnInferenceIfNecessary(); ?
  }
  clear_trace();
  ReturnInferenceIfNecessary();
  Py_INCREF(Py_None);
  return Py_None;
} catch(std::exception& e) { PyErr_SetString(PyExc_Exception, e.what()); return NULL; } }

static PyObject*
ForPython__forget(PyObject *self, PyObject *args)
{ try {
  PauseInference();
  int directive_id;
  if(!PyArg_ParseTuple(args, "i:forget", &directive_id)) {
    PyErr_SetString(PyExc_TypeError, "forget: wrong arguments.");
    return NULL; // ReturnInferenceIfNecessary(); ?
  }
  forget_directive(directive_id);
  cout << "Have forgotten" << endl << endl;
  ReturnInferenceIfNecessary();
  Py_INCREF(Py_None);
  return Py_None;
} catch(std::exception& e) { PyErr_SetString(PyExc_Exception, e.what()); return NULL; } }

static PyObject*
ForPython__infer(PyObject *self, PyObject *args)
{ try {
  PauseInference();
  int number_of_required_inferences;
  if(!PyArg_ParseTuple(args, "i:infer", &number_of_required_inferences)) {
    PyErr_SetString(PyExc_TypeError, "infer: wrong arguments.");
    return NULL; // ReturnInferenceIfNecessary(); ?
  }
  if (number_of_required_inferences < 0) {
    PyErr_SetString(PyExc_TypeError, "infer: wrong arguments.");
    return NULL; // ReturnInferenceIfNecessary(); ?
  }
  for (size_t iteration = 0; iteration < number_of_required_inferences; iteration++) {
    MakeMHProposal();
  }
  ReturnInferenceIfNecessary();
  Py_INCREF(Py_None);
  return Py_None;
} catch(std::exception& e) { PyErr_SetString(PyExc_Exception, e.what()); return NULL; } }

static PyObject*
ForPython__start_continuous_inference(PyObject *self, PyObject *args)
{ try {
  if(!PyArg_ParseTuple(args, ":start_continuous_inference")) {
    PyErr_SetString(PyExc_TypeError, "start_continuous_inference: wrong arguments.");
    return NULL; // ReturnInferenceIfNecessary(); ?
  }
  pthread_t new_thread;
  if (continuous_inference_status == 0) {
    continuous_inference_status = 1;
    cout << "Starting thread" << endl;
    pthread_create(&new_thread, NULL, &ContinuousInference, NULL);
    cout << "Have started" << endl;
    Py_INCREF(Py_None);
    return Py_None;
  } else {
    throw std::exception("The continuous inference has been already started.");
  }
} catch(std::exception& e) { PyErr_SetString(PyExc_Exception, e.what()); return NULL; } }

static PyObject*
ForPython__continuous_inference_status(PyObject *self, PyObject *args)
{ try {
  if(!PyArg_ParseTuple(args, ":continuous_inference_status")) {
    PyErr_SetString(PyExc_TypeError, "continuous_inference_status: wrong arguments.");
    return NULL; // ReturnInferenceIfNecessary(); ?
  }
  if (continuous_inference_status == 0) {
    return Py_BuildValue("b", false);
  } else {
    return Py_BuildValue("b", true);
  }
} catch(std::exception& e) { PyErr_SetString(PyExc_Exception, e.what()); return NULL; } }

static PyObject*
ForPython__stop_continuous_inference(PyObject *self, PyObject *args)
{ try {
  if(!PyArg_ParseTuple(args, ":stop_continuous_inference")) {
    PyErr_SetString(PyExc_TypeError, "stop_continuous_inference: wrong arguments.");
    return NULL;
  }
  pthread_t new_thread;
  if (continuous_inference_status != 0) {
    continuous_inference_status = 0;
    // Here we should wait until the inference will realy finish.
    Py_INCREF(Py_None);
    return Py_None;
  } else {
    throw std::exception("The continuous inference is not running.");
  }
} catch(std::exception& e) { PyErr_SetString(PyExc_Exception, e.what()); return NULL; } }

static PyObject*
ForPython__assume(PyObject *self, PyObject *args)
{ try {
  PauseInference();
  char* variable_name_as_chars;
  shared_ptr<VentureValue> expression;
  if(!PyArg_ParseTuple(args, "sO&:assume",
                         &variable_name_as_chars,
                         ConvertPythonObjectToVentureValue, &expression))
  {
    PyErr_SetString(PyExc_TypeError, "assume: wrong arguments.");
    return NULL; // ReturnInferenceIfNecessary(); ?
  }
  shared_ptr<VentureSymbol> variable_name = shared_ptr<VentureSymbol>(new VentureSymbol(variable_name_as_chars));
  string directive_string_representation = "ASSUME " + string(variable_name_as_chars) + " " + expression->GetString();
  size_t directive_id =
    execute_directive(directive_string_representation,
                      shared_ptr<NodeEvaluation>(new NodeDirectiveAssume(variable_name, AnalyzeExpression(expression))));
  shared_ptr<VentureValue> directive_value = report_value(directive_id);
  PyObject* returning_python_object = Py_BuildValue("(iO)", static_cast<int>(directive_id), directive_value->GetAsPythonObject());
  ReturnInferenceIfNecessary();
  return returning_python_object;
} catch(std::exception& e) { PyErr_SetString(PyExc_Exception, e.what()); return NULL; } }

static PyObject*
ForPython__predict(PyObject *self, PyObject *args)
{ try {
  PauseInference();
  shared_ptr<VentureValue> expression;
  if(!PyArg_ParseTuple(args, "O&:predict",
                         ConvertPythonObjectToVentureValue, &expression))
  {
    PyErr_SetString(PyExc_TypeError, "predict: wrong arguments.");
    return NULL; // ReturnInferenceIfNecessary(); ?
  }
  string directive_string_representation = "PREDICT " + expression->GetString();
  size_t directive_id =
    execute_directive(directive_string_representation,
                      shared_ptr<NodeEvaluation>(new NodeDirectivePredict(AnalyzeExpression(expression))));
  shared_ptr<VentureValue> directive_value = report_value(directive_id);
  PyObject* returning_python_object = Py_BuildValue("(iO)", static_cast<int>(directive_id), directive_value->GetAsPythonObject());
  ReturnInferenceIfNecessary();
  return returning_python_object;
} catch(std::exception& e) { PyErr_SetString(PyExc_Exception, e.what()); return NULL; } }

static PyObject*
ForPython__observe(PyObject *self, PyObject *args)
{ try {
  PauseInference();
  char* variable_name_as_chars;
  shared_ptr<VentureValue> expression;
  shared_ptr<VentureValue> literal_value;
  if(!PyArg_ParseTuple(args, "O&O&:observe",
                         ConvertPythonObjectToVentureValue, &expression,
                         ConvertPythonObjectToVentureValue, &literal_value))
  {
    PyErr_SetString(PyExc_TypeError, "observe: wrong arguments.");
    return NULL; // ReturnInferenceIfNecessary(); ?
  }
  string directive_string_representation = "OBSERVE " + expression->GetString() + " " + literal_value->GetString();
  size_t directive_id =
    execute_directive(directive_string_representation,
                      shared_ptr<NodeEvaluation>(new NodeDirectiveObserve(AnalyzeExpression(expression), literal_value)));
  PyObject* returning_python_object = Py_BuildValue("i", static_cast<int>(directive_id));
  ReturnInferenceIfNecessary();
  return returning_python_object;
} catch(std::exception& e) { PyErr_SetString(PyExc_Exception, e.what()); return NULL; } }

static PyMethodDef MethodsForPythons[] = {
    {"report_directives", ForPython__report_directives, METH_VARARGS,
     "... Write description ..."},
    {"report_value", ForPython__report_value, METH_VARARGS,
     "... Write description ..."},
    {"clear", ForPython__clear, METH_VARARGS,
     "... Write description ..."},
    {"forget", ForPython__forget, METH_VARARGS,
     "... Write description ..."},
    {"infer", ForPython__infer, METH_VARARGS,
     "... Write description ..."},
    {"start_continuous_inference", ForPython__start_continuous_inference, METH_VARARGS,
     "... Write description ..."},
    {"continuous_inference_status", ForPython__continuous_inference_status, METH_VARARGS,
     "... Write description ..."},
    {"stop_continuous_inference", ForPython__stop_continuous_inference, METH_VARARGS,
     "... Write description ..."},
    {"assume", ForPython__assume, METH_VARARGS,
     "... Write description ..."},
    {"predict", ForPython__predict, METH_VARARGS,
     "... Write description ..."},
    {"observe", ForPython__observe, METH_VARARGS,
     "... Write description ..."},
    {NULL, NULL, 0, NULL}
};

int main(int argc, char *argv[])
{
  random_generator = gsl_rng_alloc(gsl_rng_mt19937);
  unsigned long seed = 0; // time(NULL)
  cout << "WARNING: RANDOM SEED is not random!" << endl;
  gsl_rng_set(random_generator, seed);

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

  // PyRun_SimpleFile(...) does not work in Release configuration (works in debug).
  // Read here: http://docs.python.org/2/faq/windows.html#pyrun-simplefile-crashes-on-windows-but-not-on-unix-why
  // It seems it is necessary to recompile pythonXY.lib and *.dll.
  // Now using this variant:
  PyRun_SimpleString("execfile(\"C:/Users/Yura Perov/workspace/VentureAlpha/src/RESTPython.py\")");

  //FILE *python_script = fopen("C:/Users/Yura Perov/workspace/VentureAlpha/src/RESTPython.py", "r");
  /*cout << python_script << endl;
  long len;
  char* buf;
  fseek(python_script, 0, SEEK_END); //go to end
  len = ftell(python_script); //get position at end (length)
  fseek(python_script, 0, SEEK_SET); //go to beg.
  buf = (char*) malloc(len); //malloc buffer
  fread(buf, len, 1, python_script); //read into buffer
  cout << buf << endl;*/
  //PyRun_SimpleFile(python_script, "RESTPython.py");
  //PyRun_SimpleString(buf);
  // Why it does not work?: PyRun_SimpleFile(python_script, "RESTPython.py");
  //fclose(python_script);
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
