
#ifndef VENTURE___ANALYZER_H
#define VENTURE___ANALYZER_H

#include "Header.h"
#include "VentureValues.h"

shared_ptr<VentureValue> GetFirst(shared_ptr<VentureList>);

shared_ptr<VentureList> GetNext(shared_ptr<VentureList>);

shared_ptr<VentureValue> GetNth(shared_ptr<VentureList>, size_t);

enum NodeTypes { UNDEFINED_NODE, ENVIRONMENT, VARIABLE, UNDEFINED_EVALUATION_NODE, DIRECTIVE_ASSUME,
                 DIRECTIVE_PREDICT, DIRECTIVE_OBSERVE, SELF_EVALUATING, LAMBDA_CREATOR,
                 LOOKUP, APPLICATION_CALLER};

extern size_t NEXT_UNIQUE_ID;

struct Node : public VentureValue {
  Node()
  {
    unique_id = ++NEXT_UNIQUE_ID; // FIXME: make transaction free!
  }
  VentureDataTypes GetType() { return NODE; }
  virtual NodeTypes GetNodeType() { return UNDEFINED_NODE; }

  size_t unique_id;
};

struct NodeVariable;

struct NodeEnvironment : public Node {
  NodeEnvironment(shared_ptr<NodeEnvironment> parent_environment) : parent_environment(parent_environment) {}
  NodeTypes GetNodeType() { return ENVIRONMENT; }
  shared_ptr<NodeEnvironment> parent_environment;
  map<string, shared_ptr<NodeVariable> > variables;
};

struct NodeVariable : public Node {
  NodeVariable(shared_ptr<NodeEnvironment> parent_environment, shared_ptr<VentureValue> value)
    : parent_environment(parent_environment), value(value)
  {}
  NodeTypes GetNodeType() { return VARIABLE; }
  shared_ptr<NodeEnvironment> parent_environment;
  shared_ptr<VentureValue> value;
};

struct NodeEvaluation : public Node {
  NodeEvaluation()
    : environment(shared_ptr<NodeEnvironment>()),
      earlier_evaluation_nodes(shared_ptr<NodeEvaluation>()),
      evaluated(false)
  {}
  virtual NodeTypes GetNodeType() { return UNDEFINED_EVALUATION_NODE; }
  virtual shared_ptr<VentureValue> Evaluate(shared_ptr<NodeEnvironment>);
  shared_ptr<NodeEnvironment> environment;
  shared_ptr<NodeEvaluation> earlier_evaluation_nodes;
  bool evaluated;
};

struct NodeDirectiveAssume : public NodeEvaluation {
  virtual NodeTypes GetNodeType() { return DIRECTIVE_ASSUME; }
  NodeDirectiveAssume(shared_ptr<VentureSymbol> name, shared_ptr<NodeEvaluation> expression)
    : name(name), expression(expression) {}
  shared_ptr<VentureValue> Evaluate(shared_ptr<NodeEnvironment>);
  
  shared_ptr<VentureSymbol> name;
  shared_ptr<NodeEvaluation> expression;
};

struct NodeDirectivePredict : public NodeEvaluation {
  virtual NodeTypes GetNodeType() { return DIRECTIVE_PREDICT; }
  NodeDirectivePredict(shared_ptr<NodeEvaluation> expression)
    : expression(expression) {}
  shared_ptr<VentureValue> Evaluate(shared_ptr<NodeEnvironment>);

  shared_ptr<NodeEvaluation> expression;
};

struct NodeDirectiveObserve : public NodeEvaluation {
  virtual NodeTypes GetNodeType() { return DIRECTIVE_ASSUME; }
  NodeDirectiveObserve(shared_ptr<NodeEvaluation> expression, shared_ptr<VentureValue> observed_value)
    : expression(expression), observed_value(observed_value) {}
  shared_ptr<VentureValue> Evaluate(shared_ptr<NodeEnvironment>);
  
  shared_ptr<NodeEvaluation> expression;
  shared_ptr<VentureValue> observed_value;
};

struct NodeSelfEvaluating : public NodeEvaluation {
  virtual NodeTypes GetNodeType() { return SELF_EVALUATING; }
  NodeSelfEvaluating(shared_ptr<VentureValue> value)
    : value(value) {}
  shared_ptr<VentureValue> Evaluate(shared_ptr<NodeEnvironment>);
  
  shared_ptr<VentureValue> value;
};

struct NodeLambdaCreator : public NodeEvaluation {
  virtual NodeTypes GetNodeType() { return LAMBDA_CREATOR; }
  NodeLambdaCreator(shared_ptr<VentureList> arguments, shared_ptr<NodeEvaluation> expressions)
    : arguments(arguments), expressions(expressions) {}
  shared_ptr<VentureValue> Evaluate(shared_ptr<NodeEnvironment>);
  
  shared_ptr<VentureList> arguments;
  shared_ptr<NodeEvaluation> expressions;
};

struct NodeLookup : public NodeEvaluation {
  virtual NodeTypes GetNodeType() { return LOOKUP; }
  NodeLookup(shared_ptr<VentureSymbol> symbol)
    : symbol(symbol) {}
  shared_ptr<VentureValue> Evaluate(shared_ptr<NodeEnvironment>);
  
  shared_ptr<VentureSymbol> symbol;
};

struct NodeApplicationCaller : public NodeEvaluation {
  virtual NodeTypes GetNodeType() { return APPLICATION_CALLER; }
  NodeApplicationCaller(shared_ptr<NodeEvaluation> application_operator,
                        vector< shared_ptr<NodeEvaluation> >& application_operands)
    : application_operator(application_operator),
      application_operands(application_operands),
      application_node(shared_ptr<NodeEvaluation>())
  {}
  shared_ptr<VentureValue> Evaluate(shared_ptr<NodeEnvironment>);
  
  shared_ptr<NodeEvaluation> application_operator;
  vector< shared_ptr<NodeEvaluation> > application_operands;
  shared_ptr<NodeEvaluation> application_node;
};

shared_ptr<NodeEvaluation> AnalyzeDirective(shared_ptr<VentureValue>);
shared_ptr<NodeEvaluation> AnalyzeExpression(shared_ptr<VentureValue>);

#endif
