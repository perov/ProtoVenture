
#include "Header.h"
#include "VentureValues.h"
#include "Analyzer.h"
#include "Evaluator.h"

shared_ptr<VentureValue> Evaluator(shared_ptr<NodeEvaluation> evaluation_node,
                                   shared_ptr<NodeEnvironment> environment,
                                   shared_ptr<Node> output_reference_target,
                                   shared_ptr<NodeEvaluation> caller) {
  if (caller == shared_ptr<NodeEvaluation>()) { // It is directive.
    DIRECTIVE_COUNTER++;
    evaluation_node->myorder.push_back(DIRECTIVE_COUNTER);
  } else {
    evaluation_node->myorder = caller->myorder;
    caller->last_child_order++;
    evaluation_node->myorder.push_back(caller->last_child_order);
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
              dynamic_pointer_cast<NodeEvaluation>(evaluation_node->shared_from_this()));
  }

  evaluation_node->evaluated = true; // Not too early?
  return evaluation_node->Evaluate(environment);
}

shared_ptr<Node> BindToEnvironment(shared_ptr<NodeEnvironment> target_environment,
                                  shared_ptr<VentureSymbol> variable_name,
                                  shared_ptr<VentureValue> binding_value) {
  target_environment->variables[variable_name->GetString()] =
    shared_ptr<NodeVariable>(new NodeVariable(target_environment, binding_value));
  return target_environment->variables[variable_name->GetString()];
}

void BindVariableToEnvironment(shared_ptr<NodeEnvironment> target_environment,
                               shared_ptr<VentureSymbol> variable_name,
                               shared_ptr<NodeVariable> binding_variable) {
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
      if (lookuper->GetNodeType() == LOOKUP) {
        dynamic_pointer_cast<NodeLookup>(lookuper)->where_lookuped =
          inspecting_environment->variables[variable_name->GetString()];
      }
      inspecting_environment->variables[variable_name->GetString()]->output_references.insert(lookuper);
      if (inspecting_environment->variables[variable_name->GetString()]->new_value == shared_ptr<VentureValue>() ||
          old_values == true) {
        return inspecting_environment->variables[variable_name->GetString()]->value;
      } else {
        return inspecting_environment->variables[variable_name->GetString()]->new_value;
      }
    } else {
      inspecting_environment = inspecting_environment->parent_environment;
    }
  }
  throw std::exception(("Unbound variable: " + variable_name->GetString()).c_str());
}

shared_ptr<VentureValue> LookupValue(shared_ptr<NodeEnvironment> environment,
                                     size_t index,
                                     shared_ptr<NodeEvaluation> lookuper,
                                     bool old_values) { // = false, see the header
  if (index >= environment->local_variables.size()) {
    throw std::exception("LookupValue: out of bound.");
  }
  if (lookuper->GetNodeType() == LOOKUP) {
    dynamic_pointer_cast<NodeLookup>(lookuper)->where_lookuped =
      environment->local_variables[index];
  }
  environment->local_variables[index]->output_references.insert(lookuper);
  if (environment->local_variables[index]->new_value == shared_ptr<VentureValue>() ||
      old_values == true) {
    return environment->local_variables[index]->value;
  } else {
    return environment->local_variables[index]->new_value;
  }
}
