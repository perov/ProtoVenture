
#ifndef VENTURE___XRPMEM_H
#define VENTURE___XRPMEM_H

#include "Analyzer.h"
#include "Evaluator.h"
#include "MHProposal.h"
#include "XRPCore.h"

struct NodeApplicationCaller;

struct XRP__memoizer_map_element {
  XRP__memoizer_map_element()
  {
    throw std::runtime_error("Default constructor should not be evaluated.");
  }
  XRP__memoizer_map_element(shared_ptr<NodeApplicationCaller> application_caller_node)
    : application_caller_node(application_caller_node),
      hidden_uses(0),
      active_uses(0),
      frozen_elements(0),
      result(shared_ptr<NodeVariable>())
  {}

  shared_ptr<NodeApplicationCaller> application_caller_node;
  size_t hidden_uses;
  size_t active_uses;
  size_t frozen_elements;
  shared_ptr<NodeVariable> result; // FIXME: "NodeVariable", is it okay?
};

class XRP__memoizer : public XRP { // So called "mem-maker".
  virtual shared_ptr<VentureValue> XRP__memoizer::Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller);
  virtual real XRP__memoizer::GetSampledLoglikelihood(vector< shared_ptr<VentureValue> >&,
                                       shared_ptr<VentureValue>);
  virtual void XRP__memoizer::Incorporate(vector< shared_ptr<VentureValue> >&,
                                shared_ptr<VentureValue>);
  virtual void XRP__memoizer::Remove(vector< shared_ptr<VentureValue> >&,
                           shared_ptr<VentureValue>);

public:
  shared_ptr<VentureValue> XRP__memoizer::Sample(vector< shared_ptr<VentureValue> >&,
                                       shared_ptr<NodeXRPApplication>);
  virtual bool XRP__memoizer::IsRandomChoice();
  virtual bool XRP__memoizer::CouldBeRescored();
  virtual string XRP__memoizer::GetName();
};

string XRP__memoized_procedure__MakeMapKeyFromArguments(vector< shared_ptr<VentureValue> >& arguments);

class XRP__memoized_procedure : public XRP { // So called "mem-sampler".
  virtual shared_ptr<VentureValue> XRP__memoized_procedure::Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller);
  virtual real XRP__memoized_procedure::GetSampledLoglikelihood(vector< shared_ptr<VentureValue> >&,
                                       shared_ptr<VentureValue>);
  virtual void XRP__memoized_procedure::Incorporate(vector< shared_ptr<VentureValue> >&,
                                shared_ptr<VentureValue>);
  virtual void XRP__memoized_procedure::Remove(vector< shared_ptr<VentureValue> >&,
                           shared_ptr<VentureValue>);

public:
  shared_ptr<VentureValue> XRP__memoized_procedure::Sample(vector< shared_ptr<VentureValue> >&,
                                       shared_ptr<NodeXRPApplication>);
  virtual void XRP__memoized_procedure::Unsampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller); // Unsampler or sampler?
  virtual bool XRP__memoized_procedure::IsRandomChoice();
  virtual bool XRP__memoized_procedure::CouldBeRescored();
  virtual string XRP__memoized_procedure::GetName();
  
  shared_ptr<VentureValue> operator_value; // FIXME: VentureValue is not too ambiguous?
  map<string, XRP__memoizer_map_element> mem_table;
};

void XRP__memoized_procedure__Freeze
 (shared_ptr<XRP__memoized_procedure> xrp_object,
  vector< shared_ptr<VentureValue> >& arguments,
  shared_ptr<NodeXRPApplication> caller);
void XRP__memoized_procedure__Unfreeze
 (shared_ptr<XRP__memoized_procedure> xrp_object,
  vector< shared_ptr<VentureValue> >& arguments,
  shared_ptr<NodeXRPApplication> caller);

void FreezeBranch(shared_ptr<Node> first_node, ReevaluationParameters& reevaluation_parameters);
void UnfreezeBranch(shared_ptr<Node> first_node);

#endif
