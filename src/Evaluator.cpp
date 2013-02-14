
#include "HeaderPre.h"
#include "Header.h"
#include "VentureValues.h"
#include "Analyzer.h"
#include "Evaluator.h"
#include "XRPmem.h"

shared_ptr<VentureValue> Evaluator(shared_ptr<NodeEvaluation> evaluation_node,
                                   shared_ptr<NodeEnvironment> environment,
                                   shared_ptr<Node> output_reference_target,
                                   shared_ptr<NodeEvaluation> caller,
                                   EvaluationConfig& evaluation_config,
                                   string request_postfix) {
  if (caller == shared_ptr<NodeEvaluation>()) { // It is directive.
    DIRECTIVE_COUNTER++;
    evaluation_node->node_key = boost::lexical_cast<string>(DIRECTIVE_COUNTER);
    evaluation_node->myorder.push_back(DIRECTIVE_COUNTER);
  } else {
    if (caller->GetNodeType() == DIRECTIVE_ASSUME ||
          caller->GetNodeType() == DIRECTIVE_PREDICT ||
          caller->GetNodeType() == DIRECTIVE_OBSERVE) {
      evaluation_node->node_key = caller->node_key + "|from_dir";
    } else if (caller->GetNodeType() == APPLICATION_CALLER) {
      shared_ptr<NodeApplicationCaller> parent_node = dynamic_pointer_cast<NodeApplicationCaller>(caller);
      if (parent_node->application_operator == evaluation_node) {
        evaluation_node->node_key = caller->node_key + "|op";
      } else if (request_postfix.substr(0, 5) == "args_") {
        evaluation_node->node_key = caller->node_key + "|" + request_postfix;
      } else if (parent_node->application_node == evaluation_node || parent_node->new_application_node == evaluation_node) {
        evaluation_node->node_key = caller->node_key + "|ap";
      } else {
        throw std::runtime_error("Unexpected situation for the 'node_key' (1).");
      }
    } else if (caller->GetNodeType() == XRP_APPLICATION &&
                 dynamic_pointer_cast<NodeXRPApplication>(caller)->xrp->xrp->GetName() == "XRP__memoizer") {
      assert(request_postfix != "");
      evaluation_node->node_key = caller->node_key + "|m:" + request_postfix;
    } else {
      throw std::runtime_error("Unexpected situation for the 'node_key' (2).");
    }

    evaluation_node->myorder = caller->myorder;
    if (caller->GetNodeType() == XRP_APPLICATION &&
          dynamic_pointer_cast<NodeXRPApplication>(caller)->xrp->xrp->GetName() == "XRP__memoizer" && 1 == 2)
    {
      size_t& my_last_evaluation_id =
        dynamic_pointer_cast<XRP__memoized_procedure>(dynamic_pointer_cast<VentureXRP>(
          dynamic_pointer_cast<NodeXRPApplication>(caller)->my_sampled_value)->xrp)->my_last_evaluation_id;
      // FIXME: lock things for multithread version?
      evaluation_node->myorder.push_back(std::numeric_limits<size_t>::max());
    } else {
      caller->last_child_order++;
      evaluation_node->myorder.push_back(caller->last_child_order);
    }
    evaluation_node->parent = caller;
  }

  evaluation_node->environment = environment;
  if (output_reference_target != shared_ptr<Node>()) {
    evaluation_node->output_references.insert(output_reference_target);
  }

  if (evaluation_node->earlier_evaluation_nodes != shared_ptr<NodeEvaluation>()) {
    // Potential recursion problem (i.e. stack overflow).
    // FIXME: in the future implement via the loop.
    Evaluator(evaluation_node->earlier_evaluation_nodes,
              environment,
              shared_ptr<Node>(),
              dynamic_pointer_cast<NodeEvaluation>(evaluation_node->shared_from_this()),
              evaluation_config,
              "..."); // FIXME: not supported!
  }

  assert(evaluation_node->evaluated == false);
  evaluation_node->evaluated = true; // Not too early?
  return evaluation_node->Evaluate(environment, evaluation_config);
}

shared_ptr<Node> BindToEnvironment(shared_ptr<NodeEnvironment> target_environment,
                                   shared_ptr<VentureSymbol> variable_name,
                                   shared_ptr<VentureValue> binding_value,
                                   shared_ptr<NodeEvaluation> binding_node) {
  if (target_environment->variables.count(variable_name->GetString()) != 0) {
    throw std::runtime_error(("Binding variable, which has been already bound: " + variable_name->GetString()).c_str());
  }
  target_environment->variables[variable_name->GetString()] =
    shared_ptr<NodeVariable>(new NodeVariable(target_environment, binding_value, binding_node));
  target_environment->variables[variable_name->GetString()]->weak_ptr_to_me = dynamic_pointer_cast<Node>(target_environment->variables[variable_name->GetString()]->shared_from_this()); // Silly.
  return target_environment->variables[variable_name->GetString()];
}

// Should be implemented with the function above?
void BindVariableToEnvironment(shared_ptr<NodeEnvironment> target_environment,
                               shared_ptr<VentureSymbol> variable_name,
                               shared_ptr<NodeVariable> binding_variable) {
  if (target_environment->variables.count(variable_name->GetString()) != 0) {
    throw std::runtime_error(("Binding variable, which has been already bound: " + variable_name->GetString()).c_str());
  }
  target_environment->variables[variable_name->GetString()] =
    binding_variable;
}

shared_ptr<VentureValue> LookupValue(shared_ptr<NodeEnvironment> environment,
                                     shared_ptr<VentureSymbol> variable_name,
                                     shared_ptr<NodeEvaluation> lookuper,
                                     bool old_values) { // = false, see the header
  shared_ptr<NodeEnvironment> inspecting_environment = environment;
  while (inspecting_environment != shared_ptr<NodeEnvironment>()) {
    if (inspecting_environment->variables.count(variable_name->GetString()) == 1) {
      if (lookuper != shared_ptr<NodeEvaluation>()) {
        if (lookuper->GetNodeType() == LOOKUP) {
          dynamic_pointer_cast<NodeLookup>(lookuper)->where_lookuped =
            inspecting_environment->variables[variable_name->GetString()];
        }
        inspecting_environment->variables[variable_name->GetString()]->output_references.insert(lookuper);
      }
      if (inspecting_environment->variables[variable_name->GetString()]->new_value == shared_ptr<VentureValue>() ||
          old_values == true) {
        // cout << ">>> Returning the old value " << inspecting_environment->variables[variable_name->GetString()]->value->GetString() << endl;
        return inspecting_environment->variables[variable_name->GetString()]->value;
      } else {
        // cout << ">>> Returning the new value " << inspecting_environment->variables[variable_name->GetString()]->new_value->GetString() << endl;
        return inspecting_environment->variables[variable_name->GetString()]->new_value;
      }
    } else {
      inspecting_environment = inspecting_environment->parent_environment.lock();
    }
  }
  //cout << "Error: " << variable_name->GetString() << endl;
  throw std::runtime_error((string("Unbound variable: ") + variable_name->GetString()).c_str());
}

shared_ptr<VentureValue> LookupValue(shared_ptr<NodeEnvironment> environment,
                                     size_t index,
                                     shared_ptr<NodeEvaluation> lookuper,
                                     bool old_values) { // = false, see the header
  if (index >= environment->local_variables.size()) {
    throw std::runtime_error("LookupValue: out of bound.");
  }
  if (lookuper != shared_ptr<NodeEvaluation>()) {
    if (lookuper->GetNodeType() == LOOKUP) {
      dynamic_pointer_cast<NodeLookup>(lookuper)->where_lookuped =
        environment->local_variables[index];
    }
    environment->local_variables[index]->output_references.insert(lookuper);
  }
  if (environment->local_variables[index]->new_value == shared_ptr<VentureValue>() ||
      old_values == true) {
    // cout << ">>> Returning the old value " << environment->local_variables[index]->value << endl;
    return environment->local_variables[index]->value;
  } else {
    // cout << ">>> Returning the new value " << environment->local_variables[index]->new_value << endl;
    return environment->local_variables[index]->new_value;
  }
}

shared_ptr<NodeEvaluation> FindConstrainingNode(shared_ptr<Node> node) {
  while (true) {
    assert(node != shared_ptr<Node>());
    if (node->GetNodeType() == LOOKUP) {
      node = dynamic_pointer_cast<NodeLookup>(node)->where_lookuped.lock();
      continue;
    } else if (node->GetNodeType() == VARIABLE) {
      node = dynamic_pointer_cast<NodeVariable>(node)->binding_node.lock();
      continue;
    } else if (node->GetNodeType() == APPLICATION_CALLER) {
      if (dynamic_pointer_cast<NodeApplicationCaller>(node)->new_application_node != shared_ptr<NodeEvaluation>()) {
        node = dynamic_pointer_cast<NodeApplicationCaller>(node)->new_application_node;
      } else {
        node = dynamic_pointer_cast<NodeApplicationCaller>(node)->application_node;
      }
      continue;
    } else if (node->GetNodeType() == SELF_EVALUATING) {
      return dynamic_pointer_cast<NodeEvaluation>(node);
    } else if (node->GetNodeType() == XRP_APPLICATION) {
      if (dynamic_pointer_cast<NodeXRPApplication>(node)->xrp->xrp->GetName() == "XRP__memoized_procedure") {
        vector< shared_ptr<VentureValue> > got_arguments = GetArgumentsFromEnvironment(dynamic_pointer_cast<NodeXRPApplication>(node)->environment, // Not efficient?
                                        dynamic_pointer_cast<NodeEvaluation>(node),
                                        false);
        string mem_table_key = XRP__memoized_procedure__MakeMapKeyFromArguments(got_arguments);
        if (dynamic_pointer_cast<XRP__memoized_procedure>(dynamic_pointer_cast<NodeXRPApplication>(node)->xrp->xrp)->mem_table.count(mem_table_key) == 0) {
          throw std::runtime_error("Cannot find the necessary key in the mem table.");
        }
        XRP__memoizer_map_element& mem_table_element =
          (*(dynamic_pointer_cast<XRP__memoized_procedure>(dynamic_pointer_cast<NodeXRPApplication>(node)->xrp->xrp)->mem_table.find(mem_table_key))).second;
        return FindConstrainingNode(mem_table_element.application_caller_node);
      } else {
        return dynamic_pointer_cast<NodeEvaluation>(node);
      }
    } else if (node->GetNodeType() == LAMBDA_CREATOR) {
      return shared_ptr<NodeEvaluation>();
    } else {
      throw std::runtime_error("Cannot find the node for potential constraining. Unexpected node type.");
    }
  }
}

// If possible to force (return True) or not (return False)?
pair<bool, shared_ptr<NodeEvaluation> > ConstrainExpressionValue(shared_ptr<Node> node, shared_ptr<VentureValue> desired_value, shared_ptr<ReevaluationParameters> reevaluation_parameters) {
  while (true) {
    assert(node != shared_ptr<Node>());
    if (node->GetNodeType() == LOOKUP) {
      node = dynamic_pointer_cast<NodeLookup>(node)->where_lookuped.lock();
      continue;
    } else if (node->GetNodeType() == VARIABLE) {
      node = dynamic_pointer_cast<NodeVariable>(node)->binding_node.lock();
      continue;
    } else if (node->GetNodeType() == APPLICATION_CALLER) {
      if (dynamic_pointer_cast<NodeApplicationCaller>(node)->new_application_node != shared_ptr<NodeEvaluation>()) {
        node = dynamic_pointer_cast<NodeApplicationCaller>(node)->new_application_node;
      } else {
        node = dynamic_pointer_cast<NodeApplicationCaller>(node)->application_node;
      }
      continue;
    } else if (node->GetNodeType() == SELF_EVALUATING) {
      if (CompareValue(dynamic_pointer_cast<NodeSelfEvaluating>(node)->value, desired_value)) {
        return pair<bool, shared_ptr<NodeEvaluation> >(true, shared_ptr<NodeEvaluation>());
      } else {
        return pair<bool, shared_ptr<NodeEvaluation> >(false, shared_ptr<NodeEvaluation>());
      }
    } else if (node->GetNodeType() == XRP_APPLICATION) {
      shared_ptr<NodeXRPApplication> node2 = dynamic_pointer_cast<NodeXRPApplication>(node);
      DeleteRandomChoices(node2); // FIXME: it is not right, if we are in MH.
      if (CompareValue(node2->my_sampled_value, desired_value)) {
        //if (node2->forced_by_observations == true) {
        //  throw std::runtime_error("Sorry, still does not support forcing the same value from several observations.");
        //}
        node2->forced_by_observations = true;
        return pair<bool, shared_ptr<NodeEvaluation> >(true, shared_ptr<NodeEvaluation>());
      } else {
        if (node2->forced_by_observations == true) {
          return pair<bool, shared_ptr<NodeEvaluation> >(false, shared_ptr<NodeEvaluation>()); // The already forced value is not the value we want. Rejecting.
        } else {
          node2->forced_by_observations = true;
          vector< shared_ptr<VentureValue> > got_arguments = GetArgumentsFromEnvironment(node2->environment, // Not efficient?
                                          dynamic_pointer_cast<NodeEvaluation>(node2),
                                          false);
          pair<bool, shared_ptr<NodeEvaluation> > internal_forcing = node2->xrp->xrp->ForceValue(got_arguments, desired_value, reevaluation_parameters, node2);
          if (internal_forcing.first == false) { return pair<bool, shared_ptr<NodeEvaluation> >(false, shared_ptr<NodeEvaluation>()); }
          node2->xrp->xrp->Remove(got_arguments, node2->my_sampled_value);
          //real logscore_change = -1.0 * node2->xrp->xrp->GetSampledLoglikelihood(got_arguments, node2->my_sampled_value);
          // Assuming that if it was rescored, it saved the necessary value, i.e. there would not be necessity in
          // the forcing. Otherwise:
          reevaluation_parameters->__log_q_from_old_to_new -= node2->xrp->xrp->GetSampledLoglikelihood(got_arguments, node2->my_sampled_value);
          node2->my_sampled_value = desired_value;
          //logscore_change +=node2->xrp->xrp->GetSampledLoglikelihood(got_arguments, node2->my_sampled_value);
          node2->xrp->xrp->Incorporate(got_arguments, node2->my_sampled_value);
          return internal_forcing;
        }
      }
    } else {
      throw std::runtime_error("Cannot force the observation value. Unexpected node type.");
    }
  }
}

shared_ptr<VentureValue> UnconstrainNode(shared_ptr<NodeEvaluation> node) {
  shared_ptr<NodeEvaluation> potentially_constraint_node = FindConstrainingNode(this->application_node);
  if (potentially_constraint_node == shared_ptr<NodeEvaluation>()) {
    // The branch was not constraint.
    return shared_ptr<VentureValue>();
  }
  if (potentially_constraint_node->constraint_times > 0) {
    potentially_constraint_node->constraint_times--;
    if (potentially_constraint_node->constraint_times == 0 &&
          potentially_constraint_node->GetNodeType() == XRP_APPLICATION &&
          dynamic_pointer_cast<NodeXRPApplication>(potentially_constraint_node)->xrp->xrp->IsRandomChoice()) {
      AddToRandomChoices(dynamic_pointer_cast<NodeXRPApplication>(potentially_constraint_node));
    }
    if (potentially_constraint_node->GetNodeType() == SELF_EVALUATING) {
      return dynamic_pointer_cast<NodeSelfEvaluating>(potentially_constraint_node)->value;
    } else if (potentially_constraint_node->GetNodeType() == XRP_APPLICATION) {
      return dynamic_pointer_cast<NodeXRPApplication>(potentially_constraint_node)->my_sampled_value;
    } else {
      throw std::runtime_error("Strange 'potentially_constraint_node->GetNodeType()'.");
    }
  } else {
    // The branch was not constraint.
    return shared_ptr<VentureValue>();
  }
}

weak_ptr<NodeEvaluation> UnforceExpressionValue(shared_ptr<Node> node) {
  while (true) {
    assert(node != shared_ptr<Node>());
    if (node->GetNodeType() == LOOKUP) {
      node = dynamic_pointer_cast<NodeLookup>(node)->where_lookuped.lock();
      continue;
    } else if (node->GetNodeType() == VARIABLE) {
      node = dynamic_pointer_cast<NodeVariable>(node)->binding_node.lock();
      continue;
    } else if (node->GetNodeType() == APPLICATION_CALLER) {
      if (dynamic_pointer_cast<NodeApplicationCaller>(node)->new_application_node != shared_ptr<NodeEvaluation>()) {
        node = dynamic_pointer_cast<NodeApplicationCaller>(node)->new_application_node;
      } else {
        node = dynamic_pointer_cast<NodeApplicationCaller>(node)->application_node;
      }
      continue;
    } else if (node->GetNodeType() == SELF_EVALUATING) {
      return shared_ptr<NodeEvaluation>();
    } else if (node->GetNodeType() == XRP_APPLICATION) {
      shared_ptr<NodeXRPApplication> node2 = dynamic_pointer_cast<NodeXRPApplication>(node);
      assert(node2->forced_by_observations == true);
      node2->forced_by_observations = false;
      AddToRandomChoices(node2);
      vector< shared_ptr<VentureValue> > got_arguments = GetArgumentsFromEnvironment(node2->environment, // Not efficient?
                                      dynamic_pointer_cast<NodeEvaluation>(node2),
                                      false);
      return node2->xrp->xrp->UnforceValue(got_arguments, node2);
    } else {
      throw std::runtime_error("Cannot unforce the observation value. Unexpected node type.");
    }
  }
}

bool PropagateForObserve(shared_ptr<NodeEvaluation> observe_directive_node, shared_ptr<NodeXRPApplication> xrp_node_to_go_from) {
  ReevaluationEntry current_reevaluation(observe_directive_node, shared_ptr<NodeEvaluation>(), shared_ptr<VentureValue>(), REEVALUATION_PRIORITY__STANDARD);
  set<ReevaluationEntry,
      ReevaluationOrderComparer> reevaluation_queue;
  stack< shared_ptr<Node> > touched_nodes;
  vector< shared_ptr<Node> > touched_nodes2;
  ProposalInfo this_proposal; // FIXME: define?
  shared_ptr<ReevaluationResult> reevaluation_result =
    shared_ptr<ReevaluationResult>(
      new ReevaluationResult(xrp_node_to_go_from, true));
  shared_ptr<ReevaluationParameters> reevaluation_parameters =
    shared_ptr<ReevaluationParameters>(new ReevaluationParameters(dynamic_pointer_cast<NodeXRPApplication>(xrp_node_to_go_from)));
  reevaluation_parameters->we_are_in_enumeration = false;
  stack<OmitPattern> omit_patterns;
  AddToReevaluationQueue(current_reevaluation, reevaluation_queue, touched_nodes, touched_nodes2, this_proposal, reevaluation_result, reevaluation_parameters, omit_patterns);
  PropagateNewValue(reevaluation_queue, touched_nodes, touched_nodes2, this_proposal, reevaluation_parameters);
  set< shared_ptr<NodeXRPApplication> > creating_random_choices;
  set< shared_ptr<NodeXRPApplication> > deleting_random_choices;
  GetCreatingRandomChoices(touched_nodes2, creating_random_choices);
  GetDeletingRandomChoices(touched_nodes2, deleting_random_choices);
  FinalizeProposal(touched_nodes, MH_APPROVED, deleting_random_choices, creating_random_choices);
  return reevaluation_parameters->__unsatisfied_constraint;
}

shared_ptr<VentureValue> GetBranchValue(shared_ptr<Node> node) {
  while (true) {
    assert(node != shared_ptr<Node>());
    if (node->GetNodeType() == LOOKUP) {
      node = dynamic_pointer_cast<NodeLookup>(node)->where_lookuped.lock();
      continue;
    } else if (node->GetNodeType() == VARIABLE) {
      return dynamic_pointer_cast<NodeVariable>(node)->GetCurrentValue();
      continue;
    } else if (node->GetNodeType() == APPLICATION_CALLER) {
      if (dynamic_pointer_cast<NodeApplicationCaller>(node)->new_application_node != shared_ptr<NodeEvaluation>()) {
        node = dynamic_pointer_cast<NodeApplicationCaller>(node)->new_application_node;
      } else {
        node = dynamic_pointer_cast<NodeApplicationCaller>(node)->application_node;
      }
      continue;
    } else if (node->GetNodeType() == SELF_EVALUATING) {
      return dynamic_pointer_cast<NodeSelfEvaluating>(node)->value;
    } else if (node->GetNodeType() == XRP_APPLICATION) {
      return dynamic_pointer_cast<NodeXRPApplication>(node)->my_sampled_value;
    } else if (node->GetNodeType() == LAMBDA_CREATOR) {
      return dynamic_pointer_cast<NodeLambdaCreator>(node)->returned_value;
    } else {
      throw std::runtime_error("Cannot get the branch value. Unexpected node type.");
    }
  }
}

//void PropagateByDirective(shared_ptr<>) {

//}
