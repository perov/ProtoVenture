
#include "HeaderPre.h"
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

void DeleteNode(shared_ptr<Node> node, bool old_values) {
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

      /*
      // FIXME: GetArgumentsFromEnvironment should be called without adding lookup references!
      vector< shared_ptr<VentureValue> > old_arguments = GetArgumentsFromEnvironment(dynamic_pointer_cast<NodeXRPApplication>(node)->environment, // Not efficient?
                                      dynamic_pointer_cast<NodeEvaluation>(node), // Are we sure that we have not deleted yet lookup links?
                                      true); // FIXME: we should be sure that we are receiving old arguments!

      dynamic_pointer_cast<NodeXRPApplication>(node)->xrp->xrp->
        Remove(old_arguments, dynamic_pointer_cast<NodeXRPApplication>(node)->my_sampled_value);

      dynamic_pointer_cast<NodeXRPApplication>(node)->xrp->xrp->Unsampler(old_arguments, dynamic_pointer_cast<NodeXRPApplication>(node));

      if (dynamic_pointer_cast<NodeXRPApplication>(node)->xrp->xrp->IsRandomChoice() == true) {
        GetSizeOfRandomChoices(dynamic_pointer_cast<NodeXRPApplication>(node->shared_from_this()));
      }
      */
    }
  }
}

void DeleteBranch(shared_ptr<Node> first_node, bool old_values) {
  ApplyToMeAndAllMyChildren(first_node, old_values, DeleteNode);
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

void AddToReevaluationQueue
(ReevaluationEntry& current_reevaluation,
 shared_ptr<ReevaluationResult> reevaluation_result,
 shared_ptr<ReevaluationParameters> reevaluation_parameters)
{
  if (current_reevaluation.reevaluation_node->GetNodeType() == DIRECTIVE_OBSERVE) {
    current_reevaluation.reevaluation_node = dynamic_pointer_cast<NodeEvaluation>(reevaluation_result->passing_value);
    reevaluation_result->passing_value = dynamic_pointer_cast<NodeXRPApplication>(current_reevaluation.reevaluation_node)->my_sampled_value;
  }

  // If arguments have changed, delete the potentially existing reevaluation result
  // from the memoizer node.
  if (current_reevaluation.reevaluation_node->GetNodeType() == XRP_APPLICATION) {
    if (dynamic_pointer_cast<NodeXRPApplication>(current_reevaluation.reevaluation_node)->xrp->xrp->GetName() ==
          "XRP__memoized_procedure") {
      reevaluation_parameters->
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
      assert(std::find(GetStackContainer(reevaluation_parameters->touched_nodes).begin(), GetStackContainer(reevaluation_parameters->touched_nodes).end(), iterator->lock())
        == GetStackContainer(reevaluation_parameters->touched_nodes).end());
#endif
      TouchNode(iterator->lock(), reevaluation_parameters->touched_nodes, reevaluation_parameters->touched_nodes2, reevaluation_parameters->this_proposal);
      // assert(reevaluation_result->passing_value != shared_ptr<VentureValue>());
      dynamic_pointer_cast<NodeVariable>(iterator->lock())->Reevaluate(reevaluation_result->passing_value,
                                                                        current_reevaluation.reevaluation_node,
                                                                        reevaluation_parameters);
      for (set< weak_ptr<Node> >::iterator variable_iterator =
              dynamic_pointer_cast<NodeVariable>(iterator->lock())->output_references.begin();
            variable_iterator != dynamic_pointer_cast<NodeVariable>(iterator->lock())->output_references.end();
            variable_iterator++)
      {
        assert(variable_iterator->expired() == false);
        assert(variable_iterator->lock()->GetNodeType() != VARIABLE);
        shared_ptr<VentureValue> tmp_passing_value;
        if (dynamic_pointer_cast<NodeEvaluation>(variable_iterator->lock())->GetNodeType() == LOOKUP) {
          tmp_passing_value = reevaluation_result->passing_value;
        } else {
          tmp_passing_value = shared_ptr<VentureValue>();
        }
        reevaluation_parameters->reevaluation_queue.insert(ReevaluationEntry(dynamic_pointer_cast<NodeEvaluation>(variable_iterator->lock()),
                                                                             current_reevaluation.caller, // Or just NULL?
                                                                             tmp_passing_value, // Or also just NULL?
                                                                             REEVALUATION_PRIORITY__STANDARD - 1));
      }
    } else {
      if (current_reevaluation.reevaluation_node->parent.lock() != shared_ptr<NodeEvaluation>()) {
        if (current_reevaluation.reevaluation_node->parent.lock()->GetNodeType() == APPLICATION_CALLER &&
              current_reevaluation.reevaluation_node->parent.lock()->already_propagated != shared_ptr<VentureValue>()) {
          assert(CompareValue(current_reevaluation.reevaluation_node->parent.lock()->already_propagated, reevaluation_result->passing_value));
          continue;
        }
      }

      if (current_reevaluation.reevaluation_node->parent.lock() != shared_ptr<NodeEvaluation>()) {
        if (current_reevaluation.reevaluation_node->parent.lock()->GetNodeType() == APPLICATION_CALLER &&
              dynamic_pointer_cast<NodeApplicationCaller>(current_reevaluation.reevaluation_node->parent.lock())->application_operator ==
                current_reevaluation.reevaluation_node) {
          if (CompareValue(reevaluation_result->passing_value,
                            dynamic_pointer_cast<NodeApplicationCaller>(
                              current_reevaluation.reevaluation_node->parent.lock())->saved_evaluated_operator)) {
            continue; // The operator is the same.
          }
          reevaluation_parameters->
            omit_patterns.push(OmitPattern(dynamic_pointer_cast<NodeApplicationCaller>(
                                            current_reevaluation.reevaluation_node->parent.lock())->application_node->myorder,
                                          current_reevaluation.reevaluation_node->parent.lock()->myorder));
        }
      }
      assert(current_reevaluation.reevaluation_node != shared_ptr<NodeEvaluation>());
      reevaluation_parameters->
        reevaluation_queue.insert(ReevaluationEntry(dynamic_pointer_cast<NodeEvaluation>(iterator->lock()),
                                                  current_reevaluation.reevaluation_node,
                                                  reevaluation_result->passing_value,
                                                  REEVALUATION_PRIORITY__STANDARD));

    }
  }
}

void PropagateNewValue
(set<ReevaluationEntry,
   ReevaluationOrderComparer>& reevaluation_queue,
 stack< shared_ptr<Node> >& touched_nodes,
 vector< shared_ptr<Node> >& touched_nodes2,
 ProposalInfo& this_proposal,
 shared_ptr<ReevaluationParameters>& reevaluation_parameters)
{
  stack<OmitPattern>& omit_patterns = reevaluation_parameters->omit_patterns;

  while (reevaluation_queue.size() != 0) {
    if (reevaluation_parameters->__unsatisfied_constraint == true) {
      return;
    }

    ReevaluationEntry current_reevaluation = *(reevaluation_queue.rbegin());
    {
      set<ReevaluationEntry,
          ReevaluationOrderComparer>::iterator iterator_to_last_element = reevaluation_queue.end();
      --iterator_to_last_element;
      reevaluation_queue.erase(iterator_to_last_element);
    }
    if (!omit_patterns.empty()) {
      if (VerifyOrderPattern(omit_patterns.top().omit_pattern, current_reevaluation.reevaluation_node->myorder)) {
        if (omit_patterns.top().stop_pattern == current_reevaluation.reevaluation_node->myorder) {
          omit_patterns.pop(); // FIXME: do not copy this condition the second time below?
        }
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
      //cout << "Pam: " << distance << endl;
      DrawGraphDuringMH(touched_nodes);
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
      AddToReevaluationQueue(current_reevaluation, reevaluation_result, reevaluation_parameters);
    }
  }
  if (omit_patterns.size() > 0) {
    throw std::runtime_error("omit_patterns.size() > 0");
  }
}

void FinalizeProposal
(MHDecision mh_decision,
 shared_ptr<ReevaluationParameters>& reevaluation_parameters)
{
  if (mh_decision == MH_APPROVED) {
    // Order matters!
    // If firstly we remove, then incorporate:
    //   ASSUME proc (mem (lambda () (flip)))
    //   PREDICT (if (flip) (proc) (proc))
    // (from_Clojure_engine_tests_with_theoretical_25_test.py)
    for (set< shared_ptr<NodeXRPApplication> >::const_iterator iterator = reevaluation_parameters->deleting_random_choices.begin();
         iterator != reevaluation_parameters->deleting_random_choices.end();
         iterator++) {
      //assert(creating_random_choices.count(*iterator) == 0);
      DeleteRandomChoices(*iterator);
    }
    for (set< shared_ptr<NodeXRPApplication> >::const_iterator iterator = reevaluation_parameters->creating_random_choices.begin();
         iterator != reevaluation_parameters->creating_random_choices.end();
         iterator++) {
      //assert(deleting_random_choices.count(*iterator) == 0);
      AddToRandomChoices(*iterator);
    }
  }

  while (!reevaluation_parameters->touched_nodes.empty()) {
    shared_ptr<Node> current_node = reevaluation_parameters->touched_nodes.top();
    current_node->already_propagated = shared_ptr<VentureValue>();
    reevaluation_parameters->touched_nodes.pop();
    if (current_node->was_deleted == true) { continue; } // FIXME: Why do we need it?
    if (current_node->GetNodeType() == VARIABLE) {
      assert(dynamic_pointer_cast<NodeVariable>(current_node)->new_value != shared_ptr<VentureValue>());
      if (mh_decision == MH_APPROVED) {
        if (dynamic_pointer_cast<NodeVariable>(current_node)->output_references.size() == 1 &&
              dynamic_pointer_cast<NodeVariable>(current_node)->output_references.begin()->lock()->GetNodeType() == XRP_APPLICATION &&
              ((dynamic_pointer_cast<NodeXRPApplication>(dynamic_pointer_cast<NodeVariable>(current_node)->output_references.begin()->lock())->xrp->xrp->GetName() != "XRP__SymmetricDirichletMultinomial_maker" && dynamic_pointer_cast<NodeXRPApplication>(dynamic_pointer_cast<NodeVariable>(current_node)->output_references.begin()->lock())->xrp->xrp->GetName() != "XRP__CRPmaker") ||
               global_environment->variables.count("fast-calc-joint-prob") != 1)) {
          // Do nothing, because arguments of the NodeXRPApplication has been changed.
        } else {
          dynamic_pointer_cast<NodeVariable>(current_node)->value = dynamic_pointer_cast<NodeVariable>(current_node)->new_value;
        }
      }
      dynamic_pointer_cast<NodeVariable>(current_node)->new_value = shared_ptr<VentureValue>();
    } else if (current_node->GetNodeType() == XRP_APPLICATION) {
      continue; // Right?
    } else if (current_node->GetNodeType() == APPLICATION_CALLER) {
      assert(dynamic_pointer_cast<NodeApplicationCaller>(current_node)->MH_made_action != MH_ACTION__EMPTY_STATUS);

      if (dynamic_pointer_cast<NodeApplicationCaller>(current_node)->MH_made_action == MH_ACTION__SDD_RESCORED) {
        if (dynamic_pointer_cast<VentureXRP>(dynamic_pointer_cast<NodeXRPApplication>(dynamic_pointer_cast<NodeApplicationCaller>(current_node)->application_node)->my_sampled_value)->xrp->GetName() == "XRP__DirichletMultinomial_sampler")
        {
          if (mh_decision == MH_DECLINED) {
            shared_ptr<XRP__DirichletMultinomial_sampler> xrpobject =
              dynamic_pointer_cast<XRP__DirichletMultinomial_sampler>(dynamic_pointer_cast<VentureXRP>(dynamic_pointer_cast<NodeXRPApplication>(dynamic_pointer_cast<NodeApplicationCaller>(current_node)->application_node)->my_sampled_value)->xrp);
            for (size_t index = 0; index < xrpobject->statistics.size(); index++) {
              xrpobject->statistics[index] += xrpobject->old_a - xrpobject->new_a;
            }
            xrpobject->sum_of_statistics += (xrpobject->old_a - xrpobject->new_a) * xrpobject->statistics.size();
          }
        } else if (dynamic_pointer_cast<VentureXRP>(dynamic_pointer_cast<NodeXRPApplication>(dynamic_pointer_cast<NodeApplicationCaller>(current_node)->application_node)->my_sampled_value)->xrp->GetName() == "XRP__CRPsampler") {
          if (mh_decision == MH_DECLINED) {
            shared_ptr<XRP__CRPsampler> xrpobject =
              dynamic_pointer_cast<XRP__CRPsampler>(dynamic_pointer_cast<VentureXRP>(dynamic_pointer_cast<NodeXRPApplication>(dynamic_pointer_cast<NodeApplicationCaller>(current_node)->application_node)->my_sampled_value)->xrp);
            xrpobject->alpha = xrpobject->old_alpha;
          }
        } else {
          throw std::runtime_error("Unknown MH_ACTION__SDD_RESCORED source.");
        }
      }

      if (dynamic_pointer_cast<NodeApplicationCaller>(current_node)->new_application_node != shared_ptr<NodeEvaluation>()) {
        UnabsorbBranchProbability(dynamic_pointer_cast<NodeApplicationCaller>(current_node)->application_node, shared_ptr<ReevaluationParameters>());
        
        if (mh_decision == MH_DECLINED) {
          if (current_node->constraint_times > 0) {
            FindConstrainingNode(dynamic_pointer_cast<NodeApplicationCaller>(current_node)->new_application_node, -1 * current_node->constraint_times, false);
            FindConstrainingNode(dynamic_pointer_cast<NodeApplicationCaller>(current_node)->application_node, current_node->constraint_times, true);
          }
        }
        
        if (mh_decision == MH_APPROVED) {
          ////Debug// cout << "IT__DeleteOld: " << dynamic_pointer_cast<NodeApplicationCaller>(current_node)->application_node << endl;
          //Q_NewToOld += GatherBranch__QNewToOld(dynamic_pointer_cast<NodeApplicationCaller>(current_node)->application_node, MH_ACTION__EMPTY_STATUS);
          //DeleteBranch(dynamic_pointer_cast<NodeApplicationCaller>(current_node)->application_node, true);
        } else {
          ////Debug// cout << "IT__DeleteNew: " << dynamic_pointer_cast<NodeApplicationCaller>(current_node)->new_application_node << endl;
          //set< shared_ptr<NodeXRPApplication> > useless_set;
          //P_new += GatherBranch__PNew(dynamic_pointer_cast<NodeApplicationCaller>(current_node)->new_application_node, MH_ACTION__EMPTY_STATUS, useless_set);
          //DeleteBranch(dynamic_pointer_cast<NodeApplicationCaller>(current_node)->new_application_node, false);
        }
      
        if (mh_decision == MH_APPROVED) {
          dynamic_pointer_cast<NodeApplicationCaller>(current_node)->application_node =
            dynamic_pointer_cast<NodeApplicationCaller>(current_node)->new_application_node;
          if (dynamic_pointer_cast<NodeApplicationCaller>(current_node)->proposing_evaluated_operator != shared_ptr<VentureValue>()) {
            // If operator has changed.
            dynamic_pointer_cast<NodeApplicationCaller>(current_node)->saved_evaluated_operator =
              dynamic_pointer_cast<NodeApplicationCaller>(current_node)->proposing_evaluated_operator;
          }
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
  
  /*
  if (dynamic_pointer_cast<NodeXRPApplication>(
        dynamic_pointer_cast<NodeApplicationCaller>(
          dynamic_pointer_cast<NodeDirectiveObserve>(directives[2].directive_node)->expression)->application_node)->constraint_times == 0) {
    cout << "Should not be1!" << endl;
    throw std::runtime_error("Should not be1!\n");
  }
  shared_ptr<NodeXRPApplication> constraint_xrp =
    dynamic_pointer_cast<NodeXRPApplication>(
        dynamic_pointer_cast<NodeApplicationCaller>(
          dynamic_pointer_cast<NodeDirectiveObserve>(directives[2].directive_node)->expression)->application_node);
  if (random_choices.count(constraint_xrp) != 0) {
    cout << "Should not be1a!" << endl;
    throw std::runtime_error("Should not be1a!\n");
  }
  if (dynamic_pointer_cast<NodeXRPApplication>(
        dynamic_pointer_cast<NodeApplicationCaller>(
          dynamic_pointer_cast<NodeDirectiveObserve>(directives[2].directive_node)->expression)->new_application_node) != shared_ptr<NodeXRPApplication>() &&
      dynamic_pointer_cast<NodeXRPApplication>(
        dynamic_pointer_cast<NodeApplicationCaller>(
          dynamic_pointer_cast<NodeDirectiveObserve>(directives[2].directive_node)->expression)->new_application_node)->constraint_times == 0) {
    cout << "Should not be2!" << endl;
    throw std::runtime_error("Should not be2!\n");
  }
  if (dynamic_pointer_cast<NodeXRPApplication>(
        dynamic_pointer_cast<NodeApplicationCaller>(
          dynamic_pointer_cast<NodeDirectiveObserve>(directives[2].directive_node)->expression)->new_application_node) != shared_ptr<NodeXRPApplication>()) {
    shared_ptr<NodeXRPApplication> constraint_xrp =
      dynamic_pointer_cast<NodeXRPApplication>(
          dynamic_pointer_cast<NodeApplicationCaller>(
            dynamic_pointer_cast<NodeDirectiveObserve>(directives[2].directive_node)->expression)->new_application_node);
    if (random_choices.count(constraint_xrp) != 0) {
      cout << "Should not be2a!" << endl;
      throw std::runtime_error("Should not be2a!\n");
    }
  }
  */
}

MHProposalResults MakeMHProposal
(shared_ptr<NodeXRPApplication> principal_node,
 shared_ptr<VentureValue> proposing_value,
 shared_ptr< map<string, shared_ptr<VentureValue> > > random_database,
 bool forcing_not_collecting)
{
  int proposal_unique_id = 0; // FIXME: deprecated?

  //Debug// cout << "New MH" << endl;

  ProposalInfo this_proposal;
  this_proposal.proposal_unique_id = proposal_unique_id; // FIXME: through constructor
  this_proposal.request_to_terminate = false; // FIXME: through constructor
  
  size_t number_of_random_choices = GetSizeOfRandomChoices();
  if (number_of_random_choices == 0) {
    return MHProposalResults(0.0); // There is no random choices in the trace.
  }

  shared_ptr<NodeXRPApplication> random_choice;

  if (principal_node == shared_ptr<NodeXRPApplication>()) {
    // set< weak_ptr<NodeXRPApplication> >::iterator iterator = random_choices.begin();
    // int random_choice_id = UniformDiscrete(0, number_of_random_choices - 1);
    // std::advance(iterator, random_choice_id);
    random_choice = GetRandomRandomChoice(); // FIXME: Should be NodeEvaluation?

    //if (random_choice->xrp->xrp->CouldBeEnumerated()) {
    //  Enumerate(random_choice);
    //  return MHProposalResults(0.0);
    //}
  } else {
    random_choice = principal_node;
  }

  set<ReevaluationEntry,
      ReevaluationOrderComparer> reevaluation_queue;
      
  stack< shared_ptr<Node> > touched_nodes; // Should be LIFO for Unsampler(...)!
                                           // (To correctly receive old arguments.)
  vector< shared_ptr<Node> > touched_nodes2; // FIXME: get rid of the touched_nodes2?

  stack<OmitPattern> omit_patterns;
  
  shared_ptr<ReevaluationParameters> reevaluation_parameters =
    shared_ptr<ReevaluationParameters>(new ReevaluationParameters(
      random_choice,
      reevaluation_queue,
      touched_nodes,
      touched_nodes2,
      this_proposal,
      omit_patterns));
  
  reevaluation_parameters->we_are_in_enumeration = proposing_value != shared_ptr<VentureValue>();
  reevaluation_parameters->proposing_value_for_this_proposal = proposing_value;
  reevaluation_parameters->random_database = random_database;
  reevaluation_parameters->forcing_not_collecting = forcing_not_collecting;

  reevaluation_queue.insert(ReevaluationEntry(random_choice,
                                            shared_ptr<NodeEvaluation>(),
                                            shared_ptr<VentureValue>(),
                                            REEVALUATION_PRIORITY__STANDARD));

  PropagateNewValue(reevaluation_queue, touched_nodes, touched_nodes2, this_proposal, reevaluation_parameters);

  real Q_OldToNew = reevaluation_parameters->__log_q_from_old_to_new;
  real Q_NewToOld = reevaluation_parameters->__log_q_from_new_to_old;
  real P_new = reevaluation_parameters->__log_p_new;
  real P_old = reevaluation_parameters->__log_p_old;
  
  size_t posterior_number_of_random_choices =
    number_of_random_choices + reevaluation_parameters->creating_random_choices.size() - reevaluation_parameters->deleting_random_choices.size();
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
  
  if (reevaluation_parameters->__unsatisfied_constraint == true) {
    cout << "Rejection sampling happens" << endl;
  }

  if (principal_node != shared_ptr<NodeXRPApplication>()) {
    if (forcing_not_collecting == false) {
      mh_decision = MH_DECLINED;
    } else {
      mh_decision = MH_APPROVED;
    }
  } else {
    real random_value = log(gsl_ran_flat(random_generator, 0, 1));
    if (random_value < to_compare && reevaluation_parameters->__unsatisfied_constraint != true) {
      mh_decision = MH_APPROVED;
    } else {
      mh_decision = MH_DECLINED;
    }
  }
  if (mh_decision == MH_APPROVED) {
    //Debug// cout << "Approved" << endl;
  } else {
    //Debug// cout << "Decline" << endl;
  }
  // cout << "***" << endl;

  /*
  cout << "New MH decision: " << mh_decision << " " << random_choice->xrp->xrp->GetName()
       << " " << random_choice->node_key
       << " " << random_choice->my_sampled_value->GetString()
       << " " << dynamic_pointer_cast<NodeXRPApplication>(dynamic_pointer_cast<NodeApplicationCaller>(random_choice->parent.lock())->new_application_node)->my_sampled_value->GetString()
       << " " << number_of_random_choices << " " << reevaluation_parameters->creating_random_choices.size() << " " << reevaluation_parameters->deleting_random_choices.size()
       << " " << P_new << " " << P_old << " " << Q_NewToOld << " " << Q_OldToNew << endl << endl;
  */

  FinalizeProposal(mh_decision, reevaluation_parameters);

  return MHProposalResults(P_new);
}

// Returns pair<logP_constraint, logP_unconstraint>
pair<real, real> AbsorbBranchProbability(shared_ptr<Node> first_node, shared_ptr<ReevaluationParameters> reevaluation_parameters) {
  real constraint_loglikelihood = log(1.0);
  real unconstraint_loglikelihood = log(1.0);
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
      
      real node_loglikelihood = current_node2->xrp->xrp->GetSampledLoglikelihood(got_arguments, current_node2->my_sampled_value);

      if (current_node2->xrp->xrp->IsRandomChoice()) {
        if (current_node2->constraint_times > 0) {
          constraint_loglikelihood += node_loglikelihood;
        } else {
          reevaluation_parameters->deleting_random_choices.insert(current_node2);
          unconstraint_loglikelihood += node_loglikelihood;
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
  return pair<real, real>(constraint_loglikelihood, unconstraint_loglikelihood);
}

void UnabsorbBranchProbability(shared_ptr<Node> first_node, shared_ptr<ReevaluationParameters> reevaluation_parameters) {
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
      real node_loglikelihood = current_node2->xrp->xrp->GetSampledLoglikelihood(got_arguments, current_node2->my_sampled_value);
      current_node2->xrp->xrp->Incorporate(got_arguments, current_node2->my_sampled_value);
      
      if (reevaluation_parameters != shared_ptr<ReevaluationParameters>()) {
        if (current_node2->xrp->xrp->IsRandomChoice()) {
          if (current_node2->constraint_times > 0) {
            reevaluation_parameters->__log_p_new += node_loglikelihood;
          } else {
            reevaluation_parameters->creating_random_choices.insert(current_node2);
            reevaluation_parameters->__log_p_new += node_loglikelihood;
            reevaluation_parameters->__log_q_from_old_to_new += node_loglikelihood;
          }
        }
      }
  
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
}

void Enumerate(shared_ptr<NodeXRPApplication> principal_node) {
  set< shared_ptr<VentureValue> > returning_set = principal_node->xrp->xrp->EnumeratingSupport();
  map<shared_ptr<VentureValue>, real> logprobabilities;
  map<shared_ptr<VentureValue>, shared_ptr< map<string, shared_ptr<VentureValue> > > > random_databases;

  assert(returning_set.empty() != true);

  while (returning_set.empty() == false) {
    shared_ptr<VentureValue> proposing_value = *(returning_set.begin());
    returning_set.erase(proposing_value);
    random_databases[proposing_value] = shared_ptr< map<string, shared_ptr<VentureValue> > >(new map<string, shared_ptr<VentureValue> >());
    MHProposalResults mh_proposal_results = MakeMHProposal(principal_node, proposing_value, random_databases[proposing_value], false);
    logprobabilities[proposing_value] = mh_proposal_results.logscore_PNew;
  }

  /*
  real max_exp = logprobabilities.begin()->second, sum = 0.0;
  for (map<shared_ptr<VentureValue>, real>::const_iterator iterator = logprobabilities.begin();
       iterator != logprobabilities.end();
       iterator++)
  {
    if (iterator->second > max_exp)
      max_exp = iterator->second;
  }
  for (map<shared_ptr<VentureValue>, real>::const_iterator iterator = logprobabilities.begin();
       iterator != logprobabilities.end();
       iterator++)
  {
    sum += exp(iterator->second - max_exp);
  }
  sum = log(sum) + max_exp;
  real log_random_value = log(gsl_ran_flat(random_generator, 0, 1));
  real log_accumulated = log(1.0);
  for (map<shared_ptr<VentureValue>, real>::iterator iterator = logprobabilities.begin();
       iterator != logprobabilities.end();
       iterator++)
  {
    iterator->second -= sum;
    log_accumulated += ;
  }*/
  real random_value = gsl_ran_flat(random_generator, 0, 1);
  real accumulated = 0.0;
  map<shared_ptr<VentureValue>, real>::iterator iterator_to_the_new_value;
  for (map<shared_ptr<VentureValue>, real>::iterator iterator = logprobabilities.begin();
       iterator != logprobabilities.end();
       iterator++)
  {
    accumulated += exp(iterator->second);
    iterator_to_the_new_value = iterator;
    if (accumulated > random_value) { break; }
  }
 
  {
    shared_ptr<VentureValue> proposing_value = iterator_to_the_new_value->first;
    MakeMHProposal(principal_node, proposing_value, random_databases[proposing_value], true);
  }
}
