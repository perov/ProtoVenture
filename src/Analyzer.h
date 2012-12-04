
#ifndef VENTURE___ANALYZER_H
#define VENTURE___ANALYZER_H

#include "Header.h"
#include "VentureValues.h"
#include "XRP.h"

enum NodeTypes { UNDEFINED_NODE, ENVIRONMENT, VARIABLE, UNDEFINED_EVALUATION_NODE, DIRECTIVE_ASSUME,
                 DIRECTIVE_PREDICT, DIRECTIVE_OBSERVE, SELF_EVALUATING, LAMBDA_CREATOR,
                 LOOKUP, APPLICATION_CALLER, XRP_APPLICATION};

string GetNodeTypeAsString(size_t node_type);

extern size_t DIRECTIVE_COUNTER;

struct Node : public VentureValue {
  Node::Node();
  virtual VentureDataTypes Node::GetType();
  virtual NodeTypes Node::GetNodeType();
  virtual string Node::GetUniqueID();
  virtual shared_ptr<ReevaluationResult> Node::Reevaluate(shared_ptr<VentureValue>,
                                                    shared_ptr<Node>,
                                                    ReevaluationParameters&); // Should be in NodeEvaluation?
  virtual void Node::GetChildren(queue< shared_ptr<Node> >& processing_queue);
  virtual bool Node::WasEvaluated();
  virtual string Node::GetContent();
  ~Node();

  virtual void Node::DeleteNode();
};

struct NodeVariable;

struct NodeEnvironment : public Node {
  NodeEnvironment::NodeEnvironment(shared_ptr<NodeEnvironment> parent_environment);
  virtual NodeTypes NodeEnvironment::GetNodeType();
  ~NodeEnvironment();

  shared_ptr<NodeEnvironment> parent_environment;
  map<string, shared_ptr<NodeVariable> > variables;
  vector< shared_ptr<NodeVariable> > local_variables;
  
  virtual void NodeEnvironment::DeleteNode();
};

struct NodeVariable : public Node {
  NodeVariable::NodeVariable(shared_ptr<NodeEnvironment> parent_environment, shared_ptr<VentureValue> value);
  virtual NodeTypes NodeVariable::GetNodeType();
  ~NodeVariable();

  shared_ptr<NodeEnvironment> parent_environment;
  shared_ptr<VentureValue> value;
  shared_ptr<VentureValue> new_value;
  set< shared_ptr<Node> > output_references;
  
  virtual void NodeVariable::DeleteNode();
};

struct NodeEvaluation : public Node {
  NodeEvaluation::NodeEvaluation();
  virtual NodeTypes NodeEvaluation::GetNodeType();
  virtual shared_ptr<NodeEvaluation> NodeEvaluation::clone() const;
  virtual shared_ptr<VentureValue> NodeEvaluation::Evaluate(shared_ptr<NodeEnvironment>);
  virtual shared_ptr<ReevaluationResult> NodeEvaluation::Reevaluate(shared_ptr<VentureValue>,
                                                    shared_ptr<Node>,
                                                    ReevaluationParameters&);
  virtual bool NodeEvaluation::WasEvaluated();
  ~NodeEvaluation();

  shared_ptr<NodeEnvironment> environment;
  shared_ptr<NodeEvaluation> parent;
  shared_ptr<NodeEvaluation> earlier_evaluation_nodes;
  bool evaluated;
  set< shared_ptr<Node> > output_references;
  vector<size_t> myorder;
  size_t last_child_order;
  
  virtual void NodeEvaluation::DeleteNode();
};

struct ReevaluationOrderComparer {
  bool ReevaluationOrderComparer::operator()(const ReevaluationEntry& first, const ReevaluationEntry& second);
};

struct NodeDirectiveAssume : public NodeEvaluation {
  NodeDirectiveAssume::NodeDirectiveAssume(shared_ptr<VentureSymbol> name, shared_ptr<NodeEvaluation> expression);
  virtual NodeTypes NodeDirectiveAssume::GetNodeType();
  virtual shared_ptr<VentureValue> NodeDirectiveAssume::Evaluate(shared_ptr<NodeEnvironment>);
  virtual shared_ptr<ReevaluationResult> NodeDirectiveAssume::Reevaluate(shared_ptr<VentureValue>,
                                                    shared_ptr<Node>,
                                                    ReevaluationParameters&);
  virtual void NodeDirectiveAssume::GetChildren(queue< shared_ptr<Node> >& processing_queue);
  ~NodeDirectiveAssume();
  
  shared_ptr<VentureSymbol> name;
  shared_ptr<NodeEvaluation> expression;
  shared_ptr<VentureValue> my_value; // It should not be implemented in this way?

  virtual void NodeDirectiveAssume::DeleteNode();
};

struct NodeDirectivePredict : public NodeEvaluation {
  NodeDirectivePredict::NodeDirectivePredict(shared_ptr<NodeEvaluation> expression);
  virtual NodeTypes NodeDirectivePredict::GetNodeType();
  virtual shared_ptr<VentureValue> NodeDirectivePredict::Evaluate(shared_ptr<NodeEnvironment>);
  virtual shared_ptr<ReevaluationResult> NodeDirectivePredict::Reevaluate(shared_ptr<VentureValue>,
                                                    shared_ptr<Node>,
                                                    ReevaluationParameters&);
  virtual void NodeDirectivePredict::GetChildren(queue< shared_ptr<Node> >& processing_queue);
  ~NodeDirectivePredict();

  shared_ptr<NodeEvaluation> expression;
  shared_ptr<VentureValue> my_value; // It should not be implemented in this way?

  virtual void NodeDirectivePredict::DeleteNode();
};

struct NodeDirectiveObserve : public NodeEvaluation {
  NodeDirectiveObserve::NodeDirectiveObserve(shared_ptr<NodeEvaluation> expression, shared_ptr<VentureValue> observed_value);
  virtual NodeTypes NodeDirectiveObserve::GetNodeType();
  virtual shared_ptr<VentureValue> Evaluate(shared_ptr<NodeEnvironment>);
  //virtual shared_ptr<ReevaluationResult> NodeDirectiveObserve::Reevaluate(shared_ptr<VentureValue>,
  //                                                  shared_ptr<Node>,
  //                                                  ReevaluationParameters&);
  virtual void NodeDirectiveObserve::GetChildren(queue< shared_ptr<Node> >& processing_queue);
  ~NodeDirectiveObserve();
  
  shared_ptr<NodeEvaluation> expression;
  shared_ptr<VentureValue> observed_value;

  virtual void NodeDirectiveObserve::DeleteNode();
};

struct NodeSelfEvaluating : public NodeEvaluation {
  NodeSelfEvaluating::NodeSelfEvaluating(shared_ptr<VentureValue> value);
  virtual NodeTypes NodeSelfEvaluating::GetNodeType();
  virtual shared_ptr<NodeEvaluation> NodeSelfEvaluating::clone() const;
  shared_ptr<VentureValue> NodeSelfEvaluating::Evaluate(shared_ptr<NodeEnvironment>);
  virtual void NodeSelfEvaluating::GetChildren(queue< shared_ptr<Node> >& processing_queue);
  // Using standard copy constructor.
  virtual string NodeSelfEvaluating::GetContent();
  ~NodeSelfEvaluating();
  
  shared_ptr<VentureValue> value;

  virtual void NodeSelfEvaluating::DeleteNode();
};

struct NodeLambdaCreator : public NodeEvaluation {
  NodeLambdaCreator::NodeLambdaCreator(shared_ptr<VentureList> arguments, shared_ptr<NodeEvaluation> expressions);
  virtual NodeTypes NodeLambdaCreator::GetNodeType();
  virtual shared_ptr<NodeEvaluation> NodeLambdaCreator::clone() const;
  virtual shared_ptr<VentureValue> NodeLambdaCreator::Evaluate(shared_ptr<NodeEnvironment>);
  virtual void NodeLambdaCreator::GetChildren(queue< shared_ptr<Node> >& processing_queue);
  // Using standard copy constructor.
  ~NodeLambdaCreator();
  
  shared_ptr<VentureList> arguments;
  shared_ptr<NodeEvaluation> expressions;

  virtual void NodeLambdaCreator::DeleteNode();
};

struct NodeLookup : public NodeEvaluation {
  NodeLookup::NodeLookup(shared_ptr<VentureSymbol> symbol);
  virtual NodeTypes NodeLookup::GetNodeType();
  virtual shared_ptr<NodeEvaluation> NodeLookup::clone() const;
  virtual shared_ptr<VentureValue> NodeLookup::Evaluate(shared_ptr<NodeEnvironment>);
  virtual shared_ptr<ReevaluationResult> NodeLookup::Reevaluate(shared_ptr<VentureValue>,
                                                    shared_ptr<Node>,
                                                    ReevaluationParameters&);
  virtual void NodeLookup::GetChildren(queue< shared_ptr<Node> >& processing_queue);
  virtual string NodeLookup::GetContent();
  ~NodeLookup();

  shared_ptr<VentureSymbol> symbol;
  shared_ptr<NodeVariable> where_lookuped;

  virtual void NodeLookup::DeleteNode();
};

struct NodeApplicationCaller : public NodeEvaluation {
  NodeApplicationCaller::NodeApplicationCaller(shared_ptr<NodeEvaluation> application_operator,
                        vector< shared_ptr<NodeEvaluation> >& application_operands);
  virtual NodeTypes NodeApplicationCaller::GetNodeType();
  virtual shared_ptr<NodeEvaluation> NodeApplicationCaller::clone() const;
  virtual shared_ptr<VentureValue> NodeApplicationCaller::Evaluate(shared_ptr<NodeEnvironment>);
  virtual shared_ptr<ReevaluationResult> NodeApplicationCaller::Reevaluate(shared_ptr<VentureValue>,
                                                    shared_ptr<Node>,
                                                    ReevaluationParameters&);
  virtual void NodeApplicationCaller::GetChildren(queue< shared_ptr<Node> >& processing_queue);
  ~NodeApplicationCaller();
  
  shared_ptr<VentureValue> saved_evaluated_operator;
  shared_ptr<VentureValue> proposing_evaluated_operator;
  shared_ptr<NodeEvaluation> application_operator;
  vector< shared_ptr<NodeEvaluation> > application_operands;
  shared_ptr<NodeEvaluation> application_node;
  shared_ptr<NodeEvaluation> new_application_node;
  
  virtual void NodeApplicationCaller::DeleteNode();
};

struct NodeXRPApplication : public NodeEvaluation {
  NodeXRPApplication::NodeXRPApplication(shared_ptr<VentureXRP> xrp);
  virtual NodeTypes NodeXRPApplication::GetNodeType();
  // It should not have clone() method?
  virtual shared_ptr<VentureValue> NodeXRPApplication::Evaluate(shared_ptr<NodeEnvironment>);
  virtual shared_ptr<ReevaluationResult> NodeXRPApplication::Reevaluate(shared_ptr<VentureValue>,
                                                    shared_ptr<Node>,
                                                    ReevaluationParameters&);
  virtual void NodeXRPApplication::GetChildren(queue< shared_ptr<Node> >& processing_queue);
  ~NodeXRPApplication();
  
  shared_ptr<VentureXRP> xrp;
  shared_ptr<VentureValue> sampled_value;
  shared_ptr<VentureValue> new_sampled_value;

  virtual void NodeXRPApplication::DeleteNode();
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

void DrawGraph(shared_ptr<Node> first_node);

size_t CalculateNumberOfRandomChoices(shared_ptr<Node>);

#endif
