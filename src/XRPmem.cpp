
#include "XRPmem.h"

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
    // cout << "*** Creating the mem node " << mem_table_key << endl;
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

    shared_ptr<VentureValue> result =
      Evaluator(this->mem_table[mem_table_key].application_caller_node,
                caller->environment, // FIXME: Is it okay?
                caller, // FIXME: Is it okay?
                caller);
    
    (*(this->mem_table.find(mem_table_key))).second.result
      = shared_ptr<NodeVariable>(new NodeVariable(shared_ptr<NodeEnvironment>(), result));
    this->mem_table[mem_table_key].application_caller_node
      ->output_references.insert((*(this->mem_table.find(mem_table_key))).second.result);
  } else {
    // cout << "*** Restoring the mem node " << mem_table_key << endl;
    // Adding the output reference link by hand.
    this->mem_table[mem_table_key].application_caller_node
      ->output_references.insert(caller);
  }
  XRP__memoizer_map_element& mem_table_element =
    (*(this->mem_table.find(mem_table_key))).second;

  mem_table_element.hidden_uses++;
  //cout << "Returning (from mem): " << mem_table_element.result->value->GetString() << endl;
  return mem_table_element.result->GetCurrentValue();
}

void XRP__memoized_procedure::Unsampler(vector< shared_ptr<VentureValue> >& old_arguments, shared_ptr<NodeXRPApplication> caller) {
  string mem_table_key =
    XRP__memoized_procedure__MakeMapKeyFromArguments(old_arguments);
  if (this->mem_table.count(mem_table_key) == 0) {
    throw std::runtime_error("Cannot find the necessary key in the mem table.");
  }
  XRP__memoizer_map_element& mem_table_element =
    (*(this->mem_table.find(mem_table_key))).second;
  mem_table_element.application_caller_node->output_references.erase(caller);
  if (caller->frozen == true) {
    throw std::runtime_error("We should be never frozen at this time!"); // FIXME: What about the multi-thread version?
  }
  if (mem_table_element.hidden_uses == 0) {
    throw std::runtime_error("(1) Cannot do 'mem_table_element.hidden_uses--'.");
  }
  mem_table_element.hidden_uses--;
  if (mem_table_element.hidden_uses == 0 && mem_table_element.active_uses == 0) {
    // cout << "*** Deleting the mem node " << mem_table_key << endl;
    ApplyToMeAndAllMyChildren(mem_table_element.application_caller_node, DeleteNode);
    this->mem_table.erase(mem_table_key);
  }
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

// FREEZING BRANCHES.

void XRP__memoized_procedure__Freeze
 (shared_ptr<XRP__memoized_procedure> xrp_object,
  vector< shared_ptr<VentureValue> >& arguments,
  shared_ptr<NodeXRPApplication> caller)
{
  if (caller->frozen == true) {
    throw std::runtime_error("We should not be frozen yet at this time!"); // It is not right for the parallel implementation?
  }
  string mem_table_key = XRP__memoized_procedure__MakeMapKeyFromArguments(arguments);
  if (xrp_object->mem_table.count(mem_table_key) == 0) {
    throw std::runtime_error("Cannot find the necessary key in the mem table.");
  }
  XRP__memoizer_map_element& mem_table_element =
    (*(xrp_object->mem_table.find(mem_table_key))).second;
  mem_table_element.frozen_elements++;
  caller->frozen = true;
}

void XRP__memoized_procedure__Unfreeze
 (shared_ptr<XRP__memoized_procedure> xrp_object,
  vector< shared_ptr<VentureValue> >& arguments,
  shared_ptr<NodeXRPApplication> caller)
{
  if (caller->frozen == false) {
    throw std::runtime_error("We should be already frozen at this time!"); // It is not right for the parallel implementation?
  }
  string mem_table_key = XRP__memoized_procedure__MakeMapKeyFromArguments(arguments);
  if (xrp_object->mem_table.count(mem_table_key) == 0) {
    throw std::runtime_error("Cannot find the necessary key in the mem table.");
  }
  XRP__memoizer_map_element& mem_table_element =
    (*(xrp_object->mem_table.find(mem_table_key))).second;
  mem_table_element.frozen_elements--;
  caller->frozen = false;
}

void FreezeBranch(shared_ptr<Node> first_node, ReevaluationParameters& reevaluation_parameters) {
  queue< shared_ptr<Node> > processing_queue;
  processing_queue.push(first_node);
  while (!processing_queue.empty()) {
    processing_queue.front()->GetChildren(processing_queue);
    if (processing_queue.front()->GetNodeType() == XRP_APPLICATION &&
          dynamic_pointer_cast<NodeXRPApplication>(processing_queue.front())->xrp->xrp->GetName() == "XRP__memoized_procedure") {
      // It is stupid that we check here by GetName().
      // It means that the Freeze(...) should be implemented in another way...

      // FIXME: GetArgumentsFromEnvironment should be called without adding lookup references!
      vector< shared_ptr<VentureValue> > got_arguments = GetArgumentsFromEnvironment(dynamic_pointer_cast<NodeXRPApplication>(processing_queue.front())->environment, // Not efficient?
                                      dynamic_pointer_cast<NodeEvaluation>(processing_queue.front()), // Are we sure that we have not deleted yet lookup links?
                                      true); // FIXME: we should be sure that we are receiving old arguments!

      string mem_table_key = XRP__memoized_procedure__MakeMapKeyFromArguments(got_arguments);
      XRP__memoizer_map_element& mem_table_element =
        (*(dynamic_pointer_cast<XRP__memoized_procedure>(dynamic_pointer_cast<NodeXRPApplication>(processing_queue.front())->xrp->xrp)->mem_table.find(mem_table_key))).second;

      XRP__memoized_procedure__Freeze(
        dynamic_pointer_cast<XRP__memoized_procedure>(dynamic_pointer_cast<NodeXRPApplication>(processing_queue.front())->xrp->xrp),
        got_arguments,
        dynamic_pointer_cast<NodeXRPApplication>(processing_queue.front())
      );

      if (mem_table_element.hidden_uses +
            mem_table_element.active_uses ==
            mem_table_element.frozen_elements) {
        reevaluation_parameters.random_choices_delta -= CalculateNumberOfRandomChoices(mem_table_element.application_caller_node);
        processing_queue.push(mem_table_element.application_caller_node);
      }
    }
    processing_queue.pop();
  }
};

void UnfreezeBranch(shared_ptr<Node> first_node) {
  queue< shared_ptr<Node> > processing_queue;
  processing_queue.push(first_node);
  while (!processing_queue.empty()) {
    processing_queue.front()->GetChildren(processing_queue);
    if (processing_queue.front()->GetNodeType() == XRP_APPLICATION &&
          dynamic_pointer_cast<NodeXRPApplication>(processing_queue.front())->xrp->xrp->GetName() == "XRP__memoized_procedure") {
      // It is stupid that we check here by GetName().
      // It means that the Freeze(...) should be implemented in another way...

      if (dynamic_pointer_cast<NodeXRPApplication>(processing_queue.front())->frozen == false) {
        throw std::runtime_error("We should be frozen at this time!"); // Not right for the multi-thread version?
      }

      // FIXME: GetArgumentsFromEnvironment should be called without adding lookup references!
      vector< shared_ptr<VentureValue> > got_arguments = GetArgumentsFromEnvironment(dynamic_pointer_cast<NodeXRPApplication>(processing_queue.front())->environment, // Not efficient?
                                      dynamic_pointer_cast<NodeEvaluation>(processing_queue.front()), // Are we sure that we have not deleted yet lookup links?
                                      true); // FIXME: we should be sure that we are receiving old arguments!

      string mem_table_key = XRP__memoized_procedure__MakeMapKeyFromArguments(got_arguments);
      XRP__memoizer_map_element& mem_table_element =
        (*(dynamic_pointer_cast<XRP__memoized_procedure>(dynamic_pointer_cast<NodeXRPApplication>(processing_queue.front())->xrp->xrp)->mem_table.find(mem_table_key))).second;

      XRP__memoized_procedure__Unfreeze(
        dynamic_pointer_cast<XRP__memoized_procedure>(dynamic_pointer_cast<NodeXRPApplication>(processing_queue.front())->xrp->xrp),
        got_arguments,
        dynamic_pointer_cast<NodeXRPApplication>(processing_queue.front())
      );

      if (mem_table_element.hidden_uses +
            mem_table_element.active_uses ==
            mem_table_element.frozen_elements + 1) {
        processing_queue.push(mem_table_element.application_caller_node);
      }
    }
    processing_queue.pop();
  }
};
