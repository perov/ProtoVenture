
#ifndef VENTURE___ANALYZER_H
#define VENTURE___ANALYZER_H

#include "Header.h"
#include "VentureValues.h"
#include "XRP.h"

enum NodeTypes { UNDEFINED_NODE, ENVIRONMENT, VARIABLE, UNDEFINED_EVALUATION_NODE, DIRECTIVE_ASSUME,
                 DIRECTIVE_PREDICT, DIRECTIVE_OBSERVE, SELF_EVALUATING, LAMBDA_CREATOR,
                 LOOKUP, APPLICATION_CALLER, XRP_APPLICATION};

extern size_t NEXT_UNIQUE_ID;
extern size_t DIRECTIVE_COUNTER;

struct Node : public VentureValue {
  Node()
  {
    unique_id = ++NEXT_UNIQUE_ID; // FIXME: make transaction free!
  }
  VentureDataTypes GetType() { return NODE; }
  virtual NodeTypes GetNodeType() { return UNDEFINED_NODE; }
  virtual shared_ptr<ReevaluationResult> Reevaluate(shared_ptr<VentureValue>,
                                                    shared_ptr<Node>,
                                                    ReevaluationParameters&);
  virtual void GetChildren(queue< shared_ptr<Node> >& processing_queue) {
    throw std::exception("Should not be called.");
  };

  size_t unique_id;
};

struct NodeVariable;

struct NodeEnvironment : public Node {
  NodeEnvironment(shared_ptr<NodeEnvironment> parent_environment) : parent_environment(parent_environment) {}
  NodeTypes GetNodeType() { return ENVIRONMENT; }
  shared_ptr<NodeEnvironment> parent_environment;
  map<string, shared_ptr<NodeVariable> > variables;
  vector< shared_ptr<NodeVariable> > local_variables;
};

struct NodeVariable : public Node {
  NodeVariable(shared_ptr<NodeEnvironment> parent_environment, shared_ptr<VentureValue> value)
    : parent_environment(parent_environment),
      value(value),
      new_value(shared_ptr<VentureValue>())
  {}
  NodeTypes GetNodeType() { return VARIABLE; }
  shared_ptr<NodeEnvironment> parent_environment;
  shared_ptr<VentureValue> value;
  shared_ptr<VentureValue> new_value;
  set< shared_ptr<Node> > output_references;
};

struct NodeEvaluation : public Node {
  NodeEvaluation()
    : environment(shared_ptr<NodeEnvironment>()),
      earlier_evaluation_nodes(shared_ptr<NodeEvaluation>()),
      evaluated(false),
      last_child_order(0),
      parent(shared_ptr<NodeEvaluation>())
  {}
  virtual NodeTypes GetNodeType() { return UNDEFINED_EVALUATION_NODE; }
  virtual NodeEvaluation* clone() const {
    // Using the "clone pattern" in order to clone the Node for the lambda application.
    // See details here: http://www.cplusplus.com/forum/articles/18757/
    return new NodeEvaluation(*this);
  }
  virtual shared_ptr<VentureValue> Evaluate(shared_ptr<NodeEnvironment>);
  virtual shared_ptr<ReevaluationResult> Reevaluate(shared_ptr<VentureValue>,
                                                    shared_ptr<Node>,
                                                    ReevaluationParameters&);
  shared_ptr<NodeEnvironment> environment;
  shared_ptr<NodeEvaluation> parent;
  shared_ptr<NodeEvaluation> earlier_evaluation_nodes;
  bool evaluated;
  set< shared_ptr<Node> > output_references;
  vector<size_t> myorder;
  size_t last_child_order;
};

struct ReevaluationOrderComparer {
  bool operator()(const ReevaluationEntry& first, const ReevaluationEntry& second) {
    if (first.reevaluation_node->myorder.size() == 0) {
      throw std::exception("The first node has not been evaluated yet!");
    }
    if (second.reevaluation_node->myorder.size() == 0) {
      throw std::exception("The second node has not been evaluated yet!");
    }
    for (size_t index = 0;; index++) {
      if (index >= first.reevaluation_node->myorder.size() &&
          index >= second.reevaluation_node->myorder.size()) {
        return false; // They are the same.
      } else {
        if (index >= first.reevaluation_node->myorder.size()) {
          return true; // The first should be reevaluated later than the second.
        } else {
          if (index >= second.reevaluation_node->myorder.size()) {
            return false; // The first should be reevaluated earlier than the second.
          } else {
            if (first.reevaluation_node->myorder[index] == second.reevaluation_node->myorder[index]) {
              continue;
            } else {
              return (first.reevaluation_node->myorder[index] > second.reevaluation_node->myorder[index]);
            }
          }
        }
      }
    }
  }
};

struct NodeDirectiveAssume : public NodeEvaluation {
  virtual NodeTypes GetNodeType() { return DIRECTIVE_ASSUME; }
  NodeDirectiveAssume(shared_ptr<VentureSymbol> name, shared_ptr<NodeEvaluation> expression)
    : name(name), expression(expression) {}
  shared_ptr<VentureValue> Evaluate(shared_ptr<NodeEnvironment>);
  virtual shared_ptr<ReevaluationResult> Reevaluate(shared_ptr<VentureValue>,
                                                    shared_ptr<Node>,
                                                    ReevaluationParameters&);
  virtual void GetChildren(queue< shared_ptr<Node> >& processing_queue) {
    if (earlier_evaluation_nodes != shared_ptr<NodeEvaluation>()) {
      processing_queue.push(earlier_evaluation_nodes);
    }
    processing_queue.push(expression);
  };
  
  shared_ptr<VentureSymbol> name;
  shared_ptr<NodeEvaluation> expression;
};

struct NodeDirectivePredict : public NodeEvaluation {
  virtual NodeTypes GetNodeType() { return DIRECTIVE_PREDICT; }
  NodeDirectivePredict(shared_ptr<NodeEvaluation> expression)
    : expression(expression) {}
  shared_ptr<VentureValue> Evaluate(shared_ptr<NodeEnvironment>);
  virtual shared_ptr<ReevaluationResult> Reevaluate(shared_ptr<VentureValue>,
                                                    shared_ptr<Node>,
                                                    ReevaluationParameters&);
  virtual void GetChildren(queue< shared_ptr<Node> >& processing_queue) {
    if (earlier_evaluation_nodes != shared_ptr<NodeEvaluation>()) {
      processing_queue.push(earlier_evaluation_nodes);
    }
    processing_queue.push(expression);
  };

  shared_ptr<NodeEvaluation> expression;
};

struct NodeDirectiveObserve : public NodeEvaluation {
  virtual NodeTypes GetNodeType() { return DIRECTIVE_ASSUME; }
  NodeDirectiveObserve(shared_ptr<NodeEvaluation> expression, shared_ptr<VentureValue> observed_value)
    : expression(expression), observed_value(observed_value) {}
  shared_ptr<VentureValue> Evaluate(shared_ptr<NodeEnvironment>);
  virtual void GetChildren(queue< shared_ptr<Node> >& processing_queue) {
    if (earlier_evaluation_nodes != shared_ptr<NodeEvaluation>()) {
      processing_queue.push(earlier_evaluation_nodes);
    }
    processing_queue.push(expression);
  };
  
  shared_ptr<NodeEvaluation> expression;
  shared_ptr<VentureValue> observed_value;
};

struct NodeSelfEvaluating : public NodeEvaluation {
  virtual NodeTypes GetNodeType() { return SELF_EVALUATING; }
  NodeSelfEvaluating(shared_ptr<VentureValue> value)
    : value(value) {}
  NodeSelfEvaluating* clone() const {
    return new NodeSelfEvaluating(*this);
  }
  shared_ptr<VentureValue> Evaluate(shared_ptr<NodeEnvironment>);
  virtual void GetChildren(queue< shared_ptr<Node> >& processing_queue) {
    if (earlier_evaluation_nodes != shared_ptr<NodeEvaluation>()) {
      processing_queue.push(earlier_evaluation_nodes);
    }
  };
  // Using standard copy constructor.
  
  shared_ptr<VentureValue> value;
};

struct NodeLambdaCreator : public NodeEvaluation {
  virtual NodeTypes GetNodeType() { return LAMBDA_CREATOR; }
  NodeLambdaCreator(shared_ptr<VentureList> arguments, shared_ptr<NodeEvaluation> expressions)
    : arguments(arguments), expressions(expressions) {

  }
  NodeLambdaCreator* clone() const {
    return new NodeLambdaCreator(*this);
  }
  shared_ptr<VentureValue> Evaluate(shared_ptr<NodeEnvironment>);
  virtual void GetChildren(queue< shared_ptr<Node> >& processing_queue) {
    if (earlier_evaluation_nodes != shared_ptr<NodeEvaluation>()) {
      processing_queue.push(earlier_evaluation_nodes);
    }
    processing_queue.push(expressions);
  };
  // Using standard copy constructor.
  
  shared_ptr<VentureList> arguments;
  shared_ptr<NodeEvaluation> expressions;
};

struct NodeLookup : public NodeEvaluation {
  virtual NodeTypes GetNodeType() { return LOOKUP; }
  NodeLookup(shared_ptr<VentureSymbol> symbol)
    : symbol(symbol) {}
  NodeLookup* clone() const {
    return new NodeLookup(*this);
  }
  shared_ptr<VentureValue> Evaluate(shared_ptr<NodeEnvironment>);
  virtual shared_ptr<ReevaluationResult> Reevaluate(shared_ptr<VentureValue>,
                                                    shared_ptr<Node>,
                                                    ReevaluationParameters&);
  virtual void GetChildren(queue< shared_ptr<Node> >& processing_queue) {
    if (earlier_evaluation_nodes != shared_ptr<NodeEvaluation>()) {
      processing_queue.push(earlier_evaluation_nodes);
    }
  };

  shared_ptr<VentureSymbol> symbol;
  shared_ptr<NodeVariable> where_lookuped;
};

struct NodeApplicationCaller : public NodeEvaluation {
  virtual NodeTypes GetNodeType() { return APPLICATION_CALLER; }
  NodeApplicationCaller(shared_ptr<NodeEvaluation> application_operator,
                        vector< shared_ptr<NodeEvaluation> >& application_operands)
    : saved_evaluated_operator(shared_ptr<VentureValue>()),
      proposing_evaluated_operator(shared_ptr<VentureValue>()),
      application_operator(application_operator),
      application_operands(application_operands),
      application_node(shared_ptr<NodeEvaluation>()),
      new_application_node(shared_ptr<NodeEvaluation>())
  {}
  NodeApplicationCaller* clone() const {
    NodeApplicationCaller* NodeApplicationCaller_new = new NodeApplicationCaller(*this);
    NodeApplicationCaller_new->application_operator =
      shared_ptr<NodeEvaluation>(application_operator->clone());
    for (size_t index = 0; index < application_operands.size(); index++) {
      NodeApplicationCaller_new->application_operands[index] =
        shared_ptr<NodeEvaluation>(application_operands[index]->clone());
    }
    return NodeApplicationCaller_new;
  }
  shared_ptr<VentureValue> Evaluate(shared_ptr<NodeEnvironment>);
  virtual shared_ptr<ReevaluationResult> Reevaluate(shared_ptr<VentureValue>,
                                                    shared_ptr<Node>,
                                                    ReevaluationParameters&);
  virtual void GetChildren(queue< shared_ptr<Node> >& processing_queue) {
    if (earlier_evaluation_nodes != shared_ptr<NodeEvaluation>()) {
      processing_queue.push(earlier_evaluation_nodes);
    }
    processing_queue.push(application_operator);
    for (size_t index = 0; index < application_operands.size(); index++) {
      processing_queue.push(application_operands[index]);
    }
    processing_queue.push(application_node);
  };
  
  shared_ptr<VentureValue> saved_evaluated_operator;
  shared_ptr<VentureValue> proposing_evaluated_operator;
  shared_ptr<NodeEvaluation> application_operator;
  vector< shared_ptr<NodeEvaluation> > application_operands;
  shared_ptr<NodeEvaluation> application_node;
  shared_ptr<NodeEvaluation> new_application_node;
};

struct NodeXRPApplication : public NodeEvaluation {
  virtual NodeTypes GetNodeType() { return XRP_APPLICATION; }
  NodeXRPApplication(shared_ptr<VentureXRP> xrp)
    : xrp(xrp),
      sampled_value(shared_ptr<VentureValue>()),
      new_sampled_value(shared_ptr<VentureValue>())
  {}
  // It should not have clone() method?
  shared_ptr<VentureValue> Evaluate(shared_ptr<NodeEnvironment>);
  virtual shared_ptr<ReevaluationResult> Reevaluate(shared_ptr<VentureValue>,
                                                    shared_ptr<Node>,
                                                    ReevaluationParameters&);
  virtual void GetChildren(queue< shared_ptr<Node> >& processing_queue) {
    if (earlier_evaluation_nodes != shared_ptr<NodeEvaluation>()) {
      processing_queue.push(earlier_evaluation_nodes);
    }
  };
  
  shared_ptr<VentureXRP> xrp;
  shared_ptr<VentureValue> sampled_value;
  shared_ptr<VentureValue> new_sampled_value;
};

shared_ptr<NodeEvaluation> AnalyzeDirective(shared_ptr<VentureValue>);
shared_ptr<NodeEvaluation> AnalyzeExpression(shared_ptr<VentureValue>);

vector< shared_ptr<VentureValue> >
GetArgumentsFromEnvironment(shared_ptr<NodeEnvironment>,
                            shared_ptr<NodeEvaluation>,
                            bool);

shared_ptr<VentureValue>
EvaluateApplication(shared_ptr<VentureValue>,
                    shared_ptr<NodeEnvironment>,
                    size_t,
                    shared_ptr<NodeEvaluation>&,
                    shared_ptr<NodeApplicationCaller>);

void ApplyToMeAndAllMyChildren(shared_ptr<Node>,
                               void (*f)(shared_ptr<Node>));

size_t CalculateNumberOfRandomChoices(shared_ptr<Node>);

#endif
