
#ifndef VENTURE___EVALUATOR_H
#define VENTURE___EVALUATOR_H

#include "Header.h"
#include "VentureValues.h"
#include "Analyzer.h"

shared_ptr<VentureValue> Evaluator(shared_ptr<NodeEvaluation>,
                                   shared_ptr<NodeEnvironment>);

void BindToEnvironment(shared_ptr<NodeEnvironment>,
                       shared_ptr<VentureSymbol>,
                       shared_ptr<VentureValue>); // void?

shared_ptr<VentureValue> LookupValue(shared_ptr<NodeEnvironment>,
                                     shared_ptr<VentureSymbol>);

#endif
