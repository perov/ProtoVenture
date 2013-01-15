
#ifndef VENTURE___EVALUATOR_H
#define VENTURE___EVALUATOR_H

#include "Header.h"
#include "VentureValues.h"
#include "Analyzer.h"

shared_ptr<VentureValue> Evaluator(shared_ptr<NodeEvaluation> evaluation_node,
                                   shared_ptr<NodeEnvironment> environment,
                                   shared_ptr<Node> output_reference_target,
                                   shared_ptr<NodeEvaluation> caller,
                                   EvaluationConfig& evaluation_config);

shared_ptr<Node> BindToEnvironment(shared_ptr<NodeEnvironment> target_environment,
                                   shared_ptr<VentureSymbol> variable_name,
                                   shared_ptr<VentureValue> binding_value,
                                   shared_ptr<NodeEvaluation> binding_node = shared_ptr<NodeEvaluation>());

void BindVariableToEnvironment(shared_ptr<NodeEnvironment> target_environment,
                               shared_ptr<VentureSymbol> variable_name,
                               shared_ptr<NodeVariable> binding_variable);

shared_ptr<VentureValue> LookupValue(shared_ptr<NodeEnvironment> environment,
                                     shared_ptr<VentureSymbol> variable_name,
                                     shared_ptr<NodeEvaluation> lookuper,
                                     bool old_values);

shared_ptr<VentureValue> LookupValue(shared_ptr<NodeEnvironment> environment,
                                     size_t index,
                                     shared_ptr<NodeEvaluation> lookuper,
                                     bool old_values);

bool ForceExpressionValue(shared_ptr<Node> node, shared_ptr<VentureValue> desired_value, ReevaluationParameters& reevaluation_parameters);

void UnforceExpressionValue(shared_ptr<Node> node);

shared_ptr<VentureValue> GetBranchValue(shared_ptr<Node> node);

#endif
