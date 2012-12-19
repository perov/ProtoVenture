
#include "Header.h"
#include "VentureValues.h"
#include "VentureParser.h"
#include "Analyzer.h"
#include "XRP.h"
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
    if (dynamic_pointer_cast<NodeLookup>(node)->where_lookuped != shared_ptr<NodeVariable>()) {
      dynamic_pointer_cast<NodeLookup>(node)->where_lookuped->output_references.erase(node);
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
        Remove(old_arguments, dynamic_pointer_cast<NodeXRPApplication>(node)->sampled_value);

      dynamic_pointer_cast<NodeXRPApplication>(node)->xrp->xrp->Unsampler(old_arguments, dynamic_pointer_cast<NodeXRPApplication>(node));

      if (dynamic_pointer_cast<NodeXRPApplication>(node)->xrp->xrp->IsRandomChoice() == true) {
        random_choices.erase(dynamic_pointer_cast<NodeXRPApplication>(node->shared_from_this()));
      }

      //if (dynamic_pointer_cast<NodeXRPApplication>(node)->xrp->xrp->SaveReferencesToItsSamplers() == true) {
      //  dynamic_pointer_cast<NodeXRPApplication>(node)->xrp->xrp->references_to_its_samplers
      //    .erase(dynamic_pointer_cast<NodeXRPApplication>(node)->shared_from_this());
      //}
    }
  }

  if (dynamic_pointer_cast<NodeEvaluation>(node) != shared_ptr<NodeEvaluation>() &&
        dynamic_pointer_cast<NodeEvaluation>(node)->parent.lock() != shared_ptr<NodeEvaluation>() &&
        dynamic_pointer_cast<NodeEvaluation>(node)->parent.lock()->GetType() == APPLICATION_CALLER) {
    // Only now we can clean the environment.
    shared_ptr<NodeEvaluation>& application_node =
      dynamic_pointer_cast<NodeApplicationCaller>(dynamic_pointer_cast<NodeEvaluation>(node)->parent.lock())->application_node;
    if (application_node != shared_ptr<NodeEvaluation>() &&
          application_node->environment != shared_ptr<NodeEnvironment>()) {
      // THIS NOTICE IS UNIVERSAL, AND IT IS CALLED "BAD-POINTER"
      // Roughly speaking, if the engine works without errors,
      // application_node always should not be NULL.
      // TODO: figure out why this invariant fails
      // if there were errors in RIPL requests
      application_node->environment->DeleteNode();
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

  set<ReevaluationEntry,
      ReevaluationOrderComparer> reevaluation_queue;
  reevaluation_queue.insert(ReevaluationEntry(random_choice,
                                            shared_ptr<NodeEvaluation>(),
                                            shared_ptr<VentureValue>()));

  stack<OmitPattern> omit_patterns;

  stack< shared_ptr<Node> > touched_nodes; // Should be LIFO for Unsampler(...)!
                                           // (To correctly receive old arguments.)
  
  ReevaluationParameters reevaluation_parameters;

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
        //cout << "Omitting" << endl;
        continue;
      }
      if (omit_patterns.top().stop_pattern == current_reevaluation.reevaluation_node->myorder) {
        omit_patterns.pop();
      }
    }
#ifdef _MSC_VER // This IF should be removed. It is here only because the GetQueueContainer returns not the deque in Unix?
#ifndef NDEBUG
    std::deque< shared_ptr<Node> >::const_iterator already_existent_element =
      std::find(GetQueueContainer(touched_nodes).begin(), GetQueueContainer(touched_nodes).end(), current_reevaluation.reevaluation_node);
    if (!(already_existent_element == GetQueueContainer(touched_nodes).end())) {
      int distance = std::distance(GetQueueContainer(touched_nodes).begin(), already_existent_element);
      cout << "Pam: " << distance << endl;
      DrawGraphDuringMH(GetLastDirectiveNode(), touched_nodes);
    }
#endif
#endif
#ifdef _MSC_VER // This IF should be removed. It is here only because the GetQueueContainer returns not the deque in Unix?
    assert(std::find(GetQueueContainer(touched_nodes).begin(), GetQueueContainer(touched_nodes).end(), current_reevaluation.reevaluation_node)
      == GetQueueContainer(touched_nodes).end());
#endif
    touched_nodes.push(current_reevaluation.reevaluation_node);
    shared_ptr<ReevaluationResult> reevaluation_result =
      current_reevaluation.reevaluation_node->Reevaluate(current_reevaluation.passing_value,
                                                         current_reevaluation.caller,
                                                         reevaluation_parameters);
    
    if (reevaluation_result->pass_further == true) {
      for (set< weak_ptr<Node> >::iterator iterator = current_reevaluation.reevaluation_node->output_references.begin();
           iterator != current_reevaluation.reevaluation_node->output_references.end();
           iterator++)
      {
        if (iterator->lock()->GetNodeType() == VARIABLE) {
#ifdef _MSC_VER // This IF should be removed. It is here only because the GetQueueContainer returns not the deque in Unix?
          assert(std::find(GetQueueContainer(touched_nodes).begin(), GetQueueContainer(touched_nodes).end(), iterator->lock())
            == GetQueueContainer(touched_nodes).end());
#endif
          touched_nodes.push(iterator->lock());
          assert(reevaluation_result->passing_value != shared_ptr<VentureValue>());
          dynamic_pointer_cast<NodeVariable>(iterator->lock())->new_value = reevaluation_result->passing_value;
          for (set< weak_ptr<Node> >::iterator variable_iterator =
                 dynamic_pointer_cast<NodeVariable>((*iterator).lock())->output_references.begin();
               variable_iterator != dynamic_pointer_cast<NodeVariable>(iterator->lock())->output_references.end();
               variable_iterator++)
          {
            assert((variable_iterator->lock())->GetNodeType() != VARIABLE);
            reevaluation_queue.insert(ReevaluationEntry(dynamic_pointer_cast<NodeEvaluation>(variable_iterator->lock()),
                                                      current_reevaluation.caller, // Or just NULL?
                                                      reevaluation_result->passing_value)); // Or also just NULL?
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
                                                    reevaluation_result->passing_value));

        }
      }
    }
  }
  if (omit_patterns.size() > 0) {
    throw std::runtime_error("omit_patterns.size() > 0");
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
    shared_ptr<Node> current_node = touched_nodes.top();
    touched_nodes.pop();
    if (current_node->was_deleted == true) { continue; } // FIXME: Why do we need it?
    if (current_node->GetNodeType() == VARIABLE) {
      assert(dynamic_pointer_cast<NodeVariable>(current_node)->new_value != shared_ptr<VentureValue>());
      if (mh_decision == MH_APPROVED) {
        dynamic_pointer_cast<NodeVariable>(current_node)->value = dynamic_pointer_cast<NodeVariable>(current_node)->new_value;
      }
      dynamic_pointer_cast<NodeVariable>(current_node)->new_value = shared_ptr<VentureValue>();
    } else if (current_node->GetNodeType() == APPLICATION_CALLER &&
                 dynamic_pointer_cast<NodeApplicationCaller>(current_node)->new_application_node != shared_ptr<NodeEvaluation>())
    {
      UnfreezeBranch(dynamic_pointer_cast<NodeApplicationCaller>(current_node)->application_node);

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
        // FIXME: Why we do not make Remove/Incorporate if it is was rescored, though
        // we do it on the rescoring-resampling step?

        // FIXME: GetArgumentsFromEnvironment should be called without adding lookup references!
        vector< shared_ptr<VentureValue> > old_arguments = GetArgumentsFromEnvironment(dynamic_pointer_cast<NodeXRPApplication>(current_node)->environment, // Not efficient?
                                        dynamic_pointer_cast<NodeEvaluation>(current_node), // Are we sure that we have not deleted yet lookup links?
                                        true); // FIXME: we should be sure that we are receiving old arguments!

        dynamic_pointer_cast<NodeXRPApplication>(current_node)->xrp->xrp->Unfreeze(old_arguments, dynamic_pointer_cast<NodeXRPApplication>(current_node));

        if (mh_decision == MH_APPROVED) {
          dynamic_pointer_cast<NodeXRPApplication>(current_node)->xrp->xrp->Unsampler(old_arguments, dynamic_pointer_cast<NodeXRPApplication>(current_node));

          assert(dynamic_pointer_cast<NodeXRPApplication>(current_node)->new_sampled_value != shared_ptr<VentureValue>());
          dynamic_pointer_cast<NodeXRPApplication>(current_node)->sampled_value =
            dynamic_pointer_cast<NodeXRPApplication>(current_node)->new_sampled_value;
        } else {
          // FIXME: GetArgumentsFromEnvironment should be called without adding lookup references!
          vector< shared_ptr<VentureValue> > new_arguments = GetArgumentsFromEnvironment(dynamic_pointer_cast<NodeXRPApplication>(current_node)->environment, // Not efficient?
                                          dynamic_pointer_cast<NodeEvaluation>(current_node), // Are we sure that we have not deleted yet lookup links?
                                          false); // FIXME: we should be sure that we are receiving old arguments!
          dynamic_pointer_cast<NodeXRPApplication>(current_node)->xrp->xrp->Remove(new_arguments, dynamic_pointer_cast<NodeXRPApplication>(current_node)->new_sampled_value);
          dynamic_pointer_cast<NodeXRPApplication>(current_node)->xrp->xrp->Unsampler(new_arguments, dynamic_pointer_cast<NodeXRPApplication>(current_node));
          dynamic_pointer_cast<NodeXRPApplication>(current_node)->xrp->xrp->Incorporate(old_arguments, dynamic_pointer_cast<NodeXRPApplication>(current_node)->sampled_value);
        }
        dynamic_pointer_cast<NodeXRPApplication>(current_node)->new_sampled_value = shared_ptr<VentureValue>();
      }
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
  
  /*
  if (mh_decision == MH_APPROVED) {
    cout << "Approved" << endl << endl << endl;
  } else {
    cout << "Decline" << endl << endl << endl;
  }
  */
}
