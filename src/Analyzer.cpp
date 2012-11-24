
#include "Header.h"
#include "VentureParser.h"
#include "Analyzer.h"

VentureValue* GetFirst(VentureList* list) {
  if (list->GetType() == NIL) {
    return list;
  } else {
    return list->car;
  }
}

VentureList* GetNext(VentureList* list) {
  if (list->GetType() == NIL) {
    return list;
  } else {
    return list->cdr;
  }
}

VentureValue* GetNth(VentureList* list, size_t position) { // Enumeration from "1"!
  for (size_t index = 1; index < position; index++) {
    list = GetNext(list);
  }
  return GetFirst(list);
}

NodeEvaluation* AnalyzeExpressions(VentureList* expressions) {
  if (expressions->GetType() == NIL) {
    throw std::exception("Expecting at least 1 expression."); // Allow it as in Clojure?
  }

  NodeEvaluation* last_expression = 0;
  do {
    NodeEvaluation* current_expression = AnalyzeExpression(GetFirst(expressions));
    expressions = GetNext(expressions);
    if (last_expression != 0) {
      current_expression->earlier_evaluation_nodes = last_expression;
    }
    last_expression = current_expression;
  } while (expressions->GetType() == NIL);
  return last_expression;
}

NodeEvaluation* AnalyzeExpression(VentureValue* expression) {
  if (expression->GetType() == INTEGER ||
      expression->GetType() == REAL ||
      expression->GetType() == BOOLEAN) {
    return new NodeSelfEvaluating(expression);
  } else if (expression->GetType() == SYMBOL) {
    return new NodeLookup(ToVentureSymbol(expression));
  } else if (expression->GetType() == LIST) { // What about NIL? Change this comparison by IsList()?..
                                              //                                        (which returns
                                              //                                         'true' also for
                                              //                                         NIL)
    VentureList* expression_list = ToVentureList(expression);
    if (CompareValue(ToVentureSymbol(GetFirst(expression_list)), "quote")) { // FIXME: without matching case?
      return new NodeSelfEvaluating(GetNext(expression_list));
    } else if (CompareValue(ToVentureSymbol(GetFirst(expression_list)), "lambda")) {
      return new NodeLambdaCreator(ToVentureList(GetNth(expression_list, 2)),
                                   AnalyzeExpressions(GetNext(GetNext(expression_list))));
    } else {
      VentureList* arguments_expressions = ToVentureList(GetFirst(GetNext(expression_list)));
      vector<NodeEvaluation*> arguments;
      while (arguments_expressions->GetType() != NIL) {
        arguments.push_back(AnalyzeExpression(arguments_expressions));
        arguments_expressions = arguments_expressions->cdr;
      }
      return new NodeApplicationCaller(AnalyzeExpression(GetFirst(expression_list)),
                                       arguments);
    }
  } else {
    throw std::exception("Undefined expression.");
  }
}

NodeEvaluation* AnalyzeDirective(VentureValue* directive) {
  VentureList* list = ToVentureList(directive);
  if (CompareValue(ToVentureSymbol(GetFirst(list)), "ASSUME")) { // FIXME: without matching case?
    return new NodeDirectiveAssume(ToVentureSymbol(GetNth(list, 2)), AnalyzeExpression(GetNth(list, 3)));
  } else if (CompareValue(ToVentureSymbol(GetFirst(list)), "PREDICT")) {
    return new NodeDirectivePredict(AnalyzeExpression(GetNth(list, 2)));
  } else if (CompareValue(ToVentureSymbol(GetFirst(list)), "ASSUME")) {
    // In the future something like this: AssertLiteralValue(GetNth(list, 3)); ?
    return new NodeDirectiveObserve(AnalyzeExpression(GetNth(list, 2)), GetNth(list, 3));
  } else {
    throw std::exception("Undefined directive.");
  }
}
