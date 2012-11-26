
#include "Header.h"
#include "VentureParser.h"
#include "Analyzer.h"
#include "Evaluator.h"

shared_ptr<VentureValue> GetFirst(shared_ptr<VentureList> list) {
  if (list->GetType() == NIL) {
    return list;
  } else {
    return list->car;
  }
}

shared_ptr<VentureList> GetNext(shared_ptr<VentureList> list) {
  if (list->GetType() == NIL) {
    return list;
  } else {
    return list->cdr;
  }
}

shared_ptr<VentureValue> GetNth(shared_ptr<VentureList> list, size_t position) { // Enumeration from "1"!
  for (size_t index = 1; index < position; index++) {
    list = GetNext(list);
  }
  return GetFirst(list);
}

shared_ptr<NodeEvaluation> AnalyzeExpressions(shared_ptr<VentureList> expressions) {
  if (expressions->GetType() == NIL) {
    throw std::exception("Expecting at least 1 expression."); // Allow it as in Clojure?
  }

  shared_ptr<NodeEvaluation> last_expression = shared_ptr<NodeEvaluation>();
  do {
    shared_ptr<NodeEvaluation> current_expression = AnalyzeExpression(GetFirst(expressions));
    expressions = GetNext(expressions);
    if (last_expression != shared_ptr<NodeEvaluation>()) {
      current_expression->earlier_evaluation_nodes = last_expression;
    }
    last_expression = current_expression;
  } while (expressions->GetType() == NIL);
  return last_expression;
}

shared_ptr<NodeEvaluation> AnalyzeExpression(shared_ptr<VentureValue> expression) {
  if (expression->GetType() == INTEGER ||
      expression->GetType() == REAL ||
      expression->GetType() == BOOLEAN) {
    return shared_ptr<NodeEvaluation>(new NodeSelfEvaluating(expression));
  } else if (expression->GetType() == SYMBOL) {
    return shared_ptr<NodeEvaluation>(new NodeLookup(ToVentureSymbol(expression)));
  } else if (expression->GetType() == LIST) { // What about NIL? Change this comparison by IsList()?..
                                              //                                        (which returns
                                              //                                         'true' also for
                                              //                                         NIL)
    shared_ptr<VentureList> expression_list = ToVentureList(expression);
    if (CompareValue(ToVentureSymbol(GetFirst(expression_list)), "quote")) { // FIXME: without matching case?
      return shared_ptr<NodeEvaluation>(new NodeSelfEvaluating(GetNext(expression_list)));
    } else if (CompareValue(ToVentureSymbol(GetFirst(expression_list)), "lambda")) {
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
  if (CompareValue(ToVentureSymbol(GetFirst(list)), "ASSUME")) { // FIXME: without matching case?
    return shared_ptr<NodeEvaluation>(
             new NodeDirectiveAssume(ToVentureSymbol(GetNth(list, 2)), AnalyzeExpression(GetNth(list, 3))));
  } else if (CompareValue(ToVentureSymbol(GetFirst(list)), "PREDICT")) {
    return  shared_ptr<NodeEvaluation>(
              new NodeDirectivePredict(AnalyzeExpression(GetNth(list, 2))));
  } else if (CompareValue(ToVentureSymbol(GetFirst(list)), "ASSUME")) {
    // In the future something like this: AssertLiteralValue(GetNth(list, 3)); ?
    return shared_ptr<NodeEvaluation>(
             new NodeDirectiveObserve(AnalyzeExpression(GetNth(list, 2)), GetNth(list, 3)));
  } else {
    throw std::exception("Undefined directive.");
  }
}

shared_ptr<VentureValue>
NodeDirectiveAssume::Evaluate(shared_ptr<NodeEnvironment> environment) {
  BindToEnvironment(environment, this->name, Evaluator(this->expression, environment));
  return NIL_INSTANCE; // Something wiser?
}

shared_ptr<VentureValue>
NodeDirectivePredict::Evaluate(shared_ptr<NodeEnvironment> environment) {
  return Evaluator(this->expression, environment);
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
  return LookupValue(environment, this->symbol);
}

shared_ptr<VentureValue>
NodeApplicationCaller::Evaluate(shared_ptr<NodeEnvironment> environment) {
  vector< shared_ptr<VentureValue> > arguments;
  for (vector< shared_ptr<NodeEvaluation> >::iterator iterator = this->application_operands.begin();
       iterator != this->application_operands.end();
       iterator++) {
    arguments.push_back(Evaluator(*iterator, environment));
  }
  shared_ptr<VentureValue> evaluated_operator = Evaluator(application_operator, environment);
  if (evaluated_operator->GetType() == LAMBDA) {
    throw std::exception("TBA.");
  } else if (evaluated_operator->GetType() == XRP) {
    shared_ptr<VentureXRP> xrp = dynamic_pointer_cast<VentureXRP>(evaluated_operator);
    return xrp->xrp->Sample(arguments);
  } else {
    throw std::exception("Attempt to apply neither LAMBDA nor XRP.");
  }
}
