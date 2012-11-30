
#include "Header.h"
#include "VentureParser.h"
#include "Analyzer.h"
#include "Evaluator.h"
#include "XRP.h"

shared_ptr<NodeEvaluation> AnalyzeExpressions(shared_ptr<VentureList> expressions) {
  shared_ptr<NodeEvaluation> last_expression = shared_ptr<NodeEvaluation>();
  do {
    shared_ptr<NodeEvaluation> current_expression = AnalyzeExpression(GetFirst(expressions));
    expressions = GetNext(expressions);
    if (last_expression != shared_ptr<NodeEvaluation>()) {
      current_expression->earlier_evaluation_nodes = last_expression;
    }
    last_expression = current_expression;
  } while (expressions->GetType() != NIL);
  return last_expression;
}

shared_ptr<NodeEvaluation> AnalyzeExpression(shared_ptr<VentureValue> expression) {
  if (expression->GetType() == BOOLEAN ||
      expression->GetType() == COUNT ||
      expression->GetType() == REAL ||
      expression->GetType() == PROBABILITY ||
      expression->GetType() == ATOM ||
      expression->GetType() == SIMPLEXPOINT ||
      expression->GetType() == SMOOTHEDCONTINUOUS ||
      expression->GetType() == NIL) // As in Scheme and Lisp?.
  {
    return shared_ptr<NodeEvaluation>(new NodeSelfEvaluating(expression));
  } else if (expression->GetType() == SYMBOL) {
    return shared_ptr<NodeEvaluation>(new NodeLookup(ToVentureSymbol(expression)));
  } else if (expression->GetType() == LIST) { // What about NIL? Change this comparison by IsList()?..
                                              //                                        (which returns
                                              //                                         'true' also for
                                              //                                         NIL)
    shared_ptr<VentureList> expression_list = ToVentureList(expression);
    if (CompareValue(GetFirst(expression_list), shared_ptr<VentureValue>(new VentureSymbol("quote")))) { // FIXME: without matching case?
      return shared_ptr<NodeEvaluation>(new NodeSelfEvaluating(GetNext(expression_list)));
    } else if (CompareValue(GetFirst(expression_list), shared_ptr<VentureValue>(new VentureSymbol("if")))) {
      // (if predicate consequent alternative) is sugar for
      // ( (condition-ERP predicate (lambda () consequent) (lambda () alternative)) )

      shared_ptr<VentureList> condition_ERP_call =
        shared_ptr<VentureList>(new VentureList(
          shared_ptr<VentureValue>(new VentureSymbol("condition-ERP"))));
      AddToList(condition_ERP_call,
                GetNth(expression_list, 2));

      shared_ptr<VentureList> lambda_consequent =
        shared_ptr<VentureList>(new VentureList(
          shared_ptr<VentureValue>(new VentureSymbol("lambda"))));
      AddToList(lambda_consequent,
                NIL_INSTANCE);
      AddToList(lambda_consequent,
                GetNth(expression_list, 3));
      AddToList(condition_ERP_call,
                lambda_consequent);

      shared_ptr<VentureList> lambda_alternative =
        shared_ptr<VentureList>(new VentureList(
          shared_ptr<VentureValue>(new VentureSymbol("lambda"))));
      AddToList(lambda_alternative,
                NIL_INSTANCE);
      AddToList(lambda_alternative,
                GetNth(expression_list, 4));
      AddToList(condition_ERP_call,
                lambda_alternative);
      
      shared_ptr<VentureList> outer_list =
        shared_ptr<VentureList>(new VentureList(condition_ERP_call));

      return AnalyzeExpression(outer_list);
    } else if (CompareValue(GetFirst(expression_list), shared_ptr<VentureValue>(new VentureSymbol("lambda")))) {
      return shared_ptr<NodeEvaluation>(
               new NodeLambdaCreator(ToVentureList(GetNth(expression_list, 2)),
                                     AnalyzeExpressions(GetNext(GetNext(expression_list)))));
    } else {
      shared_ptr<VentureList> arguments_expressions = ToVentureList(GetNext(expression_list));
      vector< shared_ptr<NodeEvaluation> > arguments;
      while (GetFirst(arguments_expressions)->GetType() != NIL) {
        arguments.push_back(AnalyzeExpression(GetFirst(arguments_expressions)));
        arguments_expressions = arguments_expressions->cdr;
      }
      return shared_ptr<NodeEvaluation>(
               new NodeApplicationCaller(AnalyzeExpression(GetFirst(expression_list)),
                                         arguments));
    }
  } else {
    throw std::exception("Undefined expression.");
  }
}

shared_ptr<NodeEvaluation> AnalyzeDirective(shared_ptr<VentureValue> directive) {
  shared_ptr<VentureList> list = ToVentureList(directive);
  if (CompareValue(GetFirst(list), shared_ptr<VentureValue>(new VentureSymbol("ASSUME")))) { // FIXME: without matching case?
    return shared_ptr<NodeEvaluation>(
             new NodeDirectiveAssume(ToVentureSymbol(GetNth(list, 2)), AnalyzeExpression(GetNth(list, 3))));
  } else if (CompareValue(GetFirst(list), shared_ptr<VentureValue>(new VentureSymbol("PREDICT")))) {
    return  shared_ptr<NodeEvaluation>(
              new NodeDirectivePredict(AnalyzeExpression(GetNth(list, 2))));
  } else if (CompareValue(GetFirst(list), shared_ptr<VentureValue>(new VentureSymbol("ASSUME")))) {
    // In the future something like this: AssertLiteralValue(GetNth(list, 3)); ?
    return shared_ptr<NodeEvaluation>(
             new NodeDirectiveObserve(AnalyzeExpression(GetNth(list, 2)), GetNth(list, 3)));
  } else {
    throw std::exception("Undefined directive.");
  }
}

shared_ptr<VentureValue>
NodeEvaluation::Evaluate(shared_ptr<NodeEnvironment> environment) { // It seems we do not need this function, do we?
  throw std::exception("It should not happen.");
}

shared_ptr<VentureValue>
NodeDirectiveAssume::Evaluate(shared_ptr<NodeEnvironment> environment) {
  this->output_references.insert(
    BindToEnvironment(environment,
                      this->name,
                      Evaluator(this->expression,
                                environment,
                                dynamic_pointer_cast<Node>(this->shared_from_this()),
                                dynamic_pointer_cast<NodeEvaluation>(this->shared_from_this()))));
  return NIL_INSTANCE; // Something wiser?
}

shared_ptr<VentureValue>
NodeDirectivePredict::Evaluate(shared_ptr<NodeEnvironment> environment) {
  return Evaluator(this->expression,
                   environment,
                   dynamic_pointer_cast<Node>(this->shared_from_this()),
                   dynamic_pointer_cast<NodeEvaluation>(this->shared_from_this()));
}

shared_ptr<VentureValue>
NodeDirectiveObserve::Evaluate(shared_ptr<NodeEnvironment> environment) {
  throw std::exception("TBA.");
}

shared_ptr<VentureValue>
NodeSelfEvaluating::Evaluate(shared_ptr<NodeEnvironment> environment) {
  return this->value;
}

shared_ptr<VentureValue>
NodeLambdaCreator::Evaluate(shared_ptr<NodeEnvironment> environment) {
  return shared_ptr<VentureValue>(new VentureLambda(this->arguments, this->expressions, environment));
}

shared_ptr<VentureValue>
NodeLookup::Evaluate(shared_ptr<NodeEnvironment> environment) {
  return LookupValue(environment, this->symbol, dynamic_pointer_cast<NodeEvaluation>(this->shared_from_this()));
}

shared_ptr<VentureValue>
EvaluateApplication(shared_ptr<VentureValue> evaluated_operator,
                    shared_ptr<NodeEnvironment> local_environment,
                    size_t number_of_operands,
                    shared_ptr<NodeEvaluation>& application_node,
                    shared_ptr<NodeApplicationCaller> application_caller_ptr) {
  if (evaluated_operator->GetType() == LAMBDA) {
    shared_ptr<VentureList> enumerate_arguments = ToVentureType<VentureLambda>(evaluated_operator)->formal_arguments;
    size_t index = 0;
    while (enumerate_arguments->GetType() != NIL) {
      if (index >= number_of_operands) {
        throw std::exception("Too few arguments have been passed to lambda.");
      }
      BindVariableToEnvironment(local_environment,
                                ToVentureType<VentureSymbol>(enumerate_arguments->car),
                                local_environment->local_variables[index]);
      index++;
      enumerate_arguments = GetNext(enumerate_arguments);
    }
    if (index != number_of_operands) {
      throw std::exception("Too many arguments have been passed to lambda.");
    }
    
    application_node =
      shared_ptr<NodeEvaluation>(ToVentureType<VentureLambda>(evaluated_operator)->expressions->clone());

    return Evaluator(application_node,
                      local_environment,
                      dynamic_pointer_cast<Node>(application_caller_ptr),
                      dynamic_pointer_cast<NodeEvaluation>(application_caller_ptr));
  } else if (evaluated_operator->GetType() == XRP_REFERENCE) {
    application_node = shared_ptr<NodeEvaluation>(new NodeXRPApplication(
      dynamic_pointer_cast<VentureXRP>(evaluated_operator))); // Do not use ToVentureType(...)
                                                              // because we have checked
                                                              // the type in the IF condition.
    
    return Evaluator(application_node,
                     local_environment,
                     dynamic_pointer_cast<Node>(application_caller_ptr),
                     dynamic_pointer_cast<NodeEvaluation>(application_caller_ptr));
  } else {
    throw std::exception((string("Attempt to apply neither LAMBDA nor XRP (") + boost::lexical_cast<string>(evaluated_operator->GetType()) + string(")")).c_str());
  }
}

shared_ptr<VentureValue>
NodeApplicationCaller::Evaluate(shared_ptr<NodeEnvironment> environment) {
  shared_ptr<VentureValue> evaluated_operator = Evaluator(application_operator,
                                                          environment,
                                                          dynamic_pointer_cast<Node>(this->shared_from_this()),
                                                          dynamic_pointer_cast<NodeEvaluation>(this->shared_from_this()));
  this->saved_evaluated_operator = evaluated_operator;
  
  shared_ptr<NodeEnvironment> local_environment =
    shared_ptr<NodeEnvironment>(new NodeEnvironment(environment));
  for (size_t index = 0; index < application_operands.size(); index++) {
    shared_ptr<VentureValue> binding_value =
      Evaluator(application_operands[index],
                environment,
                shared_ptr<Node>(),
                dynamic_pointer_cast<NodeEvaluation>(this->shared_from_this()));
    shared_ptr<NodeVariable> new_variable_node =
      shared_ptr<NodeVariable>(new NodeVariable(local_environment, binding_value));
    local_environment->local_variables.push_back(new_variable_node);
    application_operands[index]->output_references.insert(new_variable_node);
  }

  return
    EvaluateApplication(evaluated_operator,
                        local_environment,
                        application_operands.size(),
                        application_node,
                        dynamic_pointer_cast<NodeApplicationCaller>(this->shared_from_this()));
}

vector< shared_ptr<VentureValue> >
GetArgumentsFromEnvironment(shared_ptr<NodeEnvironment> environment,
                            shared_ptr<NodeEvaluation> caller,
                            bool old_values = false) {
  vector< shared_ptr<VentureValue> > arguments;
  for (size_t index = 0; index < environment->local_variables.size(); index++) {
    arguments.push_back(LookupValue(environment,
                                    index,
                                    caller,
                                    old_values));
  }
  return arguments;
}

shared_ptr<VentureValue>
NodeXRPApplication::Evaluate(shared_ptr<NodeEnvironment> environment) {
  if (this->xrp->xrp->IsRandomChoice() == true) {
    random_choices.insert(dynamic_pointer_cast<NodeXRPApplication>(this->shared_from_this()));
  }

  return this->xrp->xrp->Sample(GetArgumentsFromEnvironment(environment,
                                  dynamic_pointer_cast<NodeEvaluation>(this->shared_from_this())),
                                dynamic_pointer_cast<NodeXRPApplication>(this->shared_from_this()));
}

shared_ptr<ReevaluationResult>
NodeXRPApplication::Reevaluate(shared_ptr<VentureValue> passing_value,
                               shared_ptr<Node> sender,
                               ReevaluationParameters& reevaluation_parameters) {
  shared_ptr<RescorerResamplerResult> result =
    this->xrp->xrp->RescorerResampler(
      GetArgumentsFromEnvironment(environment,
        dynamic_pointer_cast<NodeEvaluation>(this->shared_from_this()), true),
      GetArgumentsFromEnvironment(environment,
        dynamic_pointer_cast<NodeEvaluation>(this->shared_from_this()), false),
      dynamic_pointer_cast<NodeXRPApplication>(this->shared_from_this()),
      (sender == shared_ptr<Node>()));
  if (result->new_value == shared_ptr<VentureValue>()) { // Rescored.
    //cout << "Adding: " << result->new_loglikelihood << " - " << result->old_loglikelihood << endl;
    reevaluation_parameters.loglikelihood_changes += result->new_loglikelihood;
    reevaluation_parameters.loglikelihood_changes -= result->old_loglikelihood;
    return shared_ptr<ReevaluationResult>(
      new ReevaluationResult(shared_ptr<VentureValue>(), false));
  } else { // Resampled.
    return shared_ptr<ReevaluationResult>(
      new ReevaluationResult(result->new_value, true));
  }
}

shared_ptr<ReevaluationResult>
NodeEvaluation::Reevaluate(shared_ptr<VentureValue> passing_value,
                           shared_ptr<Node> sender,
                           ReevaluationParameters& reevaluation_parameters) {
  throw std::exception(("This node does not have the Reevaluation function (node type " + boost::lexical_cast<string>(this->GetNodeType()) + ").").c_str());
}

shared_ptr<ReevaluationResult>
Node::Reevaluate(shared_ptr<VentureValue> passing_value,
                 shared_ptr<Node> sender,
                 ReevaluationParameters& reevaluation_parameters) {
  throw std::exception(("This node does not have the Reevaluation function (node type " + boost::lexical_cast<string>(this->GetNodeType()) + ").").c_str());
}

shared_ptr<ReevaluationResult>
NodeLookup::Reevaluate(shared_ptr<VentureValue> passing_value,
                       shared_ptr<Node> sender,
                       ReevaluationParameters& reevaluation_parameters) {
  // Just passing up:
  return shared_ptr<ReevaluationResult>(
    new ReevaluationResult(passing_value, true));
}

shared_ptr<ReevaluationResult>
NodeApplicationCaller::Reevaluate(shared_ptr<VentureValue> passing_value,
                                  shared_ptr<Node> sender,
                                  ReevaluationParameters& reevaluation_parameters) {
  if (sender == this->application_node->shared_from_this()) {
    // Just passing up:
    return shared_ptr<ReevaluationResult>(
      new ReevaluationResult(passing_value, true));
  } else if (sender == this->application_operator->shared_from_this()) {
    this->proposing_evaluated_operator = passing_value;
    shared_ptr<VentureValue> evaluated_operator = passing_value;

    shared_ptr<NodeEnvironment> local_environment =
      shared_ptr<NodeEnvironment>(new NodeEnvironment(environment));
    for (size_t index = 0; index < application_operands.size(); index++) {
      shared_ptr<VentureValue> binding_value =
        this->application_node->environment->local_variables[index]->value;
      shared_ptr<NodeVariable> new_variable_node =
        shared_ptr<NodeVariable>(new NodeVariable(local_environment, binding_value));
      local_environment->local_variables.push_back(new_variable_node);
      application_operands[index]->output_references.insert(new_variable_node);
    }

    shared_ptr<VentureValue> new_value =
      EvaluateApplication(passing_value,
                          local_environment,
                          application_operands.size(),
                          new_application_node,
                          dynamic_pointer_cast<NodeApplicationCaller>(this->shared_from_this()));
    
    reevaluation_parameters.random_choices_delta += CalculateNumberOfRandomChoices(new_application_node);
    reevaluation_parameters.random_choices_delta -= CalculateNumberOfRandomChoices(application_node);
    
    return shared_ptr<ReevaluationResult>(
      new ReevaluationResult(new_value, true));
  } else {
    throw std::exception("NodeApplicationCaller::Reevaluate: strange sender.");
  }
}

shared_ptr<ReevaluationResult>
NodeDirectiveAssume::Reevaluate(shared_ptr<VentureValue> passing_value,
                                shared_ptr<Node> sender,
                                ReevaluationParameters& reevaluation_parameters) {
  // Just passing up:
  return shared_ptr<ReevaluationResult>(
    new ReevaluationResult(passing_value, true));
}

shared_ptr<ReevaluationResult>
NodeDirectivePredict::Reevaluate(shared_ptr<VentureValue> passing_value,
                                 shared_ptr<Node> sender,
                                 ReevaluationParameters& reevaluation_parameters) {
  // Just passing up:
  return shared_ptr<ReevaluationResult>(
    new ReevaluationResult(passing_value, true));
}

void ApplyToMeAndAllMyChildren(shared_ptr<Node> first_node,
                               void (*f)(shared_ptr<Node>)) {
  size_t number_of_random_choices = 0;
  queue< shared_ptr<Node> > processing_queue;
  processing_queue.push(first_node);
  while (!processing_queue.empty()) {
    processing_queue.front()->GetChildren(processing_queue);
    (*f)(dynamic_pointer_cast<NodeEvaluation>(processing_queue.front()->shared_from_this()));
    processing_queue.pop();
  }
}

size_t CalculateNumberOfRandomChoices(shared_ptr<Node> first_node) {
  size_t number_of_random_choices = 0;
  queue< shared_ptr<Node> > processing_queue;
  processing_queue.push(first_node);
  while (!processing_queue.empty()) {
    processing_queue.front()->GetChildren(processing_queue);
    if (processing_queue.front()->GetNodeType() == XRP_APPLICATION &&
          dynamic_pointer_cast<NodeXRPApplication>(processing_queue.front())->xrp->xrp->IsRandomChoice()) {
      number_of_random_choices++;
    }
    processing_queue.pop();
  }
  return number_of_random_choices;
};
