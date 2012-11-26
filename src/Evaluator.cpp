
#include "Header.h"
#include "VentureValues.h"
#include "Analyzer.h"
#include "Evaluator.h"

shared_ptr<VentureValue> Evaluator(shared_ptr<NodeEvaluation> evaluation_node,
                                   shared_ptr<NodeEnvironment> environment) {
  return evaluation_node->Evaluate(environment);
}

void BindToEnvironment(shared_ptr<NodeEnvironment> target_environment,
                       shared_ptr<VentureSymbol> variable_name,
                       shared_ptr<VentureValue> binding_value) {
  target_environment->variables[variable_name->GetString()] = binding_value;
}

shared_ptr<VentureValue> LookupValue(shared_ptr<NodeEnvironment> environment,
                                     shared_ptr<VentureSymbol> variable_name) {
  return environment->variables[variable_name->GetString()];
}
