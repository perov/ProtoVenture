
#ifndef VENTURE___ANALYZER_H
#define VENTURE___ANALYZER_H

#include "Header.h"
#include "VentureValues.h"
#include "XRPCore.h"

enum NodeTypes { UNDEFINED_NODE, ENVIRONMENT, VARIABLE, UNDEFINED_EVALUATION_NODE, DIRECTIVE_ASSUME,
                 DIRECTIVE_PREDICT, DIRECTIVE_OBSERVE, SELF_EVALUATING, LAMBDA_CREATOR,
                 LOOKUP, APPLICATION_CALLER, XRP_APPLICATION, MEMOIZER};

string GetNodeTypeAsString(size_t node_type);

extern size_t DIRECTIVE_COUNTER;

struct ProposalInfo;

struct NodeDirectiveObserve;
struct EvaluationConfig {
  EvaluationConfig(bool we_are_in_proposal) : // Move to Analyzer.cpp?
    loglikelihood_p_constraint_score(0.0),
    loglikelihood_p_unconstraint_score(0.0),
    number_of_created_random_choices(0),
    we_are_in_proposal(we_are_in_proposal)
  {}

  shared_ptr<NodeDirectiveObserve> should_be_forced_for_OBSERVE;
  real loglikelihood_p_constraint_score;
  real loglikelihood_p_unconstraint_score;
  size_t number_of_created_random_choices;
  bool we_are_in_proposal;
};

struct Node : public VentureValue {
  Node();
  virtual VentureDataTypes GetType();
  virtual NodeTypes GetNodeType();
  virtual string GetUniqueID();
  virtual shared_ptr<ReevaluationResult> Reevaluate(shared_ptr<VentureValue>,
                                                    shared_ptr<Node>,
                                                    ReevaluationParameters&); // Should be in NodeEvaluation?
  virtual void GetChildren(queue< shared_ptr<Node> >& processing_queue);
  virtual bool WasEvaluated();
  virtual string GetContent();
  ~Node();

  virtual void DeleteNode();

  bool was_deleted;
  
  boost::mutex occupying_mutex;
  shared_ptr<ProposalInfo> occupying_proposal_info;
};

struct NodeVariable;

struct NodeEnvironment : public Node {
  NodeEnvironment(shared_ptr<NodeEnvironment> parent_environment);
  virtual NodeTypes GetNodeType();
  ~NodeEnvironment();

  weak_ptr<NodeEnvironment> parent_environment;
  map<string, shared_ptr<NodeVariable> > variables;
  vector< shared_ptr<NodeVariable> > local_variables;
  
  virtual void DeleteNode();
};

struct NodeVariable : public Node {
  NodeVariable(shared_ptr<NodeEnvironment> parent_environment, shared_ptr<VentureValue> value);
  virtual NodeTypes GetNodeType();
  shared_ptr<VentureValue> GetCurrentValue();
  virtual shared_ptr<ReevaluationResult> Reevaluate(shared_ptr<VentureValue>,
                                                                  shared_ptr<Node>,
                                                                  ReevaluationParameters&);
  ~NodeVariable();

  weak_ptr<NodeEnvironment> parent_environment;
  shared_ptr<VentureValue> value;
  shared_ptr<VentureValue> new_value;
  set< weak_ptr<Node> > output_references;
  
  virtual void DeleteNode();
};

struct NodeEvaluation : public Node {
  NodeEvaluation();
  virtual NodeTypes GetNodeType();
  virtual shared_ptr<NodeEvaluation> clone() const;
  virtual shared_ptr<VentureValue> Evaluate(shared_ptr<NodeEnvironment>, EvaluationConfig& evaluation_config);
  virtual shared_ptr<ReevaluationResult> Reevaluate(shared_ptr<VentureValue>,
                                                    shared_ptr<Node>,
                                                    ReevaluationParameters&);
  virtual bool WasEvaluated();
  ~NodeEvaluation();

  shared_ptr<NodeEnvironment> environment;
  weak_ptr<NodeEvaluation> parent;
  shared_ptr<NodeEvaluation> earlier_evaluation_nodes;
  bool evaluated;
  set< weak_ptr<Node> > output_references;
  vector<size_t> myorder;
  size_t last_child_order;
  
  virtual void DeleteNode();
};

const size_t REEVALUATION_PRIORITY__STANDARD = 10;
struct ReevaluationOrderComparer {
  bool operator()(const ReevaluationEntry& first, const ReevaluationEntry& second);
};

struct NodeDirectiveAssume : public NodeEvaluation {
  NodeDirectiveAssume(shared_ptr<VentureSymbol> name, shared_ptr<NodeEvaluation> expression);
  virtual NodeTypes GetNodeType();
  virtual shared_ptr<VentureValue> Evaluate(shared_ptr<NodeEnvironment>, EvaluationConfig& evaluation_config);
  virtual shared_ptr<ReevaluationResult> Reevaluate(shared_ptr<VentureValue>,
                                                    shared_ptr<Node>,
                                                    ReevaluationParameters&);
  virtual void GetChildren(queue< shared_ptr<Node> >& processing_queue);
  ~NodeDirectiveAssume();
  
  shared_ptr<VentureSymbol> name;
  shared_ptr<NodeEvaluation> expression;
  shared_ptr<VentureValue> my_value; // It should not be implemented in this way?
  shared_ptr<VentureValue> my_new_value; // It should not be implemented in this way?

  virtual void DeleteNode();
};

struct NodeDirectivePredict : public NodeEvaluation {
  NodeDirectivePredict(shared_ptr<NodeEvaluation> expression);
  virtual NodeTypes GetNodeType();
  virtual shared_ptr<VentureValue> Evaluate(shared_ptr<NodeEnvironment>, EvaluationConfig& evaluation_config);
  virtual shared_ptr<ReevaluationResult> Reevaluate(shared_ptr<VentureValue>,
                                                    shared_ptr<Node>,
                                                    ReevaluationParameters&);
  virtual void GetChildren(queue< shared_ptr<Node> >& processing_queue);
  ~NodeDirectivePredict();

  shared_ptr<NodeEvaluation> expression;
  shared_ptr<VentureValue> my_value; // It should not be implemented in this way?
  shared_ptr<VentureValue> my_new_value; // It should not be implemented in this way?

  virtual void DeleteNode();
};

struct NodeDirectiveObserve : public NodeEvaluation {
  NodeDirectiveObserve(shared_ptr<NodeEvaluation> expression, shared_ptr<VentureValue> observed_value);
  virtual NodeTypes GetNodeType();
  virtual shared_ptr<VentureValue> Evaluate(shared_ptr<NodeEnvironment>, EvaluationConfig& evaluation_config);
  virtual shared_ptr<ReevaluationResult> Reevaluate(shared_ptr<VentureValue>,
                                                    shared_ptr<Node>,
                                                    ReevaluationParameters&);
  virtual void GetChildren(queue< shared_ptr<Node> >& processing_queue);
  ~NodeDirectiveObserve();
  
  shared_ptr<NodeEvaluation> expression;
  shared_ptr<VentureValue> observed_value;

  virtual void DeleteNode();
};

struct NodeSelfEvaluating : public NodeEvaluation {
  NodeSelfEvaluating(shared_ptr<VentureValue> value);
  virtual NodeTypes GetNodeType();
  virtual shared_ptr<NodeEvaluation> clone() const;
  shared_ptr<VentureValue> Evaluate(shared_ptr<NodeEnvironment>, EvaluationConfig& evaluation_config);
  virtual void GetChildren(queue< shared_ptr<Node> >& processing_queue);
  // Using standard copy constructor.
  virtual string GetContent();
  ~NodeSelfEvaluating();
  
  shared_ptr<VentureValue> value;

  virtual void DeleteNode();
};

struct NodeLambdaCreator : public NodeEvaluation {
  NodeLambdaCreator(shared_ptr<VentureList> arguments, shared_ptr<NodeEvaluation> expressions);
  virtual NodeTypes GetNodeType();
  virtual shared_ptr<NodeEvaluation> clone() const;
  virtual shared_ptr<VentureValue> Evaluate(shared_ptr<NodeEnvironment>, EvaluationConfig& evaluation_config);
  virtual void GetChildren(queue< shared_ptr<Node> >& processing_queue);
  // Using standard copy constructor.
  ~NodeLambdaCreator();
  
  shared_ptr<VentureList> arguments;
  shared_ptr<NodeEvaluation> expressions;

  virtual void DeleteNode();
};

struct NodeLookup : public NodeEvaluation {
  NodeLookup(shared_ptr<VentureSymbol> symbol);
  virtual NodeTypes GetNodeType();
  virtual shared_ptr<NodeEvaluation> clone() const;
  virtual shared_ptr<VentureValue> Evaluate(shared_ptr<NodeEnvironment>, EvaluationConfig& evaluation_config);
  virtual shared_ptr<ReevaluationResult> Reevaluate(shared_ptr<VentureValue>,
                                                    shared_ptr<Node>,
                                                    ReevaluationParameters&);
  virtual void GetChildren(queue< shared_ptr<Node> >& processing_queue);
  virtual string GetContent();
  ~NodeLookup();

  shared_ptr<VentureSymbol> symbol;
  shared_ptr<NodeVariable> where_lookuped;

  virtual void DeleteNode();
};

struct NodeApplicationCaller : public NodeEvaluation {
  NodeApplicationCaller(shared_ptr<NodeEvaluation> application_operator,
                        vector< shared_ptr<NodeEvaluation> >& application_operands);
  virtual NodeTypes GetNodeType();
  virtual shared_ptr<NodeEvaluation> clone() const;
  virtual shared_ptr<VentureValue> Evaluate(shared_ptr<NodeEnvironment>, EvaluationConfig& evaluation_config);
  virtual shared_ptr<ReevaluationResult> Reevaluate(shared_ptr<VentureValue>,
                                                    shared_ptr<Node>,
                                                    ReevaluationParameters&);
  virtual void GetChildren(queue< shared_ptr<Node> >& processing_queue);
  ~NodeApplicationCaller();
  
  shared_ptr<VentureValue> saved_evaluated_operator;
  shared_ptr<VentureValue> proposing_evaluated_operator;
  shared_ptr<NodeEvaluation> application_operator;
  vector< shared_ptr<NodeEvaluation> > application_operands;
  shared_ptr<NodeEvaluation> application_node;
  shared_ptr<NodeEvaluation> new_application_node;
  
  virtual void DeleteNode();
};

struct NodeXRPApplication : public NodeEvaluation {
  NodeXRPApplication(shared_ptr<VentureXRP> xrp);
  virtual NodeTypes GetNodeType();
  // It should not have clone() method?
  virtual shared_ptr<VentureValue> Evaluate(shared_ptr<NodeEnvironment>, EvaluationConfig& evaluation_config);
  virtual shared_ptr<ReevaluationResult> Reevaluate(shared_ptr<VentureValue>,
                                                    shared_ptr<Node>,
                                                    ReevaluationParameters&);
  virtual void GetChildren(queue< shared_ptr<Node> >& processing_queue);
  ~NodeXRPApplication();
  
  shared_ptr<VentureXRP> xrp;
  shared_ptr<VentureValue> sampled_value;
  shared_ptr<VentureValue> new_sampled_value;
  
  bool frozen; // For mem.
  shared_ptr<NodeDirectiveObserve> was_forced_for;
  
  virtual void DeleteNode();
};

shared_ptr<NodeEvaluation> AnalyzeExpression(shared_ptr<VentureValue>);

vector< shared_ptr<VentureValue> >
GetArgumentsFromEnvironment(shared_ptr<NodeEnvironment>,
                            shared_ptr<NodeEvaluation>,
                            bool);

shared_ptr<VentureValue>
EvaluateApplication(shared_ptr<VentureValue> evaluated_operator,
                    shared_ptr<NodeEnvironment> local_environment,
                    size_t number_of_operands,
                    shared_ptr<NodeEvaluation>& application_node,
                    shared_ptr<NodeApplicationCaller> application_caller_ptr,
                    EvaluationConfig& evaluation_config);

void ApplyToMeAndAllMyChildren(shared_ptr<Node>,
                               void (*f)(shared_ptr<Node>));

void DrawGraphDuringMH(shared_ptr<Node> first_node, stack< shared_ptr<Node> >& touched_nodes);

#endif
