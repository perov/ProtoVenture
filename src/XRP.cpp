
#include "Header.h"
#include "VentureValues.h"
#include "VentureParser.h"
#include "Analyzer.h"
#include "XRP.h"

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
          dynamic_pointer_cast<NodeVariable>(*iterator)->new_value = reevaluation_result->passing_value;
          for (set< shared_ptr<Node> >::iterator variable_iterator =
                 dynamic_pointer_cast<NodeVariable>(*iterator)->output_references.begin();
               variable_iterator != dynamic_pointer_cast<NodeVariable>(*iterator)->output_references.end();
               variable_iterator++)
          {
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
        dynamic_pointer_cast<NodeVariable>(current_node)->value = dynamic_pointer_cast<NodeVariable>(current_node)->new_value;
      }
      dynamic_pointer_cast<NodeVariable>(current_node)->new_value = shared_ptr<VentureValue>();
    } else if (current_node->GetNodeType() == APPLICATION_CALLER &&
                 dynamic_pointer_cast<NodeApplicationCaller>(current_node)->new_application_node != shared_ptr<NodeEvaluation>())
    {
      if (mh_decision == MH_APPROVED) {
        DeleteBranch(dynamic_pointer_cast<NodeApplicationCaller>(current_node)->application_node);
      } else {
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
