
#ifndef VENTURE___ANALYZER_H
#define VENTURE___ANALYZER_H

#include "Header.h"
#include "VentureValues.h"

VentureValue* GetFirst(VentureList*);

VentureList* GetNext(VentureList*);

VentureValue* GetNth(VentureList*, size_t);

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
  NodeEnvironment(NodeEnvironment* parent_environment) : parent_environment(parent_environment) {}
  NodeTypes GetNodeType() { return ENVIRONMENT; }
  NodeEnvironment* parent_environment;
  map<string, NodeVariable*> variables;
};


struct NodeVariable : public Node {
  NodeVariable(NodeEnvironment* parent_environment, VentureValue* value)
    : parent_environment(parent_environment), value(value)
  {}
  NodeTypes GetNodeType() { return VARIABLE; }
  NodeEnvironment* parent_environment;
  VentureValue* value;
};

struct NodeEvaluation : public Node {
  NodeEvaluation()
    : environment(0),
      earlier_evaluation_nodes(0),
      evaluated(false)
  {}
  virtual NodeTypes GetNodeType() { return UNDEFINED_EVALUATION_NODE; }
  NodeEnvironment* environment;
  NodeEvaluation* earlier_evaluation_nodes;
  bool evaluated;
};

struct NodeDirectiveAssume : public NodeEvaluation {
  virtual NodeTypes GetNodeType() { return DIRECTIVE_ASSUME; }
  NodeDirectiveAssume(VentureSymbol* name, NodeEvaluation* expression)
    : name(name), expression(expression) {}
  
  VentureSymbol* name;
  NodeEvaluation* expression;
};

struct NodeDirectivePredict : public NodeEvaluation {
  virtual NodeTypes GetNodeType() { return DIRECTIVE_PREDICT; }
  NodeDirectivePredict(NodeEvaluation* expression)
    : expression(expression) {}

  NodeEvaluation* expression;
};

struct NodeDirectiveObserve : public NodeEvaluation {
  virtual NodeTypes GetNodeType() { return DIRECTIVE_ASSUME; }
  NodeDirectiveObserve(NodeEvaluation* expression, VentureValue* observed_value)
    : expression(expression), observed_value(observed_value) {}
  
  NodeEvaluation* expression;
  VentureValue* observed_value;
};

struct NodeSelfEvaluating : public NodeEvaluation {
  virtual NodeTypes GetNodeType() { return SELF_EVALUATING; }
  NodeSelfEvaluating(VentureValue* value)
    : value(value) {}
  
  VentureValue* value;
};

struct NodeLambdaCreator : public NodeEvaluation {
  virtual NodeTypes GetNodeType() { return LAMBDA_CREATOR; }
  NodeLambdaCreator(VentureList* arguments, NodeEvaluation* expressions)
    : arguments(arguments), expressions(expressions) {}
  
  VentureList* arguments;
  NodeEvaluation* expressions;
};

struct NodeLookup : public NodeEvaluation {
  virtual NodeTypes GetNodeType() { return LOOKUP; }
  NodeLookup(VentureSymbol* symbol)
    : symbol(symbol) {}
  
  VentureSymbol* symbol;
};

struct NodeApplicationCaller : public NodeEvaluation {
  virtual NodeTypes GetNodeType() { return APPLICATION_CALLER; }
  NodeApplicationCaller(NodeEvaluation* application_operator,
                        vector<NodeEvaluation*>& application_operands)
    : application_operator(application_operator),
      application_operands(application_operands),
      application_node(0)
  {}
  
  NodeEvaluation* application_operator;
  vector<NodeEvaluation*> application_operands;
  NodeEvaluation* application_node;
};

NodeEvaluation* AnalyzeDirective(VentureValue*);
NodeEvaluation* AnalyzeExpression(VentureValue*);

#endif
