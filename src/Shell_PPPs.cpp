#include "HeaderPre.h"
#include "PythonProxy.h"
#include "Shell_PPPs.h"

// Deterministic procedures.

shared_ptr<VentureValue> Primitive__GenerateEmptySurfaceAndPMapPrior::Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller, EvaluationConfig& evaluation_config) {
  if(arguments.size() != 0) {
    throw std::runtime_error("Wrong number of arguments.");
  }
  
  return ExecutePythonFunction("Shell", "generate_empty_surface_and_pmap_prior", arguments);
}
string Primitive__GenerateEmptySurfaceAndPMapPrior::GetName() { return "Primitive__GenerateEmptySurfaceAndPMapPrior"; }



shared_ptr<VentureValue> Primitive__UpdatePMapAndAddLobe::Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller, EvaluationConfig& evaluation_config) {
  if(arguments.size() != 5) {
    throw std::runtime_error("Wrong number of arguments.");
  }
  
  return ExecutePythonFunction("Shell", "update_p_map_and_add_lobe", arguments);
}
string Primitive__UpdatePMapAndAddLobe::GetName() { return "Primitive__UpdatePMapAndAddLobe"; }



shared_ptr<VentureValue> Primitive__SaveToFile::Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller, EvaluationConfig& evaluation_config) {
  if(arguments.size() != 1) {
    throw std::runtime_error("Wrong number of arguments.");
  }
  
  return ExecutePythonFunction("Shell", "save_to_file", arguments);
}
string Primitive__SaveToFile::GetName() { return "Primitive__SaveToFile"; }




// Elementary probabilistic procedure.

real ERP__GetLobePosX::GetSampledLoglikelihood(vector< shared_ptr<VentureValue> >& arguments,
                                shared_ptr<VentureValue> sampled_value) { // inline?
  if (arguments.size() != 1) {
    throw std::runtime_error("Wrong number of arguments.");
  }
  
  vector< shared_ptr<VentureValue> > arguments_to_python = arguments;
  arguments_to_python.push_back(sampled_value);

  return PyFloat_AsDouble(ExecutePythonFunction("Shell", "get_lobe_pos_x__logscore", arguments_to_python)->GetAsPythonObject());
}


shared_ptr<VentureValue> ERP__GetLobePosX::Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller, EvaluationConfig& evaluation_config) {
  if (arguments.size() != 1) {
    throw std::runtime_error("Wrong number of arguments.");
  }
  
  return ExecutePythonFunction("Shell", "get_lobe_pos_x", arguments);
}
string ERP__GetLobePosX::GetName() { return "ERP__GetLobePosX"; }




real ERP__GetLobePosY::GetSampledLoglikelihood(vector< shared_ptr<VentureValue> >& arguments,
                                shared_ptr<VentureValue> sampled_value) { // inline?
  if (arguments.size() != 1) {
    throw std::runtime_error("Wrong number of arguments.");
  }
  
  vector< shared_ptr<VentureValue> > arguments_to_python = arguments;
  arguments_to_python.push_back(sampled_value);
  
  return PyFloat_AsDouble(ExecutePythonFunction("Shell", "get_lobe_pos_y__logscore", arguments_to_python)->GetAsPythonObject());
}

shared_ptr<VentureValue> ERP__GetLobePosY::Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller, EvaluationConfig& evaluation_config) {
  if (arguments.size() != 1) {
    throw std::runtime_error("Wrong number of arguments.");
  }
  
  return ExecutePythonFunction("Shell", "get_lobe_pos_y", arguments);
}
string ERP__GetLobePosY::GetName() { return "ERP__GetLobePosY"; }


real ERP__NoisyDrillWell::GetSampledLoglikelihood(vector< shared_ptr<VentureValue> >& arguments,
                                shared_ptr<VentureValue> sampled_value) { // inline?
  if (arguments.size() != 3) {
    throw std::runtime_error("Wrong number of arguments.");
  }
  
  vector< shared_ptr<VentureValue> > arguments_to_python = arguments;
  arguments_to_python.push_back(sampled_value);
  
  return PyFloat_AsDouble(ExecutePythonFunction("Shell", "noisy_drill_well__logscore", arguments_to_python)->GetAsPythonObject());
}
shared_ptr<VentureValue> ERP__NoisyDrillWell::Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller, EvaluationConfig& evaluation_config) {
  if (arguments.size() != 3) {
    throw std::runtime_error("Wrong number of arguments.");
  }
  
  return ExecutePythonFunction("Shell", "noisy_drill_well", arguments);
}
string ERP__NoisyDrillWell::GetName() { return "ERP__NoisyDrillWell"; }
