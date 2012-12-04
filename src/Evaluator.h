
#ifndef VENTURE___EVALUATOR_H
#define VENTURE___EVALUATOR_H

#include "Header.h"
#include "VentureValues.h"
#include "Analyzer.h"

shared_ptr<VentureValue> Evaluator(shared_ptr<NodeEvaluation>,
                                   shared_ptr<NodeEnvironment>,
                                   shared_ptr<Node>,
                                   shared_ptr<NodeEvaluation>);

shared_ptr<Node> BindToEnvironment(shared_ptr<NodeEnvironment>,
                                   shared_ptr<VentureSymbol>,
                                   shared_ptr<VentureValue>);

void BindVariableToEnvironment(shared_ptr<NodeEnvironment>,
                               shared_ptr<VentureSymbol>,
                               shared_ptr<NodeVariable>);

shared_ptr<VentureValue> LookupValue(shared_ptr<NodeEnvironment>,
                                     shared_ptr<VentureSymbol>,
                                     shared_ptr<NodeEvaluation>,
                                     bool old_values);

shared_ptr<VentureValue> LookupValue(shared_ptr<NodeEnvironment>,
                                     size_t,
                                     shared_ptr<NodeEvaluation>,
                                     bool old_values);

#endif
