
#include "HeaderPre.h"
#include "PythonProxy.h"

// Call this error type when it is necessary just to pass the Python error.
handling_python_error::handling_python_error() : std::runtime_error("No text is required here.") {}

PyMethodDef MethodsForPythons[] = {
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
    {"draw_graph_to_file", ForPython__draw_graph_to_file, METH_VARARGS,
     "... Write description ..."},
    {"logscore", ForPython__logscore, METH_VARARGS,
     "... Write description ..."},
#ifdef _VENTURE_USE_GOOGLE_PROFILER
    {"_start_profiler", ForPython___start_profiler, METH_VARARGS,
     "... Write description ..."},
    {"_stop_profiler", ForPython___stop_profiler, METH_VARARGS,
     "... Write description ..."},
#endif
    {"_exit", ForPython___exit, METH_VARARGS,
     "... Write description ..."},
    {NULL, NULL, 0, NULL}
};

string PythonObjectAsString(PyObject* python_object) {
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
  PyObject *suger_processing__main_module__name;
  PyObject *suger_processing__main_module;
  PyObject *suger_processing__process_sugars;
  PyObject *suger_processing__process_sugars__arguments;
  
  suger_processing__main_module__name = PyString_FromString("__main__");
  if (suger_processing__main_module__name == NULL) {
    throw std::runtime_error("Strange, cannot create the string '__main__'.");
  }
  suger_processing__main_module = PyImport_Import(suger_processing__main_module__name);
  Py_DECREF(suger_processing__main_module__name);
  if (suger_processing__main_module__name == NULL) {
    throw std::runtime_error("Strange, cannot find the module '__main__'.");
  }
  suger_processing__process_sugars = PyObject_GetAttrString(suger_processing__main_module, "process_sugars");
  Py_DECREF(suger_processing__main_module);
  if (suger_processing__main_module__name == NULL) {
    throw std::runtime_error("For some reason the function 'process_sugars' is not defined.");
  }
  suger_processing__process_sugars__arguments = PyTuple_Pack(1, python_object);
  if (suger_processing__main_module__name == NULL) {
    throw std::runtime_error("Cannot execute the function 'python_object' with the provided argument.");
  }

  python_object = PyObject_CallObject(suger_processing__process_sugars, suger_processing__process_sugars__arguments);
  Py_DECREF(suger_processing__process_sugars);
  Py_DECREF(suger_processing__process_sugars__arguments);
  if (python_object == NULL) {
    // Assuming that Python raised some error.
    // Pass this error further.
    throw handling_python_error();
    /* Old code to delete:
      PyObject* error_string__as_Python_object = PyObject_Str(PyExc_Exception);
      if (error_string__as_Python_object == NULL) {
        throw std::runtime_error("The function 'python_object' has raised an error (or was not evaluated for some other reason). Cannot get the error message.");
      }
      const char* error_string = PyString_AsString(error_string__as_Python_object);
      Py_DECREF(error_string__as_Python_object);
      throw std::runtime_error("The function 'python_object' has raised an error (or was not evaluated for some other reason). The error message: " + string(error_string));
    */
  }

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
      throw std::runtime_error("Unidentified Python boolean value.");
    }
  } else if (PyInt_Check(python_object)) {
    // Not safe, because Python returns long, not double!
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
    Py_DECREF(python_object);
    throw std::runtime_error(("Unidentified Python object (its type: '" + PythonObjectAsString(PyObject_Type(python_object)) + "'): '" + PythonObjectAsString(python_object) + "'.").c_str()); // FIXME: Decrement is necessary?
    return false;
    // http://docs.python.org/release/1.5.2p2/ext/parseTuple.html
    // "The returned status should be 1 for a successful conversion and 0 if the conversion
    // has failed. When the conversion fails, the converter function should raise an exception."
    // How to understand these two issues at the same time?
  }
  Py_DECREF(python_object);
  return true;
}

PyObject*
ForPython__report_value(PyObject *self, PyObject *args)
{ try {
  PauseInference();
  int directive_id;
  if(!PyArg_ParseTuple(args, "i:report_value", &directive_id)) {
    PyErr_SetString(PyExc_TypeError, "report_value: wrong arguments.");
    return NULL; // ReturnInferenceIfNecessary(); ?
  }
  PyObject* returning_python_object = ReportValue(directive_id)->GetAsPythonObject();
  ReturnInferenceIfNecessary();
  return returning_python_object;
} catch(handling_python_error&) { return NULL; } catch(std::runtime_error& e) { PyErr_SetString(PyExc_Exception, e.what()); return NULL; } }

PyObject*
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
    PyDict_SetItemString(directive_dictionary, "directive-id", Py_BuildValue("i", static_cast<int>(iterator->first)));
    PyDict_SetItemString(directive_dictionary, "directive-expression", Py_BuildValue("s", ("(" + iterator->second.directice_as_string + ")").c_str())); // Delete ( and ) in the future?
    if (iterator->second.directive_node->GetNodeType() == DIRECTIVE_ASSUME) {
      PyDict_SetItemString(directive_dictionary, "directive-type", Py_BuildValue("s", "DIRECTIVE-ASSUME"));
      PyDict_SetItemString(directive_dictionary, "name", Py_BuildValue("s", dynamic_pointer_cast<NodeDirectiveAssume>(iterator->second.directive_node)->name->GetString().c_str()));
    } else if (iterator->second.directive_node->GetNodeType() == DIRECTIVE_PREDICT) {
      PyDict_SetItemString(directive_dictionary, "directive-type", Py_BuildValue("s", "DIRECTIVE-PREDICT"));
    } else if (iterator->second.directive_node->GetNodeType() == DIRECTIVE_OBSERVE) {
      PyDict_SetItemString(directive_dictionary, "directive-type", Py_BuildValue("s", "DIRECTIVE-OBSERVE"));
    } else {
      throw std::runtime_error("Strange directive: " + boost::lexical_cast<string>(iterator->second.directive_node->GetType()));
    }
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
} catch(handling_python_error&) { return NULL; } catch(std::runtime_error& e) { PyErr_SetString(PyExc_Exception, e.what()); return NULL; } }

PyObject*
ForPython__clear(PyObject *self, PyObject *args)
{ try {
  PauseInference();
  if(!PyArg_ParseTuple(args, ":clear")) {
    PyErr_SetString(PyExc_TypeError, "clear: wrong arguments.");
    return NULL; // ReturnInferenceIfNecessary(); ?
  }
  ClearRIPL();
  ReturnInferenceIfNecessary();
  Py_INCREF(Py_None);
  return Py_None;
} catch(handling_python_error&) { return NULL; } catch(std::runtime_error& e) { PyErr_SetString(PyExc_Exception, e.what()); return NULL; } }

PyObject*
ForPython__forget(PyObject *self, PyObject *args)
{ try {
  PauseInference();
  int directive_id;
  if(!PyArg_ParseTuple(args, "i:forget", &directive_id)) {
    PyErr_SetString(PyExc_TypeError, "forget: wrong arguments.");
    return NULL; // ReturnInferenceIfNecessary(); ?
  }
  ForgetDirective(directive_id);
  //cout << "Have forgotten" << endl << endl;
  ReturnInferenceIfNecessary();
  Py_INCREF(Py_None);
  return Py_None;
} catch(handling_python_error&) { return NULL; } catch(std::runtime_error& e) { PyErr_SetString(PyExc_Exception, e.what()); return NULL; } }

PyObject*
ForPython__infer(PyObject *self, PyObject *args)
{ //try {
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
  for (size_t iteration = 0; iteration < static_cast<size_t>(number_of_required_inferences); iteration++) {
    MakeMHProposal(shared_ptr<NodeXRPApplication>(),
                   shared_ptr<VentureValue>(),
                   shared_ptr< map<string, shared_ptr<VentureValue> > >(),
                   false);
  }

  //stack< shared_ptr<Node> > tmp;
  //DrawGraphDuringMH(GetLastDirectiveNode(), tmp);

  ReturnInferenceIfNecessary();
  Py_INCREF(Py_None);
  return Py_None;
} //catch(handling_python_error&) { return NULL; } catch(std::runtime_error& e) { PyErr_SetString(PyExc_Exception, e.what()); return NULL; } }

PyObject*
ForPython__start_continuous_inference(PyObject *self, PyObject *args)
{ try {
  if(!PyArg_ParseTuple(args, ":start_continuous_inference")) {
    PyErr_SetString(PyExc_TypeError, "start_continuous_inference: wrong arguments.");
    return NULL; // ReturnInferenceIfNecessary(); ?
  }
  pthread_t new_thread;
  if (continuous_inference_status == 0) {
    continuous_inference_status = 1;
    // cout << "Starting thread" << endl;
    pthread_create(&new_thread, NULL, &ContinuousInference, NULL);
    // cout << "Have started" << endl;
    Py_INCREF(Py_None);
    return Py_None;
  } else {
    throw std::runtime_error("The continuous inference has been already started.");
  }
} catch(handling_python_error&) { return NULL; } catch(std::runtime_error& e) { PyErr_SetString(PyExc_Exception, e.what()); return NULL; } }

PyObject*
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
} catch(handling_python_error&) { return NULL; } catch(std::runtime_error& e) { PyErr_SetString(PyExc_Exception, e.what()); return NULL; } }

PyObject*
ForPython__stop_continuous_inference(PyObject *self, PyObject *args)
{ try {
  if(!PyArg_ParseTuple(args, ":stop_continuous_inference")) {
    PyErr_SetString(PyExc_TypeError, "stop_continuous_inference: wrong arguments.");
    return NULL;
  }
  if (continuous_inference_status != 0) {
    continuous_inference_status = 0;
    // Here we should wait until the inference will realy finish.
    Py_INCREF(Py_None);
    return Py_None;
  } else {
    throw std::runtime_error("The continuous inference is not running.");
  }
} catch(handling_python_error&) { return NULL; } catch(std::runtime_error& e) { PyErr_SetString(PyExc_Exception, e.what()); return NULL; } }

PyObject*
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
    ExecuteDirectiveWithRejectionSampling(directive_string_representation,
                                          shared_ptr<NodeEvaluation>(new NodeDirectiveAssume(variable_name, AnalyzeExpression(expression))),
                                          expression);
  GetLastDirectiveNode()->comment = directive_string_representation;
  shared_ptr<VentureValue> directive_value = ReportValue(directive_id);
  PyObject* returning_python_object = Py_BuildValue("(iO)", static_cast<int>(directive_id), directive_value->GetAsPythonObject());
  ReturnInferenceIfNecessary();
  return returning_python_object;
} catch(handling_python_error&) { return NULL; } catch(std::runtime_error& e) { PyErr_SetString(PyExc_Exception, e.what()); return NULL; } }

PyObject*
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
    ExecuteDirectiveWithRejectionSampling(directive_string_representation,
                                          shared_ptr<NodeEvaluation>(new NodeDirectivePredict(AnalyzeExpression(expression))),
                                          expression);
  GetLastDirectiveNode()->comment = directive_string_representation;
  shared_ptr<VentureValue> directive_value = ReportValue(directive_id);
  PyObject* returning_python_object = Py_BuildValue("(iO)", static_cast<int>(directive_id), directive_value->GetAsPythonObject());
  ReturnInferenceIfNecessary();
  return returning_python_object;
} catch(handling_python_error&) { return NULL; } catch(std::runtime_error& e) { PyErr_SetString(PyExc_Exception, e.what()); return NULL; } }

PyObject*
ForPython__observe(PyObject *self, PyObject *args)
{ try {
  PauseInference();
  //cout << "Starting to deal with OBSERVE" << endl;
  shared_ptr<VentureValue> expression;
  shared_ptr<VentureValue> literal_value;
  if(!PyArg_ParseTuple(args, "O&O&:observe",
                         ConvertPythonObjectToVentureValue, &expression,
                         ConvertPythonObjectToVentureValue, &literal_value))
  {
    PyErr_SetString(PyExc_TypeError, "observe: wrong arguments.");
    return NULL; // ReturnInferenceIfNecessary(); ?
  }
  //cout << "OBSERVE IS HERE" << endl;
  string directive_string_representation = "OBSERVE " + expression->GetString() + " " + literal_value->GetString();
  //cout << literal_value->GetType() << endl;
  //cout << directive_string_representation << endl;
  size_t directive_id =
    ExecuteDirectiveWithRejectionSampling(directive_string_representation,
                                          shared_ptr<NodeEvaluation>(new NodeDirectiveObserve(AnalyzeExpression(expression), literal_value)),
                                          expression);
  GetLastDirectiveNode()->comment = directive_string_representation;
  PyObject* returning_python_object = Py_BuildValue("i", static_cast<int>(directive_id));
  ReturnInferenceIfNecessary();

  //stack< shared_ptr<Node> > tmp;
  //DrawGraphDuringMH(GetLastDirectiveNode(), tmp);

  //cout << "Finishing to deal with OBSERVE" << endl;
  return returning_python_object;
} catch(handling_python_error&) { return NULL; } catch(std::runtime_error& e) { PyErr_SetString(PyExc_Exception, e.what()); return NULL; } }
  
PyObject*
ForPython__draw_graph_to_file(PyObject *self, PyObject *args) // FIXME: deprecated?
{ try {
  PauseInference();
  if(!PyArg_ParseTuple(args, ":draw_graph_to_file"))
  {
    PyErr_SetString(PyExc_TypeError, "draw_graph_to_file: wrong arguments.");
    return NULL; // ReturnInferenceIfNecessary(); ?
  }
  stack< shared_ptr<Node> > tmp;
  DrawGraphDuringMH(GetLastDirectiveNode(), tmp);
  ReturnInferenceIfNecessary();

  //cout << "Finishing to deal with OBSERVE" << endl;
  return Py_BuildValue("i", static_cast<int>(0)); // FIXME: something wiser.
} catch(handling_python_error&) { return NULL; } catch(std::runtime_error& e) { PyErr_SetString(PyExc_Exception, e.what()); return NULL; } }

PyObject*
ForPython__logscore(PyObject *self, PyObject *args) // FIXME: deprecated?
{ try {
  PauseInference();
  if(!PyArg_ParseTuple(args, ":logscore"))
  {
    PyErr_SetString(PyExc_TypeError, "logscore: wrong arguments.");
    return NULL; // ReturnInferenceIfNecessary(); ?
  }
  double logscore = GetLogscoreOfAllDirectives();
  ReturnInferenceIfNecessary();

  //cout << "Finishing to deal with OBSERVE" << endl;
  return Py_BuildValue("d", logscore); // FIXME: something wiser.
} catch(handling_python_error&) { return NULL; } catch(std::runtime_error& e) { PyErr_SetString(PyExc_Exception, e.what()); return NULL; } }

#ifdef _VENTURE_USE_GOOGLE_PROFILER
PyObject*
ForPython___start_profiler(PyObject *self, PyObject *args)
{ try {
  if(!PyArg_ParseTuple(args, ":_start_profiler"))
  {
    PyErr_SetString(PyExc_TypeError, "_start_profiler: wrong arguments.");
    return NULL; // ReturnInferenceIfNecessary(); ?
  }
  ProfilerStart("profiler_result.txt");
} catch(handling_python_error&) { return NULL; } catch(std::runtime_error& e) { PyErr_SetString(PyExc_Exception, e.what()); return NULL; } }
PyObject*
ForPython___stop_profiler(PyObject *self, PyObject *args)
{ try {
  if(!PyArg_ParseTuple(args, ":_stop_profiler"))
  {
    PyErr_SetString(PyExc_TypeError, "_stop_profiler: wrong arguments.");
    return NULL; // ReturnInferenceIfNecessary(); ?
  }
  ProfilerStop();
} catch(handling_python_error&) { return NULL; } catch(std::runtime_error& e) { PyErr_SetString(PyExc_Exception, e.what()); return NULL; } }
#endif

PyObject*
ForPython___exit(PyObject *self, PyObject *args)
{ try {
  if(!PyArg_ParseTuple(args, ":_exit"))
  {
    PyErr_SetString(PyExc_TypeError, "_exit: wrong arguments.");
    return NULL; // ReturnInferenceIfNecessary(); ?
  }

  exit(0);
} catch(handling_python_error&) { return NULL; } catch(std::runtime_error& e) { PyErr_SetString(PyExc_Exception, e.what()); return NULL; } }
