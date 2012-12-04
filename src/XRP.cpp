
#include "Header.h"
#include "VentureValues.h"
#include "VentureParser.h"
#include "Analyzer.h"
#include "XRP.h"

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
  shared_ptr<VentureValue> new_sample = Sampler(arguments);
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
  if (forced_resampling || !CouldBeRescored()) {
    Remove(old_arguments, caller->sampled_value);
    real old_loglikelihood = GetSampledLoglikelihood(old_arguments, caller->sampled_value);
    
    shared_ptr<VentureValue> new_sample = Sampler(new_arguments);
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
shared_ptr<VentureValue> XRP::Sampler(vector< shared_ptr<VentureValue> >& arguments) {
  throw std::exception("It should not happen.");
} // Should be just ";"?
real XRP::GetSampledLoglikelihood(vector< shared_ptr<VentureValue> >& arguments,
                                      shared_ptr<VentureValue> sampled_value) {
  throw std::exception("It should not happen.");
} // Should be just ";"?
void XRP::Incorporate(vector< shared_ptr<VentureValue> >& arguments,
                              shared_ptr<VentureValue> sampled_value) {
  throw std::exception("It should not happen.");
} // Should be just ";"?
void XRP::Remove(vector< shared_ptr<VentureValue> >& arguments,
                          shared_ptr<VentureValue> sampled_value) {
  throw std::exception("It should not happen.");
} // Should be just ";"?
  
bool XRP::IsRandomChoice() { return false; }
bool XRP::CouldBeRescored() { return false; }
string XRP::GetName() { return "XRPClass"; }
  
void ERP::Incorporate(vector< shared_ptr<VentureValue> >& arguments,
                  shared_ptr<VentureValue> sampled_value) { // inline?

}
void ERP::Remove(vector< shared_ptr<VentureValue> >& arguments,
            shared_ptr<VentureValue> sampled_value) { // inline?

}
  
bool ERP::IsRandomChoice() { return true; }
bool ERP::CouldBeRescored() { return true; }
string ERP::GetName() { return "ERPClass"; }
  
real ERP__Flip::GetSampledLoglikelihood(vector< shared_ptr<VentureValue> >& arguments,
                                shared_ptr<VentureValue> sampled_value) { // inline?
  real weight;
  if (arguments.size() == 0) {
    weight = 0.5;
  } else if (arguments.size() == 1) {
    weight = ToVentureType<VentureReal>(arguments[0])->data;
  } else {
    throw std::exception("Wrong number of arguments.");
  }
  if (VerifyVentureType<VentureBoolean>(sampled_value) == true) {
    if (CompareValue(sampled_value, shared_ptr<VentureValue>(new VentureBoolean(true)))) {
      return log(weight);
    } else {
      return log(1 - weight);
    }
  } else {
    cout << " " << sampled_value << endl;
    throw std::exception("Wrong value.");
  }
}
shared_ptr<VentureValue> ERP__Flip::Sampler(vector< shared_ptr<VentureValue> >& arguments) {
  real weight;
  if (arguments.size() == 0) {
    weight = 0.5;
  } else if (arguments.size() == 1) {
    weight = ToVentureType<VentureReal>(arguments[0])->data;
  } else {
    throw std::exception("Wrong number of arguments.");
  }
  unsigned int result_int = gsl_ran_bernoulli(random_generator, weight);
  bool result_bool = (result_int == 1);
  return shared_ptr<VentureBoolean>(new VentureBoolean(result_bool));
}
string ERP__Flip::GetName() { return "ERP__Flip"; }

real ERP__Normal::GetSampledLoglikelihood(vector< shared_ptr<VentureValue> >& arguments,
                                shared_ptr<VentureValue> sampled_value) {
  if (arguments.size() == 2) {
    double likelihood =
           gsl_ran_gaussian_pdf(ToVentureType<VentureReal>(sampled_value)->data -
                                  ToVentureType<VentureReal>(arguments[0])->data,
                                ToVentureType<VentureReal>(arguments[1])->data);
    return log(likelihood);
  } else {
    throw std::exception("Wrong number of arguments.");
  }
}
shared_ptr<VentureValue> ERP__Normal::Sampler(vector< shared_ptr<VentureValue> >& arguments) {
  if (arguments.size() == 2) {
    double random_value =
           gsl_ran_gaussian(random_generator,
                            ToVentureType<VentureReal>(arguments[1])->data) +
             ToVentureType<VentureReal>(arguments[0])->data;
    return shared_ptr<VentureReal>(new VentureReal(random_value));
  } else {
    throw std::exception("Wrong number of arguments.");
  }
}
string ERP__Normal::GetName() { return "ERP__Normal"; }

real ERP__UniformDiscrete::GetSampledLoglikelihood(vector< shared_ptr<VentureValue> >& arguments,
                                 shared_ptr<VentureValue> sampled_value) {
  if (arguments.size() == 2) {
    if (ToVentureType<VentureCount>(sampled_value)->data >= ToVentureType<VentureCount>(arguments[0])->data &&
      ToVentureType<VentureCount>(sampled_value)->data <= ToVentureType<VentureCount>(arguments[1])->data) {
      return log(1.0 / (1 + ToVentureType<VentureCount>(arguments[1])->data -
              ToVentureType<VentureCount>(arguments[0])->data));
    } else {
      return log(0.0);
    }
  } else {
    throw std::exception("Wrong number of arguments.");
  }
}
shared_ptr<VentureValue> ERP__UniformDiscrete::Sampler(vector< shared_ptr<VentureValue> >& arguments) {
  if (arguments.size() == 2) {
    int random_value =
           UniformDiscrete(ToVentureType<VentureCount>(arguments[0])->data,
                           ToVentureType<VentureCount>(arguments[1])->data);
    return shared_ptr<VentureCount>(new VentureCount(random_value));
  } else {
    throw std::exception("Wrong number of arguments.");
  }
}
string ERP__UniformDiscrete::GetName() { return "ERP__UniformDiscrete"; }

real ERP__UniformContinuous::GetSampledLoglikelihood(vector< shared_ptr<VentureValue> >& arguments,
                                 shared_ptr<VentureValue> sampled_value) {
  if (arguments.size() == 2) {
    if (ToVentureType<VentureReal>(sampled_value)->data >= ToVentureType<VentureReal>(arguments[0])->data &&
      ToVentureType<VentureReal>(sampled_value)->data <= ToVentureType<VentureReal>(arguments[1])->data) {
      return log(1.0 / (ToVentureType<VentureReal>(arguments[1])->data -
              ToVentureType<VentureReal>(arguments[0])->data));
    } else {
      return log(0.0);
    }
  } else {
    throw std::exception("Wrong number of arguments.");
  }
}
shared_ptr<VentureValue> ERP__UniformContinuous::Sampler(vector< shared_ptr<VentureValue> >& arguments) {
  if (arguments.size() == 2) {
    double random_value =
          gsl_ran_flat(random_generator,
                         ToVentureType<VentureReal>(arguments[0])->data,
                         ToVentureType<VentureReal>(arguments[1])->data);
    return shared_ptr<VentureReal>(new VentureReal(random_value));
  } else {
    throw std::exception("Wrong number of arguments.");
  }
}
string ERP__UniformContinuous::GetName() { return "ERP__UniformContinuous"; }

real ERP__NoisyNegate::GetSampledLoglikelihood(vector< shared_ptr<VentureValue> >& arguments,
                                shared_ptr<VentureValue> sampled_value) { // inline?
  bool boolean_expression;
  real weight;
  if (arguments.size() == 2) {
    boolean_expression = ToVentureType<VentureBoolean>(arguments[0])->data;
    weight = ToVentureType<VentureReal>(arguments[1])->data;
  } else {
    throw std::exception("Wrong number of arguments.");
  }
  if (VerifyVentureType<VentureBoolean>(sampled_value) == true) {
    if (CompareValue(sampled_value, shared_ptr<VentureValue>(new VentureBoolean(boolean_expression)))) {
      return log(1 - weight);
    } else {
      return log(weight);
    }
  } else {
    cout << " " << sampled_value << endl;
    throw std::exception("Wrong value.");
  }
}
shared_ptr<VentureValue> ERP__NoisyNegate::Sampler(vector< shared_ptr<VentureValue> >& arguments) {
  bool boolean_expression;
  real weight;
  if (arguments.size() == 2) {
    boolean_expression = ToVentureType<VentureBoolean>(arguments[0])->data;
    weight = ToVentureType<VentureReal>(arguments[1])->data;
  } else {
    throw std::exception("Wrong number of arguments.");
  }
  unsigned int result_int = gsl_ran_bernoulli(random_generator, weight);
  if (result_int == 1) {
    return shared_ptr<VentureBoolean>(new VentureBoolean(!boolean_expression));
  } else {
    return shared_ptr<VentureBoolean>(new VentureBoolean(boolean_expression));
  }
}
string ERP__NoisyNegate::GetName() { return "ERP__NoisyNegate"; }

real ERP__ConditionERP::GetSampledLoglikelihood(vector< shared_ptr<VentureValue> >& arguments,
                                                shared_ptr<VentureValue> sampled_value) { // inline?
  if (arguments.size() != 3) {
    throw std::exception("Wrong number of arguments.");
  }
  if (StandardPredicate(arguments[0]) &&
      arguments[1] == sampled_value) { // Comparing by reference.
    return log(1.0);
  } else if (!StandardPredicate(arguments[0])
              && arguments[2] == sampled_value) { // Comparing by reference.
    return log(1.0);
  } else {
    return log(0.0);
  }
}
shared_ptr<VentureValue> ERP__ConditionERP::Sampler(vector< shared_ptr<VentureValue> >& arguments) {
  if (arguments.size() != 3) {
    throw std::exception("Wrong number of arguments.");
  }
  if (StandardPredicate(arguments[0])) {
    return arguments[1];
  } else {
    return arguments[2];
  }
}
  
bool ERP__ConditionERP::IsRandomChoice() { return false; }
bool ERP__ConditionERP::CouldBeRescored() { return false; }
string ERP__ConditionERP::GetName() { return "ERP__ConditionERP"; }
  
real Primitive::GetSampledLoglikelihood(vector< shared_ptr<VentureValue> >& arguments,
                                shared_ptr<VentureValue> sampled_value) { // inline?
  return log(1.0);
}
  
bool Primitive::IsRandomChoice() { return false; }
bool Primitive::CouldBeRescored() { return false; }
string Primitive::GetName() { return "PrimitiveClass"; }
  
shared_ptr<VentureValue> Primitive__RealPlus::Sampler(vector< shared_ptr<VentureValue> >& arguments) {
  shared_ptr<VentureReal> result = shared_ptr<VentureReal>(new VentureReal(0.0));
  //for (vector< shared_ptr<VentureValue> >::iterator iterator = arguments.begin();
  //     iterator != arguments.end();
  //     iterator++) {
  for (size_t index = 0; index < arguments.size(); index++) {
    //result->data += dynamic_pointer_cast<VentureInteger>(iterator->get())->data; // Too ambiguous!
    result->data += ToVentureType<VentureReal>(arguments[index])->data; // Too ambiguous!
    // Consider *real*! Consider errors!
  }
  return result;
}
string Primitive__RealPlus::GetName() { return "Primitive__RealPlus"; }

shared_ptr<VentureValue> Primitive__RealMultiply::Sampler(vector< shared_ptr<VentureValue> >& arguments) {
  shared_ptr<VentureReal> result = shared_ptr<VentureReal>(new VentureReal(1.0));
  for (size_t index = 0; index < arguments.size(); index++) {
    result->data *= ToVentureType<VentureReal>(arguments[index])->data;
  }
  return result;
}
string Primitive__RealMultiply::GetName() { return "Primitive__RealMultiply"; }

shared_ptr<VentureValue> Primitive__RealPower::Sampler(vector< shared_ptr<VentureValue> >& arguments) {
  if (arguments.size() != 2) {
    throw std::exception("Wrong number of arguments.");
  }
  shared_ptr<VentureReal> result = shared_ptr<VentureReal>(new VentureReal(0.0));
  result->data
    = pow(ToVentureType<VentureReal>(arguments[0])->data,
          ToVentureType<VentureReal>(arguments[1])->data);
  return result;
}
string Primitive__RealPower::GetName() { return "Primitive__RealPower"; }

shared_ptr<VentureValue> Primitive__CountEqualOrGreater::Sampler(vector< shared_ptr<VentureValue> >& arguments) {
  if (arguments.size() != 2) {
    throw std::exception("Wrong number of arguments.");
  }

  bool result;
  if (ToVentureType<VentureCount>(arguments[0])->data >= ToVentureType<VentureCount>(arguments[1])->data) {
    result = true;
  } else {
    result = false;
  }
  return shared_ptr<VentureBoolean>(new VentureBoolean(result));
}
string Primitive__CountEqualOrGreater::GetName() { return "Primitive__CountEqualOrGreater"; }

shared_ptr<VentureValue> Primitive__List::Sampler(vector< shared_ptr<VentureValue> >& arguments) {
  if (arguments.size() == 0) {
    return NIL_INSTANCE;
  }
  shared_ptr<VentureList> new_list =
    shared_ptr<VentureList>(new VentureList(arguments[0]));
  for (size_t index = 1; index < arguments.size(); index++) {
    // AddToList() is not efficient!
    AddToList(new_list,
              arguments[index]);
  }
  return new_list;
}

string Primitive__List::GetName() { return "Primitive__List"; }

int UniformDiscrete(int a, int b) {
  return gsl_ran_flat(random_generator, a, b + 1);
}

bool VerifyOrderPattern(vector<size_t>& omit_pattern,
                        vector<size_t>& checking_order) {
  for (size_t index = 0; index < omit_pattern.size(); index++) {
    if (index >= checking_order.size()) {
      // If there is no bugs, it means that we have reached
      // the necessary ApplicationCaller.
      return false;
    }
    if (checking_order[index] != omit_pattern[index]) {
      return false;
    }
  }
  return true;
}

void DeleteNode(shared_ptr<Node> node) {
  if (node->GetNodeType() == LOOKUP) {
    dynamic_pointer_cast<NodeLookup>(node)->where_lookuped->output_references.erase(node);
  }
  if (node->GetNodeType() == XRP_APPLICATION) {
    if (dynamic_pointer_cast<NodeXRPApplication>(node)->xrp->xrp->IsRandomChoice() == true) {
      random_choices.erase(dynamic_pointer_cast<NodeXRPApplication>(node->shared_from_this()));
    }
  }
  node->DeleteNode();
}

void DeleteBranch(shared_ptr<Node> first_node) {
  ApplyToMeAndAllMyChildren(first_node, DeleteNode);
}

void MakeMHProposal() {
  size_t number_of_random_choices = random_choices.size();
  if (number_of_random_choices == 0) {
    return; // There is no random choices in the trace.
  }

  set< shared_ptr<NodeXRPApplication> >::iterator iterator = random_choices.begin();
  int random_choice_id = UniformDiscrete(0, number_of_random_choices - 1);
  std::advance(iterator, random_choice_id);

  shared_ptr<NodeXRPApplication> // FIXME: Should be NodeEvaluation?
    random_choice = *iterator;

  priority_queue<ReevaluationEntry,
                 vector< ReevaluationEntry >,
                 ReevaluationOrderComparer> reevaluation_queue;
  reevaluation_queue.push(ReevaluationEntry(random_choice,
                                            shared_ptr<NodeEvaluation>(),
                                            shared_ptr<VentureValue>()));

  stack<OmitPattern> omit_patterns;

  queue< shared_ptr<Node> > touched_nodes;
  
  ReevaluationParameters reevaluation_parameters;

  while (reevaluation_queue.size() != 0) {
    ReevaluationEntry current_reevaluation = reevaluation_queue.top();
    reevaluation_queue.pop();
    if (!omit_patterns.empty()) {
      if (VerifyOrderPattern(omit_patterns.top().omit_pattern, current_reevaluation.reevaluation_node->myorder)) {
        cout << "Omitting" << endl;
        continue;
      }
      if (omit_patterns.top().stop_pattern == current_reevaluation.reevaluation_node->myorder) {
        omit_patterns.pop();
      }
    }
    touched_nodes.push(current_reevaluation.reevaluation_node);
    shared_ptr<ReevaluationResult> reevaluation_result =
      current_reevaluation.reevaluation_node->Reevaluate(current_reevaluation.passing_value,
                                                         current_reevaluation.caller,
                                                         reevaluation_parameters);
    
    if (reevaluation_result->pass_further == true) {
      for (set< shared_ptr<Node> >::iterator iterator = current_reevaluation.reevaluation_node->output_references.begin();
           iterator != current_reevaluation.reevaluation_node->output_references.end();
           iterator++)
      {
        if ((*iterator)->GetNodeType() == VARIABLE) {
          touched_nodes.push(*iterator);
          assert(reevaluation_result->passing_value != shared_ptr<VentureValue>());
          dynamic_pointer_cast<NodeVariable>(*iterator)->new_value = reevaluation_result->passing_value;
          for (set< shared_ptr<Node> >::iterator variable_iterator =
                 dynamic_pointer_cast<NodeVariable>(*iterator)->output_references.begin();
               variable_iterator != dynamic_pointer_cast<NodeVariable>(*iterator)->output_references.end();
               variable_iterator++)
          {
            assert((*variable_iterator)->GetNodeType() != VARIABLE);
            reevaluation_queue.push(ReevaluationEntry(dynamic_pointer_cast<NodeEvaluation>(*variable_iterator),
                                                      current_reevaluation.caller, // Or just NULL?
                                                      reevaluation_result->passing_value)); // Or also just NULL?
          }
        } else {
          if (current_reevaluation.reevaluation_node->parent != shared_ptr<NodeEvaluation>()) {
            if (current_reevaluation.reevaluation_node->parent->GetNodeType() == APPLICATION_CALLER &&
                  dynamic_pointer_cast<NodeApplicationCaller>(current_reevaluation.reevaluation_node->parent)->application_operator ==
                    current_reevaluation.reevaluation_node) {
              if (CompareValue(reevaluation_result->passing_value,
                               dynamic_pointer_cast<NodeApplicationCaller>(
                                 current_reevaluation.reevaluation_node->parent)->saved_evaluated_operator)) {
                continue; // The operator is the same.
              }
              omit_patterns.push(OmitPattern(dynamic_pointer_cast<NodeApplicationCaller>(
                                               current_reevaluation.reevaluation_node->parent)->application_node->myorder,
                                             current_reevaluation.reevaluation_node->parent->myorder));
            }
          }
          assert(current_reevaluation.reevaluation_node != shared_ptr<NodeEvaluation>());
          reevaluation_queue.push(ReevaluationEntry(dynamic_pointer_cast<NodeEvaluation>(*iterator),
                                                    current_reevaluation.reevaluation_node,
                                                    reevaluation_result->passing_value));

        }
      }
    }
  }
  if (omit_patterns.size() > 0) {
    throw std::exception("omit_patterns.size() > 0");
  }

  size_t posterior_number_of_random_choices =
    number_of_random_choices + reevaluation_parameters.random_choices_delta;
  real number_of_random_choices_formula_component =
    log((1.0 / posterior_number_of_random_choices)
          / (1.0 / number_of_random_choices));
  real to_compare = number_of_random_choices_formula_component + reevaluation_parameters.loglikelihood_changes;
  real random_value = log(gsl_ran_flat(random_generator, 0, 1));
  //cout << random_value << " vs " << to_compare << " " << number_of_random_choices_formula_component <<
  //  " " << reevaluation_parameters.loglikelihood_changes << endl;
  MHDecision mh_decision;
  if (random_value < to_compare) {
    mh_decision = MH_APPROVED;
    //cout << "Approved" << endl;
  } else {
    mh_decision = MH_DECLINED;
    //cout << "Decline" << endl;
  }

  while (!touched_nodes.empty()) {
    shared_ptr<Node> current_node = touched_nodes.front();
    touched_nodes.pop();
    if (current_node->GetNodeType() == VARIABLE) {
      if (mh_decision == MH_APPROVED) {
        if (dynamic_pointer_cast<NodeVariable>(current_node)->new_value != shared_ptr<VentureValue>()) {
          assert(dynamic_pointer_cast<NodeVariable>(current_node)->new_value != shared_ptr<VentureValue>());
          dynamic_pointer_cast<NodeVariable>(current_node)->value = dynamic_pointer_cast<NodeVariable>(current_node)->new_value;
        }
      }
      dynamic_pointer_cast<NodeVariable>(current_node)->new_value = shared_ptr<VentureValue>();
    } else if (current_node->GetNodeType() == APPLICATION_CALLER &&
                 dynamic_pointer_cast<NodeApplicationCaller>(current_node)->new_application_node != shared_ptr<NodeEvaluation>())
    {
      if (mh_decision == MH_APPROVED) {
        dynamic_pointer_cast<NodeApplicationCaller>(current_node)->application_node->environment->DeleteNode();
        DeleteBranch(dynamic_pointer_cast<NodeApplicationCaller>(current_node)->application_node);
        //cout << "Deleting" << endl;
        //cout << dynamic_pointer_cast<NodeApplicationCaller>(current_node)->application_node->environment.use_count() << endl;
        //cout << endl;
      } else {
        dynamic_pointer_cast<NodeApplicationCaller>(current_node)->new_application_node->environment->DeleteNode();
        DeleteBranch(dynamic_pointer_cast<NodeApplicationCaller>(current_node)->new_application_node);
      }
      
      if (mh_decision == MH_APPROVED) {
        dynamic_pointer_cast<NodeApplicationCaller>(current_node)->application_node =
          dynamic_pointer_cast<NodeApplicationCaller>(current_node)->new_application_node;
        dynamic_pointer_cast<NodeApplicationCaller>(current_node)->saved_evaluated_operator =
          dynamic_pointer_cast<NodeApplicationCaller>(current_node)->proposing_evaluated_operator;
      }
      dynamic_pointer_cast<NodeApplicationCaller>(current_node)->new_application_node = shared_ptr<NodeEvaluation>();
      dynamic_pointer_cast<NodeApplicationCaller>(current_node)->proposing_evaluated_operator = shared_ptr<VentureValue>();
    } else if (current_node->GetNodeType() == XRP_APPLICATION) {
      // If it was resampled:
      if (dynamic_pointer_cast<NodeXRPApplication>(current_node)->new_sampled_value != shared_ptr<VentureValue>()) {
        // Should be implemented via the Remove and Incorporate!
        if (mh_decision == MH_APPROVED) {
          assert(dynamic_pointer_cast<NodeXRPApplication>(current_node)->new_sampled_value != shared_ptr<VentureValue>());
          dynamic_pointer_cast<NodeXRPApplication>(current_node)->sampled_value =
            dynamic_pointer_cast<NodeXRPApplication>(current_node)->new_sampled_value;
        }
        dynamic_pointer_cast<NodeXRPApplication>(current_node)->new_sampled_value = shared_ptr<VentureValue>();
      }
    } else if (current_node->GetNodeType() == DIRECTIVE_ASSUME ||
                current_node->GetNodeType() == DIRECTIVE_PREDICT ||
                current_node->GetNodeType() == DIRECTIVE_OBSERVE ||
                current_node->GetNodeType() == APPLICATION_CALLER ||
                current_node->GetNodeType() == LOOKUP) {
      // Nothing.
    } else {
      cout << current_node->GetNodeType() << endl;
      throw std::exception("Unexpected node type.");
    }
  }
  
  /*
  if (mh_decision == MH_APPROVED) {
    cout << "Approved" << endl << endl << endl;
  } else {
    cout << "Decline" << endl << endl << endl;
  }
  */
}
