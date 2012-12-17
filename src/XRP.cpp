
#include "Header.h"
#include "VentureValues.h"
#include "VentureParser.h"
#include "Analyzer.h"
#include "XRP.h"
#include "RIPL.h"

RescorerResamplerResult::RescorerResamplerResult(shared_ptr<VentureValue> new_value,
                        real old_loglikelihood,
                        real new_loglikelihood)
  : new_value(new_value),
    old_loglikelihood(old_loglikelihood),
    new_loglikelihood(new_loglikelihood)
{}
  
ReevaluationResult::ReevaluationResult(shared_ptr<VentureValue> passing_value,
                    bool pass_further)
  : passing_value(passing_value),
    pass_further(pass_further)
{}
  
ReevaluationEntry::ReevaluationEntry(shared_ptr<NodeEvaluation> reevaluation_node,
                  shared_ptr<NodeEvaluation> caller,
                  shared_ptr<VentureValue> passing_value)
  : reevaluation_node(reevaluation_node),
    caller(caller),
    passing_value(passing_value)
{}
  
OmitPattern::OmitPattern(vector<size_t>& omit_pattern,
            vector<size_t>& stop_pattern)
  : omit_pattern(omit_pattern),
    stop_pattern(stop_pattern)
{}
  
ReevaluationParameters::ReevaluationParameters()
  : loglikelihood_changes(0.0),
    random_choices_delta(0)
{}

shared_ptr<VentureValue>
XRP::Sample(vector< shared_ptr<VentureValue> >& arguments,
            shared_ptr<NodeXRPApplication> caller) {
  shared_ptr<VentureValue> new_sample = Sampler(arguments, caller);
  real loglikelihood = GetSampledLoglikelihood(arguments, new_sample);
  Incorporate(arguments, new_sample);
  caller->sampled_value = new_sample;
  return new_sample;
}

shared_ptr<RescorerResamplerResult>
XRP::RescorerResampler(vector< shared_ptr<VentureValue> >& old_arguments,
                       vector< shared_ptr<VentureValue> >& new_arguments,
                       shared_ptr<NodeXRPApplication> caller,
                       bool forced_resampling) {
  Freeze(old_arguments, caller);
  if (forced_resampling || !CouldBeRescored()) {
    Remove(old_arguments, caller->sampled_value);
    real old_loglikelihood = GetSampledLoglikelihood(old_arguments, caller->sampled_value);
    
    shared_ptr<VentureValue> new_sample = Sampler(new_arguments, caller);
    real new_loglikelihood = GetSampledLoglikelihood(new_arguments, new_sample);
    Incorporate(new_arguments, new_sample);
    caller->new_sampled_value = new_sample;

    return shared_ptr<RescorerResamplerResult>(new RescorerResamplerResult(new_sample, 0, 0));
  } else {
    Remove(old_arguments, caller->sampled_value);
    real old_loglikelihood = GetSampledLoglikelihood(old_arguments, caller->sampled_value);
    real new_loglikelihood = GetSampledLoglikelihood(new_arguments, caller->sampled_value);
    Incorporate(new_arguments, caller->sampled_value);
    
    return shared_ptr<RescorerResamplerResult>(
      new RescorerResamplerResult(shared_ptr<VentureValue>(),
                                  old_loglikelihood,
                                  new_loglikelihood));
  }
}

XRP::XRP() {}
shared_ptr<VentureValue> XRP::Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller) {
  throw std::runtime_error("It should not happen.");
} // Should be just ";"?
void XRP::Unsampler(vector< shared_ptr<VentureValue> >& old_arguments, shared_ptr<NodeXRPApplication> caller) {
  
}
void XRP::Freeze(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller) {
  
}
void XRP::Unfreeze(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller) {
  
}
real XRP::GetSampledLoglikelihood(vector< shared_ptr<VentureValue> >& arguments,
                                      shared_ptr<VentureValue> sampled_value) {
  throw std::runtime_error("It should not happen.");
} // Should be just ";"?
void XRP::Incorporate(vector< shared_ptr<VentureValue> >& arguments,
                              shared_ptr<VentureValue> sampled_value) {
  throw std::runtime_error("It should not happen.");
} // Should be just ";"?
void XRP::Remove(vector< shared_ptr<VentureValue> >& arguments,
                          shared_ptr<VentureValue> sampled_value) {
  throw std::runtime_error("It should not happen.");
} // Should be just ";"?
  
bool XRP::IsRandomChoice() { return false; }
bool XRP::CouldBeRescored() { return false; }
string XRP::GetName() { return "XRPClass"; }

// CRPmaker
shared_ptr<VentureValue> XRP__CRPmaker::Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller) {
  if (arguments.size() != 1) {
    throw std::runtime_error("Wrong number of arguments.");
  }
  VentureSmoothedCount::CheckMyData(arguments[0].get());

  shared_ptr<XRP> new_xrp = shared_ptr<XRP>(new XRP__CRPsampler()); // If we put here XRP__CRPmaker() (it is stupid, but nevertheless), it raises some memory error. Why?
  // Should be done on the line above (through the XRP__CRPsampler constructor!):
  dynamic_pointer_cast<XRP__CRPsampler>(new_xrp)->alpha = arguments[0]->GetReal();
  dynamic_pointer_cast<XRP__CRPsampler>(new_xrp)->current_number_of_clients = 0;
  return shared_ptr<VentureXRP>(new VentureXRP(new_xrp));
}

real XRP__CRPmaker::GetSampledLoglikelihood(vector< shared_ptr<VentureValue> >& arguments,
                                      shared_ptr<VentureValue> sampled_value) {
  return log(1.0); // ?
}

void XRP__CRPmaker::Incorporate(vector< shared_ptr<VentureValue> >& arguments,
                              shared_ptr<VentureValue> sampled_value) {
}

void XRP__CRPmaker::Remove(vector< shared_ptr<VentureValue> >& arguments,
                          shared_ptr<VentureValue> sampled_value) {
}
bool XRP__CRPmaker::IsRandomChoice() { return false; }
bool XRP__CRPmaker::CouldBeRescored() { return true; }
string XRP__CRPmaker::GetName() { return "XRP__CRPmaker"; }

// CRPsampler
shared_ptr<VentureValue> XRP__CRPsampler::Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller) {
  if (arguments.size() != 0) {
    throw std::runtime_error("Wrong number of arguments.");
  }
  double random_uniform_0_1 = gsl_ran_flat(random_generator, 0, 1);
  double accumulated_probability = 0.0;
  double expected_sum = this->alpha +
                          this->current_number_of_clients;
  for (map<int, size_t>::const_iterator iterator = this->atoms.begin();
         iterator != this->atoms.end();
         iterator++) {
    accumulated_probability += static_cast<double>((*iterator).second) / expected_sum;
    if (random_uniform_0_1 <= accumulated_probability) {
      return shared_ptr<VentureAtom>(new VentureAtom((*iterator).first));
    }
  }
  next_gensym_atom++;
  //cout << "Resampling XRP" << next_gensym_atom << endl;
  return shared_ptr<VentureAtom>(new VentureAtom(next_gensym_atom));
}

real XRP__CRPsampler::GetSampledLoglikelihood(vector< shared_ptr<VentureValue> >& arguments,
                                      shared_ptr<VentureValue> sampled_value) {
  if (this->atoms.count(ToVentureType<VentureAtom>(sampled_value)->data) == 0) {
    return log(this->alpha /
                (this->alpha +
                  this->current_number_of_clients));
  } else {
    // Not 100% true, but it is okay.
    return log(this->atoms[ToVentureType<VentureAtom>(sampled_value)->data] /
                (this->alpha +
                  this->current_number_of_clients));
  }
}

void XRP__CRPsampler::Incorporate(vector< shared_ptr<VentureValue> >& arguments,
                              shared_ptr<VentureValue> sampled_value) {
  if (this->atoms.count(ToVentureType<VentureAtom>(sampled_value)->data) == 1) {
    this->atoms[ToVentureType<VentureAtom>(sampled_value)->data]++;
  } else {
    this->atoms[ToVentureType<VentureAtom>(sampled_value)->data] = 1;
  }
  this->current_number_of_clients++;
}

void XRP__CRPsampler::Remove(vector< shared_ptr<VentureValue> >& arguments,
                          shared_ptr<VentureValue> sampled_value) {
  if (this->atoms.count(ToVentureType<VentureAtom>(sampled_value)->data) == 0) {
    throw std::runtime_error("CRP statistics does not have this atom.");
  } else {
    this->atoms[ToVentureType<VentureAtom>(sampled_value)->data]--;
    if (this->atoms[ToVentureType<VentureAtom>(sampled_value)->data] == 0) {
      this->atoms.erase(ToVentureType<VentureAtom>(sampled_value)->data);
    }
  }
  this->current_number_of_clients--;
}
bool XRP__CRPsampler::IsRandomChoice() { return true; }
bool XRP__CRPsampler::CouldBeRescored() { return true; }
string XRP__CRPsampler::GetName() { return "XRP__CRPsampler"; }





shared_ptr<VentureValue> XRP__memoizer::Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller) {
  if (arguments.size() != 1) {
    throw std::runtime_error("Wrong number of arguments.");
  }
  // Check that it is LAMBDA or XRPApplication.

  shared_ptr<XRP> new_xrp = shared_ptr<XRP>(new XRP__memoized_procedure());
  dynamic_pointer_cast<XRP__memoized_procedure>(new_xrp)->operator_value = arguments[0]; // Should be done on the line above through its constructor.
  return shared_ptr<VentureXRP>(new VentureXRP(new_xrp));
}

real XRP__memoizer::GetSampledLoglikelihood(vector< shared_ptr<VentureValue> >& arguments,
                                      shared_ptr<VentureValue> sampled_value) {
  return log(1.0); // ?
}

void XRP__memoizer::Incorporate(vector< shared_ptr<VentureValue> >& arguments,
                              shared_ptr<VentureValue> sampled_value) {
}

void XRP__memoizer::Remove(vector< shared_ptr<VentureValue> >& arguments,
                          shared_ptr<VentureValue> sampled_value) {
}
bool XRP__memoizer::IsRandomChoice() { return false; }
bool XRP__memoizer::CouldBeRescored() { return false; }
string XRP__memoizer::GetName() { return "XRP__memoizer"; }






string XRP__memoized_procedure__MakeMapKeyFromArguments(vector< shared_ptr<VentureValue> >& arguments) {
  string arguments_strings = "(";
  for (size_t index = 0; index < arguments.size(); index++) {
    if (index != 0) { arguments_strings += " "; }
    arguments_strings += arguments[index]->GetString();
  }
  arguments_strings += ")";
  return arguments_strings;
}

shared_ptr<VentureValue> XRP__memoized_procedure::Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller) {
  string mem_table_key = XRP__memoized_procedure__MakeMapKeyFromArguments(arguments);

  if (this->mem_table.count(mem_table_key) == 0) {
    shared_ptr<NodeSelfEvaluating> operator_node =
      shared_ptr<NodeSelfEvaluating>(new NodeSelfEvaluating(operator_value));
    vector< shared_ptr<NodeEvaluation> > operands_nodes;
    for (size_t index = 0; index < arguments.size(); index++) {
      operands_nodes.push_back(shared_ptr<NodeSelfEvaluating>(new NodeSelfEvaluating(arguments[index])));
    }
    shared_ptr<NodeApplicationCaller> application_caller =
      shared_ptr<NodeApplicationCaller>(new NodeApplicationCaller(operator_node, operands_nodes));
    
    this->mem_table.insert
      (pair<string, XRP__memoizer_map_element>(mem_table_key, XRP__memoizer_map_element(application_caller)));

    (*(this->mem_table.find(mem_table_key))).second.result =
      Evaluator(this->mem_table[mem_table_key].application_caller_node,
                caller->environment, // FIXME: Is it okay?
                caller->parent, // FIXME: Is it okay?
                caller);
  } else {
    // Adding the output reference link by hand.
    this->mem_table[mem_table_key].application_caller_node
      ->output_references.insert(caller->parent);
  }
  XRP__memoizer_map_element& mem_table_element =
    (*(this->mem_table.find(mem_table_key))).second;

  mem_table_element.hidden_uses++;
  return mem_table_element.result;
}

void XRP__memoized_procedure::Unsampler(vector< shared_ptr<VentureValue> >& old_arguments, shared_ptr<NodeXRPApplication> caller) {
  string mem_table_key =
    XRP__memoized_procedure__MakeMapKeyFromArguments(old_arguments);
  if (this->mem_table.count(mem_table_key) == 0) {
    throw std::runtime_error("Cannot find the necessary key in the mem table.");
  }
  XRP__memoizer_map_element& mem_table_element =
    (*(this->mem_table.find(mem_table_key))).second;
  if (caller->frozen == true) {
    throw std::runtime_error("We should be never frozen at this time!"); // FIXME: What about the multi-thread version?
  }
  if (mem_table_element.hidden_uses == 0) {
    throw std::runtime_error("(1) Cannot do 'mem_table_element.hidden_uses--'.");
  }
  mem_table_element.hidden_uses--;
  if (mem_table_element.hidden_uses == 0 && mem_table_element.active_uses == 0) {
    //cout << "*** Deleting the mem node" << endl;
    ApplyToMeAndAllMyChildren(mem_table_element.application_caller_node, DeleteNode);
    this->mem_table.erase(mem_table_key);
  }
}

void XRP__memoized_procedure::Freeze(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller) {
  if (caller->frozen == true) {
    throw std::runtime_error("We should not be frozen yet at this time!");
  }
  string mem_table_key = XRP__memoized_procedure__MakeMapKeyFromArguments(arguments);
  if (this->mem_table.count(mem_table_key) == 0) {
    throw std::runtime_error("Cannot find the necessary key in the mem table.");
  }
  XRP__memoizer_map_element& mem_table_element =
    (*(this->mem_table.find(mem_table_key))).second;
  mem_table_element.frozen_elements++;
  caller->frozen = true;
}

void XRP__memoized_procedure::Unfreeze(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller) {
  if (caller->frozen == false) {
    throw std::runtime_error("We should be already frozen at this time!");
  }
  string mem_table_key = XRP__memoized_procedure__MakeMapKeyFromArguments(arguments);
  if (this->mem_table.count(mem_table_key) == 0) {
    throw std::runtime_error("Cannot find the necessary key in the mem table.");
  }
  XRP__memoizer_map_element& mem_table_element =
    (*(this->mem_table.find(mem_table_key))).second;
  mem_table_element.frozen_elements--;
  caller->frozen = false;
}

real XRP__memoized_procedure::GetSampledLoglikelihood(vector< shared_ptr<VentureValue> >& arguments,
                                      shared_ptr<VentureValue> sampled_value) {
  return log(1.0); // ?
}

void XRP__memoized_procedure::Incorporate(vector< shared_ptr<VentureValue> >& arguments,
                              shared_ptr<VentureValue> sampled_value) {
  string mem_table_key = XRP__memoized_procedure__MakeMapKeyFromArguments(arguments);
  if (this->mem_table.count(mem_table_key) == 0) {
    throw std::runtime_error("Cannot find the necessary key in the mem table.");
  }
  XRP__memoizer_map_element& mem_table_element =
    (*(this->mem_table.find(mem_table_key))).second;
  if (mem_table_element.hidden_uses == 0) {
    throw std::runtime_error("(2) Cannot do 'mem_table_element.hidden_uses--'.");
  }
  mem_table_element.hidden_uses--;
  mem_table_element.active_uses++;
}

void XRP__memoized_procedure::Remove(vector< shared_ptr<VentureValue> >& arguments,
                          shared_ptr<VentureValue> sampled_value) {
  string mem_table_key = XRP__memoized_procedure__MakeMapKeyFromArguments(arguments);
  if (this->mem_table.count(mem_table_key) == 0) {
    throw std::runtime_error("Cannot find the necessary key in the mem table.");
  }
  XRP__memoizer_map_element& mem_table_element =
    (*(this->mem_table.find(mem_table_key))).second;
  if (mem_table_element.active_uses == 0) {
    throw std::runtime_error("Cannot do 'mem_table_element.active_uses--'.");
  }
  mem_table_element.hidden_uses++;
  mem_table_element.active_uses--;
}
bool XRP__memoized_procedure::IsRandomChoice() { return false; }
bool XRP__memoized_procedure::CouldBeRescored() { return false; }
string XRP__memoized_procedure::GetName() { return "XRP__memoized_procedure"; }
