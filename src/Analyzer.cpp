
#include "Header.h"
#include "VentureParser.h"
#include "Analyzer.h"
#include "Evaluator.h"
#include "XRP.h"
#include "RIPL.h"

string GetNodeTypeAsString(size_t node_type) {
  switch (node_type) {
  case UNDEFINED_NODE:
    return "UNDEFINED_NODE";
  case ENVIRONMENT:
    return "ENVIRONMENT";
  case VARIABLE:
    return "VARIABLE";
  case UNDEFINED_EVALUATION_NODE:
    return "UNDEFINED_EVALUATION_NODE";
  case DIRECTIVE_ASSUME:
    return "DIRECTIVE_ASSUME";
  case DIRECTIVE_PREDICT:
    return "DIRECTIVE_PREDICT";
  case DIRECTIVE_OBSERVE:
    return "DIRECTIVE_OBSERVE";
  case SELF_EVALUATING:
    return "SELF_EVALUATING";
  case LAMBDA_CREATOR:
    return "LAMBDA_CREATOR";
  case LOOKUP:
    return "LOOKUP";
  case APPLICATION_CALLER:
    return "APPLICATION_CALLER";
  case XRP_APPLICATION:
    return "XRP_APPLICATION";
  default:
    throw std::runtime_error("Undefined node type.");
  }
}

string Node::GetContent() {
  return "";
}
string NodeSelfEvaluating::GetContent() {
  return this->value->GetString();
}
string NodeLookup::GetContent() {
  return this->symbol->symbol;
}

string Node::GetUniqueID() {
  return boost::lexical_cast<string>(this);
}

Node::Node() {
  //unique_id = ++NEXT_UNIQUE_ID; // FIXME: make transaction free!
}

// Destructors
/*
Node::~Node() { cout << "Deleting: Node" << endl; }
NodeEnvironment::~NodeEnvironment() { cout << "Deleting: NodeEnvironment" << endl; }
NodeVariable::~NodeVariable() { cout << "Deleting: NodeVariable" << endl; }
NodeEvaluation::~NodeEvaluation() { cout << "Deleting: NodeEvaluation" << endl; }
NodeDirectiveAssume::~NodeDirectiveAssume() { cout << "Deleting: NodeDirectiveAssume" << endl; }
NodeDirectivePredict::~NodeDirectivePredict() { cout << "Deleting: NodeDirectivePredict" << endl; }
NodeDirectiveObserve::~NodeDirectiveObserve() { cout << "Deleting: NodeDirectiveObserve" << endl; }
NodeSelfEvaluating::~NodeSelfEvaluating() { cout << "Deleting: NodeSelfEvaluating" << endl; }
NodeLambdaCreator::~NodeLambdaCreator() { cout << "Deleting: NodeLambdaCreator" << endl; }
NodeLookup::~NodeLookup() { cout << "Deleting: NodeLookup" << endl; }
NodeApplicationCaller::~NodeApplicationCaller() { cout << "Deleting: NodeApplicationCaller" << endl; }
NodeXRPApplication::~NodeXRPApplication() { cout << "Deleting: NodeXRPApplication" << endl; }
*/
Node::~Node() {}
NodeEnvironment::~NodeEnvironment() {}
NodeVariable::~NodeVariable() {}
NodeEvaluation::~NodeEvaluation() {}
NodeDirectiveAssume::~NodeDirectiveAssume() {}
NodeDirectivePredict::~NodeDirectivePredict() {}
NodeDirectiveObserve::~NodeDirectiveObserve() {}
NodeSelfEvaluating::~NodeSelfEvaluating() {}
NodeLambdaCreator::~NodeLambdaCreator() {}
NodeLookup::~NodeLookup() {}
NodeApplicationCaller::~NodeApplicationCaller() {}
NodeXRPApplication::~NodeXRPApplication() {}

VentureDataTypes Node::GetType() { return NODE; }
NodeTypes Node::GetNodeType() { return UNDEFINED_NODE; }
void Node::GetChildren(queue< shared_ptr<Node> >& processing_queue) {
  throw std::runtime_error("Should not be called.");
};

void Node::DeleteNode() {}



NodeEnvironment::NodeEnvironment(shared_ptr<NodeEnvironment> parent_environment)
  : parent_environment(parent_environment) {}
NodeTypes NodeEnvironment::GetNodeType() { return ENVIRONMENT; }

NodeVariable::NodeVariable(shared_ptr<NodeEnvironment> parent_environment, shared_ptr<VentureValue> value)
  : parent_environment(parent_environment),
    value(value),
    new_value(shared_ptr<VentureValue>())
{}
NodeTypes NodeVariable::GetNodeType() { return VARIABLE; }

void NodeVariable::DeleteNode() {
  parent_environment = shared_ptr<NodeEnvironment>();
  output_references.clear();
}

NodeEvaluation::NodeEvaluation()
  : environment(shared_ptr<NodeEnvironment>()),
    earlier_evaluation_nodes(shared_ptr<NodeEvaluation>()),
    evaluated(false),
    last_child_order(0),
    parent(shared_ptr<NodeEvaluation>())
{}
NodeTypes NodeEvaluation::GetNodeType() { return UNDEFINED_EVALUATION_NODE; }
shared_ptr<NodeEvaluation> NodeEvaluation::clone() const {
  // Using the "clone pattern" in order to clone the Node for the lambda application.
  // See details here: http://www.cplusplus.com/forum/articles/18757/
  return shared_ptr<NodeEvaluation>(new NodeEvaluation());
}

void NodeEvaluation::DeleteNode() {
  environment = shared_ptr<NodeEnvironment>();
  parent = shared_ptr<NodeEvaluation>();
  output_references.clear();
}

bool ReevaluationOrderComparer::operator()(const ReevaluationEntry& first, const ReevaluationEntry& second) {
  if (first.reevaluation_node->myorder.size() == 0) {
    throw std::runtime_error("The first node has not been evaluated yet!");
  }
  if (second.reevaluation_node->myorder.size() == 0) {
    throw std::runtime_error("The second node has not been evaluated yet!");
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
























NodeTypes NodeDirectiveAssume::GetNodeType() { return DIRECTIVE_ASSUME; }
NodeDirectiveAssume::NodeDirectiveAssume(shared_ptr<VentureSymbol> name, shared_ptr<NodeEvaluation> expression)
  : name(name), expression(expression) {}
void NodeDirectiveAssume::GetChildren(queue< shared_ptr<Node> >& processing_queue) {
  if (earlier_evaluation_nodes != shared_ptr<NodeEvaluation>()) {
    processing_queue.push(earlier_evaluation_nodes);
  }
  assert(expression.get() != 0);
  processing_queue.push(expression);
};

void NodeDirectiveAssume::DeleteNode() {
  environment = shared_ptr<NodeEnvironment>();
  parent = shared_ptr<NodeEvaluation>();
  output_references.clear();
}








NodeTypes NodeDirectivePredict::GetNodeType() { return DIRECTIVE_PREDICT; }
NodeDirectivePredict::NodeDirectivePredict(shared_ptr<NodeEvaluation> expression)
  : expression(expression) {}
void NodeDirectivePredict::GetChildren(queue< shared_ptr<Node> >& processing_queue) {
  if (earlier_evaluation_nodes != shared_ptr<NodeEvaluation>()) {
    processing_queue.push(earlier_evaluation_nodes);
  }
  assert(expression.get() != 0);
  processing_queue.push(expression);
};

void NodeDirectivePredict::DeleteNode() {
  environment = shared_ptr<NodeEnvironment>();
  parent = shared_ptr<NodeEvaluation>();
  output_references.clear();
}


















NodeTypes NodeDirectiveObserve::GetNodeType() { return DIRECTIVE_OBSERVE; }
NodeDirectiveObserve::NodeDirectiveObserve(shared_ptr<NodeEvaluation> expression, shared_ptr<VentureValue> observed_value)
  : expression(expression), observed_value(observed_value) {}
void NodeDirectiveObserve::GetChildren(queue< shared_ptr<Node> >& processing_queue) {
  if (earlier_evaluation_nodes != shared_ptr<NodeEvaluation>()) {
    processing_queue.push(earlier_evaluation_nodes);
  }
  assert(expression.get() != 0);
  processing_queue.push(expression);
}

void NodeDirectiveObserve::DeleteNode() {
  environment = shared_ptr<NodeEnvironment>();
  parent = shared_ptr<NodeEvaluation>();
  output_references.clear();
}















NodeTypes NodeSelfEvaluating::GetNodeType() { return SELF_EVALUATING; }
NodeSelfEvaluating::NodeSelfEvaluating(shared_ptr<VentureValue> value)
  : value(value) {}
shared_ptr<NodeEvaluation> NodeSelfEvaluating::clone() const {
  return shared_ptr<NodeSelfEvaluating>(new NodeSelfEvaluating(this->value));
}
void NodeSelfEvaluating::GetChildren(queue< shared_ptr<Node> >& processing_queue) {
  if (earlier_evaluation_nodes != shared_ptr<NodeEvaluation>()) {
    processing_queue.push(earlier_evaluation_nodes);
  }
};

void NodeSelfEvaluating::DeleteNode() {
  environment = shared_ptr<NodeEnvironment>();
  parent = shared_ptr<NodeEvaluation>();
  output_references.clear();
}













NodeTypes NodeLambdaCreator::GetNodeType() { return LAMBDA_CREATOR; }
NodeLambdaCreator::NodeLambdaCreator(shared_ptr<VentureList> arguments, shared_ptr<NodeEvaluation> expressions)
  : arguments(arguments), expressions(expressions) {

}
shared_ptr<NodeEvaluation> NodeLambdaCreator::clone() const {
  return shared_ptr<NodeLambdaCreator>(new NodeLambdaCreator(this->arguments, this->expressions));
}
void NodeLambdaCreator::GetChildren(queue< shared_ptr<Node> >& processing_queue) {
  if (earlier_evaluation_nodes != shared_ptr<NodeEvaluation>()) {
    processing_queue.push(earlier_evaluation_nodes);
  }
  assert(expressions.get() != 0);
  processing_queue.push(expressions);
};
// Using standard copy constructor.

void NodeLambdaCreator::DeleteNode() {
  environment = shared_ptr<NodeEnvironment>();
  parent = shared_ptr<NodeEvaluation>();
  output_references.clear();
}




NodeTypes NodeLookup::GetNodeType() { return LOOKUP; }
NodeLookup::NodeLookup(shared_ptr<VentureSymbol> symbol)
  : symbol(symbol) {}
shared_ptr<NodeEvaluation> NodeLookup::clone() const {
  return shared_ptr<NodeLookup>(new NodeLookup(this->symbol));
}
void NodeLookup::GetChildren(queue< shared_ptr<Node> >& processing_queue) {
  if (earlier_evaluation_nodes != shared_ptr<NodeEvaluation>()) {
    processing_queue.push(earlier_evaluation_nodes);
  }
};

void NodeLookup::DeleteNode() {
  environment = shared_ptr<NodeEnvironment>();
  parent = shared_ptr<NodeEvaluation>();
  output_references.clear();
  where_lookuped = shared_ptr<NodeVariable>();
}







NodeTypes NodeApplicationCaller::GetNodeType() { return APPLICATION_CALLER; }
NodeApplicationCaller::NodeApplicationCaller(shared_ptr<NodeEvaluation> application_operator,
                      vector< shared_ptr<NodeEvaluation> >& application_operands)
  : saved_evaluated_operator(shared_ptr<VentureValue>()),
    proposing_evaluated_operator(shared_ptr<VentureValue>()),
    application_operator(application_operator),
    application_operands(application_operands),
    application_node(shared_ptr<NodeEvaluation>()),
    new_application_node(shared_ptr<NodeEvaluation>())
{}
shared_ptr<NodeEvaluation> NodeApplicationCaller::clone() const {
  vector< shared_ptr<NodeEvaluation> > empty_vector; // Remove it.
  shared_ptr<NodeApplicationCaller> NodeApplicationCaller_new =
    shared_ptr<NodeApplicationCaller>
      (new NodeApplicationCaller(shared_ptr<NodeEvaluation>(),
                                 empty_vector));
  NodeApplicationCaller_new->application_operator =
    shared_ptr<NodeEvaluation>(application_operator->clone());
  for (size_t index = 0; index < application_operands.size(); index++) {
    NodeApplicationCaller_new->application_operands.push_back(
      shared_ptr<NodeEvaluation>(application_operands[index]->clone()));
  }
  return NodeApplicationCaller_new;
}
void NodeApplicationCaller::GetChildren(queue< shared_ptr<Node> >& processing_queue) {
  if (earlier_evaluation_nodes != shared_ptr<NodeEvaluation>()) {
    processing_queue.push(earlier_evaluation_nodes);
  }
  assert(application_operator.get() != 0);
  processing_queue.push(application_operator);
  for (size_t index = 0; index < application_operands.size(); index++) {
    assert(application_operands[index].get() != 0);
    processing_queue.push(application_operands[index]);
  }
  if (this->evaluated == true) { // Is it right?
    if (application_node.get() == 0) {
      stack< shared_ptr<Node> > tmp;
      tmp.push(dynamic_pointer_cast<Node>(this->shared_from_this()));
      cout << this->evaluated << endl;
      assert(false);
      DrawGraphDuringMH(GetLastDirectiveNode(), tmp);
      assert(false);
    }
    assert(application_node.get() != 0);
    processing_queue.push(application_node);
  }
};

void NodeApplicationCaller::DeleteNode() {
  // cout << "MyUniqueID: " << this->GetUniqueID() << endl;
  environment = shared_ptr<NodeEnvironment>();
  parent = shared_ptr<NodeEvaluation>();
  output_references.clear();
}






NodeTypes NodeXRPApplication::GetNodeType() { return XRP_APPLICATION; }
NodeXRPApplication::NodeXRPApplication(shared_ptr<VentureXRP> xrp)
  : xrp(xrp),
    sampled_value(shared_ptr<VentureValue>()),
    new_sampled_value(shared_ptr<VentureValue>()),
    frozen(false)
{}
// It should not have clone() method?
void NodeXRPApplication::GetChildren(queue< shared_ptr<Node> >& processing_queue) {
  if (earlier_evaluation_nodes != shared_ptr<NodeEvaluation>()) {
    processing_queue.push(earlier_evaluation_nodes);
  }
};

void NodeXRPApplication::DeleteNode() {
  environment = shared_ptr<NodeEnvironment>();
  parent = shared_ptr<NodeEvaluation>();
  output_references.clear();
  xrp = shared_ptr<VentureXRP>(); // Is it necessary?
}  




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
      expression->GetType() == SMOOTHEDCOUNT ||
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
    throw std::runtime_error("Undefined expression.");
  }
}

void NodeEnvironment::DeleteNode() {
  for (size_t index = 0; index < local_variables.size(); index++) { // Assuming there are no (def)ed inside variables.
    local_variables[index]->DeleteNode();
  }
  parent_environment = shared_ptr<NodeEnvironment>();
  variables.clear();
  local_variables.clear();
}

shared_ptr<VentureValue>
NodeEvaluation::Evaluate(shared_ptr<NodeEnvironment> environment) { // It seems we do not need this function, do we?
  throw std::runtime_error("It should not happen.");
}

shared_ptr<VentureValue>
NodeDirectiveAssume::Evaluate(shared_ptr<NodeEnvironment> environment) {
  this->my_value =              Evaluator(this->expression,
                                environment,
                                dynamic_pointer_cast<Node>(this->shared_from_this()),
                                dynamic_pointer_cast<NodeEvaluation>(this->shared_from_this()));
  this->output_references.insert(
    BindToEnvironment(environment,
                      this->name,
                      this->my_value));
  return NIL_INSTANCE; // Something wiser?
}

shared_ptr<VentureValue>
NodeDirectivePredict::Evaluate(shared_ptr<NodeEnvironment> environment) {
   this->my_value =
         Evaluator(this->expression,
                   environment,
                   dynamic_pointer_cast<Node>(this->shared_from_this()),
                   dynamic_pointer_cast<NodeEvaluation>(this->shared_from_this()));
   cout << this->my_value->GetString() << "$" << endl;
   return this->my_value;
}

shared_ptr<VentureValue>
NodeDirectiveObserve::Evaluate(shared_ptr<NodeEnvironment> environment) {
  Evaluator(this->expression,
            environment,
            dynamic_pointer_cast<Node>(this->shared_from_this()),
            dynamic_pointer_cast<NodeEvaluation>(this->shared_from_this()));

  // Silly:
  dynamic_pointer_cast<NodeXRPApplication>(
    dynamic_pointer_cast<NodeApplicationCaller>(this->expression)->application_node)->sampled_value =
      this->observed_value;
  random_choices.erase(
    dynamic_pointer_cast<NodeXRPApplication>(
      dynamic_pointer_cast<NodeApplicationCaller>(this->expression)->application_node));

  return NIL_INSTANCE;
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
  return LookupValue(environment, this->symbol, dynamic_pointer_cast<NodeEvaluation>(this->shared_from_this()), false);
}

shared_ptr<VentureValue>
EvaluateApplication(shared_ptr<VentureValue> evaluated_operator,
                    shared_ptr<NodeEnvironment> local_environment,
                    size_t number_of_operands,
                    shared_ptr<NodeEvaluation>& application_node, // "By reference" is because we also set it in the application caller node.
                    shared_ptr<NodeApplicationCaller> application_caller_ptr) {
  if (evaluated_operator->GetType() == LAMBDA) {
    shared_ptr<VentureList> enumerate_arguments = ToVentureType<VentureLambda>(evaluated_operator)->formal_arguments;
    size_t index = 0;
    while (enumerate_arguments->GetType() != NIL) {
      if (index >= number_of_operands) {
        throw std::runtime_error("Too few arguments have been passed to lambda.");
      }
      BindVariableToEnvironment(local_environment,
                                ToVentureType<VentureSymbol>(enumerate_arguments->car),
                                local_environment->local_variables[index]);
      index++;
      enumerate_arguments = GetNext(enumerate_arguments);
    }
    if (index != number_of_operands) {
      throw std::runtime_error("Too many arguments have been passed to lambda.");
    }
    
    application_node =
      shared_ptr<NodeEvaluation>(ToVentureType<VentureLambda>(evaluated_operator)->expressions->clone());
    // cout << "***" << ToVentureType<VentureLambda>(evaluated_operator)->expressions->GetUniqueID();
    // cout << " " << application_node->GetUniqueID() << endl;

    return Evaluator(application_node,
                      local_environment,
                      dynamic_pointer_cast<Node>(application_caller_ptr),
                      dynamic_pointer_cast<NodeEvaluation>(application_caller_ptr));
  } else if (evaluated_operator->GetType() == XRP_REFERENCE) {
    // Just for mem now.
    // Maybe, implement it in the future in a better way.
    for (size_t index = 0; index < number_of_operands; index++) {
      BindVariableToEnvironment(local_environment,
                                shared_ptr<VentureSymbol>(new VentureSymbol("XRP_ARGUMENT_" + boost::lexical_cast<string>(index))),
                                local_environment->local_variables[index]);
    }

    application_node = shared_ptr<NodeEvaluation>(new NodeXRPApplication(
      dynamic_pointer_cast<VentureXRP>(evaluated_operator))); // Do not use ToVentureType(...)
                                                              // because we have checked
                                                              // the type in the IF condition.
    
    cout << application_node << " with " << local_environment->local_variables.size() << endl;

    return Evaluator(application_node,
                     local_environment,
                     dynamic_pointer_cast<Node>(application_caller_ptr),
                     dynamic_pointer_cast<NodeEvaluation>(application_caller_ptr));
  } else {
    throw std::runtime_error((string("Attempt to apply neither LAMBDA nor XRP (") + boost::lexical_cast<string>(evaluated_operator->GetType()) + string(")")).c_str());
  }
}

shared_ptr<VentureValue>
NodeApplicationCaller::Evaluate(shared_ptr<NodeEnvironment> environment) {
  shared_ptr<VentureValue> evaluated_operator = Evaluator(application_operator,
                                                          environment,
                                                          dynamic_pointer_cast<Node>(this->shared_from_this()),
                                                          dynamic_pointer_cast<NodeEvaluation>(this->shared_from_this()));
  this->saved_evaluated_operator = evaluated_operator;
  
  shared_ptr<NodeEnvironment> previous_environment;
  if (evaluated_operator->GetType() == LAMBDA) {
    previous_environment = ToVentureType<VentureLambda>(evaluated_operator)->scope_environment;
  } else {
    previous_environment = environment;
  }

  shared_ptr<NodeEnvironment> local_environment =
    shared_ptr<NodeEnvironment>(new NodeEnvironment(previous_environment));
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

  //if (this->xrp->xrp->SaveReferencesToItsSamplers() == true) {
  //  this->xrp->xrp->references_to_its_samplers.insert(this->shared_from_this());
  //}
  //cout << "SIZE: " << GetArgumentsFromEnvironment(environment,
  //                                dynamic_pointer_cast<NodeEvaluation>(this->shared_from_this())).size() << endl;
  vector< shared_ptr<VentureValue> > got_arguments = GetArgumentsFromEnvironment(environment, // Not efficient?
                                  dynamic_pointer_cast<NodeEvaluation>(this->shared_from_this()));
  return this->xrp->xrp->Sample(got_arguments,
                                dynamic_pointer_cast<NodeXRPApplication>(this->shared_from_this()));
}

shared_ptr<ReevaluationResult> // FIXME: Why we pass ReevaluationResult as shared_ptr?
                               //        It is not enough to pass it as just value?
NodeXRPApplication::Reevaluate(shared_ptr<VentureValue> passing_value,
                               shared_ptr<Node> sender,
                               ReevaluationParameters& reevaluation_parameters) { // Not efficient?
  vector< shared_ptr<VentureValue> > got_old_arguments = GetArgumentsFromEnvironment(environment,
        dynamic_pointer_cast<NodeEvaluation>(this->shared_from_this()), true);
  vector< shared_ptr<VentureValue> > got_new_arguments = GetArgumentsFromEnvironment(environment,
        dynamic_pointer_cast<NodeEvaluation>(this->shared_from_this()), false);
  shared_ptr<RescorerResamplerResult> result =
    this->xrp->xrp->RescorerResampler(
      got_old_arguments,
      got_new_arguments,
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
  throw std::runtime_error(("This node does not have the Reevaluation function (node type " + boost::lexical_cast<string>(this->GetNodeType()) + ").").c_str());
}

shared_ptr<ReevaluationResult>
Node::Reevaluate(shared_ptr<VentureValue> passing_value,
                 shared_ptr<Node> sender,
                 ReevaluationParameters& reevaluation_parameters) {
  throw std::runtime_error(("This node does not have the Reevaluation function (node type " + boost::lexical_cast<string>(this->GetNodeType()) + ").").c_str());
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
    FreezeBranch(application_node, reevaluation_parameters);
    
    return shared_ptr<ReevaluationResult>(
      new ReevaluationResult(new_value, true));
  } else {
    // throw std::runtime_error("NodeApplicationCaller::Reevaluate: strange sender.");
    // It is from memoizer.
    // Just passing up:
    return shared_ptr<ReevaluationResult>(
      new ReevaluationResult(passing_value, true));
  }
}

shared_ptr<ReevaluationResult>
NodeDirectiveAssume::Reevaluate(shared_ptr<VentureValue> passing_value,
                                shared_ptr<Node> sender,
                                ReevaluationParameters& reevaluation_parameters) {
  this->my_new_value = passing_value;
  // Just passing up:
  return shared_ptr<ReevaluationResult>(
    new ReevaluationResult(passing_value, true));
}

shared_ptr<ReevaluationResult>
NodeDirectivePredict::Reevaluate(shared_ptr<VentureValue> passing_value,
                                 shared_ptr<Node> sender,
                                 ReevaluationParameters& reevaluation_parameters) {
  this->my_new_value = passing_value;
  // Just passing up:
  return shared_ptr<ReevaluationResult>(
    new ReevaluationResult(passing_value, true));
}

/*
shared_ptr<ReevaluationResult>
NodeDirectiveObserve::Reevaluate(shared_ptr<VentureValue> passing_value,
                                 shared_ptr<Node> sender,
                                 ReevaluationParameters& reevaluation_parameters) {
  // Just passing up:
  return shared_ptr<ReevaluationResult>(
    new ReevaluationResult(passing_value, true));
}
*/

void ApplyToMeAndAllMyChildren(shared_ptr<Node> first_node,
                               void (*f)(shared_ptr<Node>)) {
  queue< shared_ptr<Node> > processing_queue;
  processing_queue.push(first_node);
  while (!processing_queue.empty()) {
    if (processing_queue.front()->WasEvaluated() == false) {
      processing_queue.pop();
      continue;
      // We assume that not evaluated yet node just will be removed
      // when its parent will be removed, because there is no cycled
      // pointers.
    }
    if (processing_queue.front() != first_node &&
         (processing_queue.front()->GetNodeType() == DIRECTIVE_ASSUME || 
           processing_queue.front()->GetNodeType() == DIRECTIVE_PREDICT || 
           processing_queue.front()->GetNodeType() == DIRECTIVE_OBSERVE)) {
      // Do not delete children, which are directives itself.
      processing_queue.pop();
      continue;
    }
    processing_queue.front()->GetChildren(processing_queue);
    (*f)(dynamic_pointer_cast<NodeEvaluation>(processing_queue.front()->shared_from_this()));
    processing_queue.pop();
  }
}

bool Node::WasEvaluated() {
  return false;
}

bool NodeEvaluation::WasEvaluated() {
  return this->evaluated;
}

void DrawGraphDuringMH(shared_ptr<Node> first_node, stack< shared_ptr<Node> >& touched_nodes) {
  cout << "Writing the graph" << endl;

  std::ofstream graph_file;
  graph_file.open("C:/Temp/graph_output.txt");
  graph_file << "digraph G {" << endl;

  queue< pair< string, shared_ptr<Node> > > processing_queue;
  processing_queue.push(make_pair("", first_node));
  while (!processing_queue.empty()) {
    queue< shared_ptr<Node> > temporal_queue;
    if (processing_queue.front().second == shared_ptr<Node>()) {
      processing_queue.pop();
      continue; // It means that some not yet evaluated node returned its not ready child
                // (i.e. not existing yet child).
    }
    processing_queue.front().second->GetChildren(temporal_queue);
    while (!temporal_queue.empty()) {
      processing_queue.push(make_pair(processing_queue.front().second->GetUniqueID(), temporal_queue.front()));
      temporal_queue.pop();
    }
    std::deque< shared_ptr<Node> >::const_iterator already_existent_element =
      std::find(GetQueueContainer(touched_nodes).begin(), GetQueueContainer(touched_nodes).end(), processing_queue.front().second);
    graph_file << "  Node" << processing_queue.front().second->GetUniqueID() << " [label=\"";
    if (!(already_existent_element == GetQueueContainer(touched_nodes).end())) {
      int distance = std::distance(GetQueueContainer(touched_nodes).begin(), already_existent_element);
      graph_file << "[MH" << distance << "] ";
    }
    graph_file << "(" << processing_queue.front().second->WasEvaluated() << ") ";
    graph_file << processing_queue.front().second->GetUniqueID() << ". ";
    graph_file << GetNodeTypeAsString(processing_queue.front().second->GetNodeType())
      << ": " << processing_queue.front().second->GetContent() << "\"" << endl;
    if (!(already_existent_element == GetQueueContainer(touched_nodes).end())) {
      graph_file << ",color=red";
    }
    graph_file << "]";
    if (processing_queue.front().first != "") {
      graph_file << "  Node" << processing_queue.front().first << " -> "
        << "Node" << processing_queue.front().second->GetUniqueID() << endl;
    }

    processing_queue.pop();
  }

  graph_file << "}" << endl;
  graph_file.close();

  cout << "The graph has been written" << endl;
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

void FreezeBranch(shared_ptr<Node> first_node, ReevaluationParameters& reevaluation_parameters) {
  queue< shared_ptr<Node> > processing_queue;
  processing_queue.push(first_node);
  while (!processing_queue.empty()) {
    processing_queue.front()->GetChildren(processing_queue);
    if (processing_queue.front()->GetNodeType() == XRP_APPLICATION &&
          dynamic_pointer_cast<NodeXRPApplication>(processing_queue.front())->xrp->xrp->GetName() == "XRP__memoized_procedure") {
      // It is stupid that we check here by GetName().
      // It means that the Freeze(...) should be implemented in another way...

      // FIXME: GetArgumentsFromEnvironment should be called without adding lookup references!
      vector< shared_ptr<VentureValue> > got_arguments = GetArgumentsFromEnvironment(dynamic_pointer_cast<NodeXRPApplication>(processing_queue.front())->environment, // Not efficient?
                                      dynamic_pointer_cast<NodeEvaluation>(processing_queue.front()), // Are we sure that we have not deleted yet lookup links?
                                      false); // FIXME: we should be sure that we are receiving old arguments!

      string mem_table_key = XRP__memoized_procedure__MakeMapKeyFromArguments(got_arguments);
      XRP__memoizer_map_element& mem_table_element =
        (*(dynamic_pointer_cast<XRP__memoized_procedure>(dynamic_pointer_cast<NodeXRPApplication>(processing_queue.front())->xrp->xrp)->mem_table.find(mem_table_key))).second;

      dynamic_pointer_cast<NodeXRPApplication>(processing_queue.front())->xrp->xrp->Freeze(got_arguments, dynamic_pointer_cast<NodeXRPApplication>(processing_queue.front()));

      if (mem_table_element.hidden_uses +
            mem_table_element.active_uses ==
            mem_table_element.frozen_elements) {
        reevaluation_parameters.random_choices_delta -= CalculateNumberOfRandomChoices(mem_table_element.application_caller_node);
        processing_queue.push(mem_table_element.application_caller_node);
      }
    }
    processing_queue.pop();
  }
};

void UnfreezeBranch(shared_ptr<Node> first_node) {
  queue< shared_ptr<Node> > processing_queue;
  processing_queue.push(first_node);
  while (!processing_queue.empty()) {
    processing_queue.front()->GetChildren(processing_queue);
    if (processing_queue.front()->GetNodeType() == XRP_APPLICATION &&
          dynamic_pointer_cast<NodeXRPApplication>(processing_queue.front())->xrp->xrp->GetName() == "XRP__memoized_procedure") {
      // It is stupid that we check here by GetName().
      // It means that the Freeze(...) should be implemented in another way...

      if (dynamic_pointer_cast<NodeXRPApplication>(processing_queue.front())->frozen == false) {
        throw std::runtime_error("We should be frozen at this time!"); // Not right for the multi-thread version?
      }

      // FIXME: GetArgumentsFromEnvironment should be called without adding lookup references!
      vector< shared_ptr<VentureValue> > got_arguments = GetArgumentsFromEnvironment(dynamic_pointer_cast<NodeXRPApplication>(processing_queue.front())->environment, // Not efficient?
                                      dynamic_pointer_cast<NodeEvaluation>(processing_queue.front()), // Are we sure that we have not deleted yet lookup links?
                                      false); // FIXME: we should be sure that we are receiving old arguments!

      string mem_table_key = XRP__memoized_procedure__MakeMapKeyFromArguments(got_arguments);
      XRP__memoizer_map_element& mem_table_element =
        (*(dynamic_pointer_cast<XRP__memoized_procedure>(dynamic_pointer_cast<NodeXRPApplication>(processing_queue.front())->xrp->xrp)->mem_table.find(mem_table_key))).second;

      dynamic_pointer_cast<NodeXRPApplication>(processing_queue.front())->xrp->xrp->Unfreeze(got_arguments, dynamic_pointer_cast<NodeXRPApplication>(processing_queue.front()));

      if (mem_table_element.hidden_uses +
            mem_table_element.active_uses ==
            mem_table_element.frozen_elements + 1) {
        processing_queue.push(mem_table_element.application_caller_node);
      }
    }
    processing_queue.pop();
  }
};
