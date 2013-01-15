
#include "Header.h"
#include "VentureValues.h"
#include "VentureParser.h"
#include "Analyzer.h"
#include "XRPCore.h"
#include "MHProposal.h"
#include "RIPL.h"

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
    // See the notice "BAD-POINTER" in Analyzer.cpp
    if (dynamic_pointer_cast<NodeLookup>(node)->where_lookuped.lock() != shared_ptr<NodeVariable>()) {
      // dynamic_pointer_cast<NodeLookup>(node)->where_lookuped.lock()->output_references.erase(node);
    }
  }
  if (node->GetNodeType() == XRP_APPLICATION) {
    // See the notice "BAD-POINTER" in Analyzer.cpp
    if (dynamic_pointer_cast<NodeXRPApplication>(node)->xrp != shared_ptr<VentureXRP>() &&
          dynamic_pointer_cast<NodeXRPApplication>(node)->xrp->xrp != shared_ptr<XRP>()) {

      // FIXME: GetArgumentsFromEnvironment should be called without adding lookup references!
      vector< shared_ptr<VentureValue> > old_arguments = GetArgumentsFromEnvironment(dynamic_pointer_cast<NodeXRPApplication>(node)->environment, // Not efficient?
                                      dynamic_pointer_cast<NodeEvaluation>(node), // Are we sure that we have not deleted yet lookup links?
                                      true); // FIXME: we should be sure that we are receiving old arguments!

      dynamic_pointer_cast<NodeXRPApplication>(node)->xrp->xrp->
        Remove(old_arguments, dynamic_pointer_cast<NodeXRPApplication>(node)->my_sampled_value);

      dynamic_pointer_cast<NodeXRPApplication>(node)->xrp->xrp->Unsampler(old_arguments, dynamic_pointer_cast<NodeXRPApplication>(node));

      if (dynamic_pointer_cast<NodeXRPApplication>(node)->xrp->xrp->IsRandomChoice() == true) {
        random_choices.erase(dynamic_pointer_cast<NodeXRPApplication>(node->shared_from_this()));
      }
    }
  }
}

void DeleteBranch(shared_ptr<Node> first_node) {
  ApplyToMeAndAllMyChildren(first_node, DeleteNode);
}

VentureException__ForcedMHDecline::VentureException__ForcedMHDecline()
  : std::runtime_error("VentureException__ForcedMHDecline")
{

}

void TouchNode(shared_ptr<Node> node, stack< shared_ptr<Node> >& touched_nodes, vector< shared_ptr<Node> >& touched_nodes2, ProposalInfo& proposal_info) {
  /*while (true) {
    boost::lock_guard<boost::mutex> locking_guard(node->occupying_mutex);
    if (node->occupying_proposal_info == shared_ptr<ProposalInfo>()) {
      break;
    } else if (node->occupying_proposal_info == proposal_info.shared_from_this()) {
      throw std::runtime_error("Attempt to occupy the same node twice!");
    } else {
      assert(node->occupying_proposal_info->proposal_unique_id != proposal_info.proposal_unique_id);
      if (node->occupying_proposal_info->proposal_unique_id > proposal_info.proposal_unique_id) {
        node->occupying_proposal_info->request_to_terminate = true;
        continue;
      } else {
        throw VentureException__ForcedMHDecline();
      }
    }
  }*/
  touched_nodes.push(node);
  touched_nodes2.push_back(node);
}

void MakeMHProposal(int proposal_unique_id) {
  cout << "New MH" << endl;

  ProposalInfo this_proposal;
  this_proposal.proposal_unique_id = proposal_unique_id; // FIXME: through constructor
  this_proposal.request_to_terminate = false; // FIXME: through constructor
  
  size_t number_of_random_choices = random_choices.size();
  if (number_of_random_choices == 0) {
    return; // There is no random choices in the trace.
  }

  set< shared_ptr<NodeXRPApplication> >::iterator iterator = random_choices.begin();
  int random_choice_id = UniformDiscrete(0, number_of_random_choices - 1);
  std::advance(iterator, random_choice_id);

  shared_ptr<NodeXRPApplication> // FIXME: Should be NodeEvaluation?
    random_choice = *iterator;

  ReevaluationParameters reevaluation_parameters(random_choice);

  set<ReevaluationEntry,
      ReevaluationOrderComparer> reevaluation_queue;
  reevaluation_queue.insert(ReevaluationEntry(random_choice,
                                            shared_ptr<NodeEvaluation>(),
                                            shared_ptr<VentureValue>(),
                                            REEVALUATION_PRIORITY__STANDARD));

  stack<OmitPattern> omit_patterns;

  stack< shared_ptr<Node> > touched_nodes; // Should be LIFO for Unsampler(...)!
                                           // (To correctly receive old arguments.)
  vector< shared_ptr<Node> > touched_nodes2; // FIXME: get rid of the touched_nodes2?

  while (reevaluation_queue.size() != 0) {
    ReevaluationEntry current_reevaluation = *(reevaluation_queue.rbegin());
    {
      set<ReevaluationEntry,
          ReevaluationOrderComparer>::iterator iterator_to_last_element = reevaluation_queue.end();
      --iterator_to_last_element;
      reevaluation_queue.erase(iterator_to_last_element);
    }
    if (!omit_patterns.empty()) {
      if (VerifyOrderPattern(omit_patterns.top().omit_pattern, current_reevaluation.reevaluation_node->myorder)) {
        omit_patterns.pop();
        continue;
      }
      if (omit_patterns.top().stop_pattern == current_reevaluation.reevaluation_node->myorder) {
        omit_patterns.pop();
      }
    }
#ifdef _MSC_VER // This IF should be removed. It is here only because the GetQueueContainer returns not the deque in Unix?
#ifndef NDEBUG
    std::deque< shared_ptr<Node> >::const_iterator already_existent_element =
      std::find(GetStackContainer(touched_nodes).begin(), GetStackContainer(touched_nodes).end(), current_reevaluation.reevaluation_node);
    if (!(already_existent_element == GetStackContainer(touched_nodes).end())) {
      int distance = std::distance(GetStackContainer(touched_nodes).begin(), already_existent_element);
      cout << "Pam: " << distance << endl;
      DrawGraphDuringMH(GetLastDirectiveNode(), touched_nodes);
    }
#endif
#endif
#ifdef _MSC_VER // This IF should be removed. It is here only because the GetQueueContainer returns not the deque in Unix?
    assert(std::find(GetStackContainer(touched_nodes).begin(), GetStackContainer(touched_nodes).end(), current_reevaluation.reevaluation_node)
      == GetStackContainer(touched_nodes).end());
#endif
    TouchNode(current_reevaluation.reevaluation_node, touched_nodes, touched_nodes2, this_proposal);
    
    shared_ptr<ReevaluationResult> reevaluation_result;
    if (current_reevaluation.reevaluation_node->GetNodeType() == XRP_APPLICATION &&
        dynamic_pointer_cast<NodeXRPApplication>(current_reevaluation.reevaluation_node)->xrp->xrp->GetName() == "XRP__memoized_procedure" &&
        current_reevaluation.priority == REEVALUATION_PRIORITY__STANDARD) {
      // If it is memoized procedure, and it has received update from the memoizer node.
      reevaluation_result = shared_ptr<ReevaluationResult>(new ReevaluationResult(current_reevaluation.passing_value, true));
      // FIXME: make here also necessary blocking.
    } else {
      reevaluation_result =
        current_reevaluation.reevaluation_node->Reevaluate(current_reevaluation.passing_value,
                                                           current_reevaluation.caller,
                                                           reevaluation_parameters);
    }
    
    if (reevaluation_result->pass_further == true) {
      // If arguments have changed, delete the potentially existing reevaluation result
      // from the memoizer node.
      if (current_reevaluation.reevaluation_node->GetNodeType() == XRP_APPLICATION) {
        if (dynamic_pointer_cast<NodeXRPApplication>(current_reevaluation.reevaluation_node)->xrp->xrp->GetName() ==
              "XRP__memoized_procedure") {
          reevaluation_queue.erase(ReevaluationEntry(dynamic_pointer_cast<NodeEvaluation>(current_reevaluation.reevaluation_node),
                                                     shared_ptr<NodeEvaluation>(),
                                                     shared_ptr<VentureValue>(),
                                                     REEVALUATION_PRIORITY__STANDARD));
        }
      }

      for (set< weak_ptr<Node> >::iterator iterator = current_reevaluation.reevaluation_node->output_references.begin();
           iterator != current_reevaluation.reevaluation_node->output_references.end();
           iterator++)
      {
        assert(iterator->expired() == false);
        if (iterator->lock()->GetNodeType() == VARIABLE) {
#ifdef _MSC_VER // This IF should be removed. It is here only because the GetQueueContainer returns not the deque in Unix?
          assert(std::find(GetStackContainer(touched_nodes).begin(), GetStackContainer(touched_nodes).end(), iterator->lock())
            == GetStackContainer(touched_nodes).end());
#endif
          TouchNode(iterator->lock(), touched_nodes, touched_nodes2, this_proposal);
          assert(reevaluation_result->passing_value != shared_ptr<VentureValue>());
          dynamic_pointer_cast<NodeVariable>(iterator->lock())->Reevaluate(reevaluation_result->passing_value,
                                                                           current_reevaluation.reevaluation_node,
                                                                           reevaluation_parameters);
          for (set< weak_ptr<Node> >::iterator variable_iterator =
                 dynamic_pointer_cast<NodeVariable>(iterator->lock())->output_references.begin();
               variable_iterator != dynamic_pointer_cast<NodeVariable>(iterator->lock())->output_references.end();
               variable_iterator++)
          {
            assert(iterator->expired() == false);
            assert(variable_iterator->lock()->GetNodeType() != VARIABLE);
            reevaluation_queue.insert(ReevaluationEntry(dynamic_pointer_cast<NodeEvaluation>(variable_iterator->lock()),
                                                        current_reevaluation.caller, // Or just NULL?
                                                        reevaluation_result->passing_value, // Or also just NULL?
                                                        REEVALUATION_PRIORITY__STANDARD - 1));
          }
        } else {
          if (current_reevaluation.reevaluation_node->parent.lock() != shared_ptr<NodeEvaluation>()) {
            if (current_reevaluation.reevaluation_node->parent.lock()->GetNodeType() == APPLICATION_CALLER &&
                  dynamic_pointer_cast<NodeApplicationCaller>(current_reevaluation.reevaluation_node->parent.lock())->application_operator ==
                    current_reevaluation.reevaluation_node) {
              if (CompareValue(reevaluation_result->passing_value,
                               dynamic_pointer_cast<NodeApplicationCaller>(
                                 current_reevaluation.reevaluation_node->parent.lock())->saved_evaluated_operator)) {
                continue; // The operator is the same.
              }
              omit_patterns.push(OmitPattern(dynamic_pointer_cast<NodeApplicationCaller>(
                                               current_reevaluation.reevaluation_node->parent.lock())->application_node->myorder,
                                             current_reevaluation.reevaluation_node->parent.lock()->myorder));
            }
          }
          assert(current_reevaluation.reevaluation_node != shared_ptr<NodeEvaluation>());
          reevaluation_queue.insert(ReevaluationEntry(dynamic_pointer_cast<NodeEvaluation>(iterator->lock()),
                                                      current_reevaluation.reevaluation_node,
                                                      reevaluation_result->passing_value,
                                                      REEVALUATION_PRIORITY__STANDARD));

        }
      }
    }
  }
  if (omit_patterns.size() > 0) {
    throw std::runtime_error("omit_patterns.size() > 0");
  }
  
  if (reevaluation_parameters.__unsatisfied_constraint == true) {
    throw std::runtime_error("Unsatisfied constraint. Sorry, rejection sampling still is not implemented."); // FIXME: implement!
  }

  real Q_OldToNew = reevaluation_parameters.__log_q_from_old_to_new;
  real Q_NewToOld = 0.0; // Calculated below.
  real P_new = 0.0; // Calculated below.
  set< shared_ptr<NodeXRPApplication> > creating_random_choices;
  real P_old = 0.0; // Calculated below.
  set< shared_ptr<NodeXRPApplication> > deleting_random_choices;

  for (size_t index = 0; index < touched_nodes2.size(); index++) {
    shared_ptr<Node> current_node = touched_nodes2[index];
    if (current_node->GetNodeType() == APPLICATION_CALLER) {
      shared_ptr<NodeApplicationCaller> current_node2 = dynamic_pointer_cast<NodeApplicationCaller>(current_node);
      
      if (current_node2->MH_made_action != MH_ACTION__LAMBDA_PROPAGATED) {
        assert(current_node2->application_node != shared_ptr<NodeEvaluation>());
        assert(current_node2->new_application_node != shared_ptr<NodeEvaluation>());
        Q_NewToOld += GatherBranch__QNewToOld(current_node2->application_node, current_node2->MH_made_action);
        if (current_node2->MH_made_action == MH_ACTION__EMPTY_STATUS) {
          throw std::runtime_error("The application caller node has empty MH action history.");
        }
      }
    }
  }
  
  cout << "Random choice: " << random_choice << endl;

  for (size_t index = 0; index < touched_nodes2.size(); index++) {
    shared_ptr<Node> current_node = touched_nodes2[index];
    if (current_node->GetNodeType() == APPLICATION_CALLER) {
      shared_ptr<NodeApplicationCaller> current_node2 = dynamic_pointer_cast<NodeApplicationCaller>(current_node);
      
      if (current_node2->MH_made_action != MH_ACTION__LAMBDA_PROPAGATED) {
        cout << "IT: " << current_node2->new_application_node << endl;
        P_new += GatherBranch__PNew(current_node2->new_application_node, current_node2->MH_made_action, creating_random_choices);
      }
    }
  }

  for (size_t index = 0; index < touched_nodes2.size(); index++) {
    shared_ptr<Node> current_node = touched_nodes2[index];
    if (current_node->GetNodeType() == APPLICATION_CALLER) {
      shared_ptr<NodeApplicationCaller> current_node2 = dynamic_pointer_cast<NodeApplicationCaller>(current_node);
      
      if (current_node2->MH_made_action != MH_ACTION__LAMBDA_PROPAGATED) {
        P_old += GatherBranch__POld(current_node2->application_node, current_node2->MH_made_action, deleting_random_choices, true);
      }
    }
  }

  size_t posterior_number_of_random_choices =
    number_of_random_choices + creating_random_choices.size() - deleting_random_choices.size();
  real number_of_random_choices_formula_component =
    log((1.0 / posterior_number_of_random_choices)
          / (1.0 / number_of_random_choices));
  //cout << number_of_random_choices << " " << posterior_number_of_random_choices << endl;
  real to_compare = number_of_random_choices_formula_component;
  real scores_part = P_new - P_old + Q_NewToOld - Q_OldToNew;
  //int tmp_tmp1 = dynamic_pointer_cast<NodeDirectivePredict>(GetLastDirectiveNode()->earlier_evaluation_nodes)->my_value->GetReal();
  //int tmp_tmp2 = -1;
  //if (dynamic_pointer_cast<NodeDirectivePredict>(GetLastDirectiveNode()->earlier_evaluation_nodes)->my_new_value != shared_ptr<VentureValue>()) {
  //  tmp_tmp2 = dynamic_pointer_cast<NodeDirectivePredict>(GetLastDirectiveNode()->earlier_evaluation_nodes)->my_new_value->GetReal();
  //}

  to_compare += scores_part;
  MHDecision mh_decision;
  real random_value = log(gsl_ran_flat(random_generator, 0, 1));
  if (random_value < to_compare) {
    mh_decision = MH_APPROVED;
    cout << "Approved" << endl;
  } else {
    mh_decision = MH_DECLINED;
    cout << "Decline" << endl;
  }
  // cout << "***" << endl;

  if (mh_decision == MH_APPROVED) {
    for (set< shared_ptr<NodeXRPApplication> >::const_iterator iterator = deleting_random_choices.begin();
         iterator != deleting_random_choices.end();
         iterator++) {
      //assert(creating_random_choices.count(*iterator) == 0);
      random_choices.erase(*iterator);
    }
    for (set< shared_ptr<NodeXRPApplication> >::const_iterator iterator = creating_random_choices.begin();
         iterator != creating_random_choices.end();
         iterator++) {
      //assert(deleting_random_choices.count(*iterator) == 0);
      random_choices.insert(*iterator);
    }
  }

  stack< shared_ptr<Node> > touched_nodes_copy = touched_nodes;

  while (!touched_nodes.empty()) {
    shared_ptr<Node> current_node = touched_nodes.top();
    touched_nodes.pop();
    if (current_node->was_deleted == true) { continue; } // FIXME: Why do we need it?
    if (current_node->GetNodeType() == VARIABLE) {
      assert(dynamic_pointer_cast<NodeVariable>(current_node)->new_value != shared_ptr<VentureValue>());
      if (mh_decision == MH_APPROVED) {
        dynamic_pointer_cast<NodeVariable>(current_node)->value = dynamic_pointer_cast<NodeVariable>(current_node)->new_value;
      }
      dynamic_pointer_cast<NodeVariable>(current_node)->new_value = shared_ptr<VentureValue>();
    } else if (current_node->GetNodeType() == XRP_APPLICATION) {
      continue; // Right?
    } else if (current_node->GetNodeType() == APPLICATION_CALLER) {
      assert(dynamic_pointer_cast<NodeApplicationCaller>(current_node)->MH_made_action != MH_ACTION__EMPTY_STATUS);

      if (dynamic_pointer_cast<NodeApplicationCaller>(current_node)->new_application_node != shared_ptr<NodeEvaluation>()) {
        {
          // Adding back.
          set< shared_ptr<NodeXRPApplication> > useless_set;
          GatherBranch__POld(dynamic_pointer_cast<NodeApplicationCaller>(current_node)->new_application_node, MH_ACTION__EMPTY_STATUS, useless_set, false); // Adding.
        }

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
      }

      dynamic_pointer_cast<NodeApplicationCaller>(current_node)->MH_made_action = MH_ACTION__EMPTY_STATUS;
    } else if (current_node->GetNodeType() == DIRECTIVE_ASSUME) {
      if (mh_decision == MH_APPROVED) {
        dynamic_pointer_cast<NodeDirectiveAssume>(current_node)->my_value =
          dynamic_pointer_cast<NodeDirectiveAssume>(current_node)->my_new_value;
      }
      dynamic_pointer_cast<NodeDirectiveAssume>(current_node)->my_new_value = shared_ptr<VentureValue>();
    } else if (current_node->GetNodeType() == DIRECTIVE_PREDICT) {
      if (mh_decision == MH_APPROVED) {
        dynamic_pointer_cast<NodeDirectivePredict>(current_node)->my_value =
          dynamic_pointer_cast<NodeDirectivePredict>(current_node)->my_new_value;
      }
      dynamic_pointer_cast<NodeDirectivePredict>(current_node)->my_new_value = shared_ptr<VentureValue>();
    } else if (current_node->GetNodeType() == DIRECTIVE_ASSUME ||
                current_node->GetNodeType() == DIRECTIVE_PREDICT ||
                current_node->GetNodeType() == DIRECTIVE_OBSERVE ||
                current_node->GetNodeType() == APPLICATION_CALLER ||
                current_node->GetNodeType() == LOOKUP) {
      // Nothing.
    } else {
      cout << current_node->GetNodeType() << endl;
      throw std::runtime_error("Unexpected node type.");
    }
  }
}

real GatherBranch__QNewToOld(shared_ptr<Node> first_node, size_t node_action) {
  real changed_probability = log(1.0);
  queue< shared_ptr<Node> > processing_queue;
  processing_queue.push(first_node);
  while (!processing_queue.empty()) {
    processing_queue.front()->GetChildren(processing_queue);
    shared_ptr<Node> current_node = processing_queue.front();
    processing_queue.pop();

    if (current_node->GetNodeType() == XRP_APPLICATION) {
      shared_ptr<NodeXRPApplication> current_node2 = dynamic_pointer_cast<NodeXRPApplication>(current_node);

      vector< shared_ptr<VentureValue> > got_arguments = GetArgumentsFromEnvironment(current_node2->environment, // Not efficient?
                                      dynamic_pointer_cast<NodeEvaluation>(current_node2), true);
      current_node2->xrp->xrp->Remove(got_arguments, current_node2->my_sampled_value);
      
      if (node_action == MH_ACTION__RESAMPLED) {
        if (current_node2->forced_by_observations == false) {
          changed_probability += current_node2->xrp->xrp->GetSampledLoglikelihood(got_arguments, current_node2->my_sampled_value);
        }
      }

      if (current_node->GetNodeType() == XRP_APPLICATION &&
            dynamic_pointer_cast<NodeXRPApplication>(current_node)->xrp->xrp->GetName() == "XRP__memoized_procedure")
      {
        string mem_table_key = XRP__memoized_procedure__MakeMapKeyFromArguments(got_arguments);
        XRP__memoizer_map_element& mem_table_element =
          (*(dynamic_pointer_cast<XRP__memoized_procedure>(dynamic_pointer_cast<NodeXRPApplication>(current_node)->xrp->xrp)->mem_table.find(mem_table_key))).second;
        if (mem_table_element.active_uses == 0) {
          processing_queue.push(mem_table_element.application_caller_node);
        }
      }
    }
  }
  return changed_probability;
}

real GatherBranch__PNew(shared_ptr<Node> first_node, size_t node_action, set< shared_ptr<NodeXRPApplication> >& changing_random_choices) {
  real changed_probability = log(1.0);
  queue< shared_ptr<Node> > processing_queue;
  processing_queue.push(first_node);
  while (!processing_queue.empty()) {
    processing_queue.front()->GetChildren(processing_queue);
    shared_ptr<Node> current_node = processing_queue.front();
    processing_queue.pop();

    if (current_node->GetNodeType() == XRP_APPLICATION) {
      shared_ptr<NodeXRPApplication> current_node2 = dynamic_pointer_cast<NodeXRPApplication>(current_node);

      vector< shared_ptr<VentureValue> > got_arguments = GetArgumentsFromEnvironment(current_node2->environment, // Not efficient?
                                      dynamic_pointer_cast<NodeEvaluation>(current_node2), false);
      current_node2->xrp->xrp->Remove(got_arguments, current_node2->my_sampled_value);
      
      changed_probability += current_node2->xrp->xrp->GetSampledLoglikelihood(got_arguments, current_node2->my_sampled_value);
      if (current_node2->xrp->xrp->IsRandomChoice() && current_node2->forced_by_observations == false) {
        changing_random_choices.insert(current_node2);
      }

      if (current_node->GetNodeType() == XRP_APPLICATION &&
            dynamic_pointer_cast<NodeXRPApplication>(current_node)->xrp->xrp->GetName() == "XRP__memoized_procedure")
      {
        string mem_table_key = XRP__memoized_procedure__MakeMapKeyFromArguments(got_arguments);
        XRP__memoizer_map_element& mem_table_element =
          (*(dynamic_pointer_cast<XRP__memoized_procedure>(dynamic_pointer_cast<NodeXRPApplication>(current_node)->xrp->xrp)->mem_table.find(mem_table_key))).second;
        if (mem_table_element.active_uses == 0) {
          processing_queue.push(mem_table_element.application_caller_node);
        }
      }
    }
  }
  return changed_probability;
}

real GatherBranch__POld(shared_ptr<Node> first_node, size_t node_action, set< shared_ptr<NodeXRPApplication> >& changing_random_choices, bool if_old_variables) {
  real changed_probability = log(1.0);
  int number_of_random_choices = 0;
  queue< shared_ptr<Node> > processing_queue;
  processing_queue.push(first_node);
  while (!processing_queue.empty()) {
    processing_queue.front()->GetChildren(processing_queue);
    shared_ptr<Node> current_node = processing_queue.front();
    processing_queue.pop();

    if (current_node->GetNodeType() == XRP_APPLICATION) {
      shared_ptr<NodeXRPApplication> current_node2 = dynamic_pointer_cast<NodeXRPApplication>(current_node);

      vector< shared_ptr<VentureValue> > got_arguments = GetArgumentsFromEnvironment(current_node2->environment, // Not efficient?
                                      dynamic_pointer_cast<NodeEvaluation>(current_node2), if_old_variables);
      
      changed_probability += current_node2->xrp->xrp->GetSampledLoglikelihood(got_arguments, current_node2->my_sampled_value);
      if (current_node2->xrp->xrp->IsRandomChoice() && current_node2->forced_by_observations == false) {
        changing_random_choices.insert(current_node2);
      }
      current_node2->xrp->xrp->Incorporate(got_arguments, current_node2->my_sampled_value);

      if (current_node->GetNodeType() == XRP_APPLICATION &&
            dynamic_pointer_cast<NodeXRPApplication>(current_node)->xrp->xrp->GetName() == "XRP__memoized_procedure")
      {
        string mem_table_key = XRP__memoized_procedure__MakeMapKeyFromArguments(got_arguments);
        XRP__memoizer_map_element& mem_table_element =
          (*(dynamic_pointer_cast<XRP__memoized_procedure>(dynamic_pointer_cast<NodeXRPApplication>(current_node)->xrp->xrp)->mem_table.find(mem_table_key))).second;
        if (mem_table_element.active_uses == 1) {
          processing_queue.push(mem_table_element.application_caller_node);
        }
      }
    }
  }
  return changed_probability;
}
