
#ifndef VENTURE___XRP_H
#define VENTURE___XRP_H

#include "Header.h"
#include "VentureValues.h"
#include "VentureParser.h"
#include "Analyzer.h"

struct NodeXRPApplication;
extern set< shared_ptr<NodeXRPApplication> > random_choices;
extern int next_gensym_atom;

struct RescorerResamplerResult {
  RescorerResamplerResult(shared_ptr<VentureValue>,
                          real,
                          real);
  shared_ptr<VentureValue> new_value;
  real old_loglikelihood;
  real new_loglikelihood;
};

struct ReevaluationResult {
  ReevaluationResult(shared_ptr<VentureValue>,
                     bool);
  shared_ptr<VentureValue> passing_value;
  bool pass_further;
};

struct ReevaluationEntry {
  ReevaluationEntry(shared_ptr<NodeEvaluation>,
                    shared_ptr<NodeEvaluation>,
                    shared_ptr<VentureValue>);
  shared_ptr<NodeEvaluation> reevaluation_node;
  shared_ptr<NodeEvaluation> caller;
  shared_ptr<VentureValue> passing_value;
};

struct OmitPattern {
  OmitPattern(vector<size_t>&,
              vector<size_t>&);
  vector<size_t> omit_pattern;
  vector<size_t> stop_pattern; // stop_pattern is omit_pattern.pop().
                               // Use it in the future to increase efficiency,
                               // and make the code more accurate.
};

struct ReevaluationParameters {
  ReevaluationParameters();
  double loglikelihood_changes;
  int random_choices_delta;
};

class XRP : public boost::enable_shared_from_this<XRP> {
public: // Should be private.
  virtual shared_ptr<VentureValue> XRP::Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller);
  virtual real XRP::GetSampledLoglikelihood(vector< shared_ptr<VentureValue> >&,
                                       shared_ptr<VentureValue>);
  virtual void XRP::Incorporate(vector< shared_ptr<VentureValue> >&,
                                shared_ptr<VentureValue>);
  virtual void XRP::Remove(vector< shared_ptr<VentureValue> >&,
                           shared_ptr<VentureValue>);

public:
  //XRP(shared_ptr<XRP> maker) : maker(maker) {}
  XRP::XRP();
  shared_ptr<VentureValue> XRP::Sample(vector< shared_ptr<VentureValue> >&, // FIXME: why not virtual?
                                       shared_ptr<NodeXRPApplication>);
  virtual void XRP::Unsampler(vector< shared_ptr<VentureValue> >& old_arguments, shared_ptr<NodeXRPApplication> caller); // Unsampler or sampler?
  virtual void XRP::Freeze(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller);
  virtual void XRP::Unfreeze(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller);
  shared_ptr<RescorerResamplerResult>
  XRP::RescorerResampler(vector< shared_ptr<VentureValue> >&,
                         vector< shared_ptr<VentureValue> >&,
                         shared_ptr<NodeXRPApplication>,
                         bool);
  virtual bool XRP::IsRandomChoice();
  virtual bool XRP::CouldBeRescored();
  virtual bool XRP::SaveReferencesToItsSamplers() { return false; } // FIXME: Should be in the XRP.cpp.
  virtual string XRP::GetName();

public: // Should be private?
  //set< shared_ptr<XRP> > references_to_its_samplers;
};

class XRP__CRPmaker : public XRP {
  virtual shared_ptr<VentureValue> XRP__CRPmaker::Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller);
  virtual real XRP__CRPmaker::GetSampledLoglikelihood(vector< shared_ptr<VentureValue> >&,
                                       shared_ptr<VentureValue>);
  virtual void XRP__CRPmaker::Incorporate(vector< shared_ptr<VentureValue> >&,
                                shared_ptr<VentureValue>);
  virtual void XRP__CRPmaker::Remove(vector< shared_ptr<VentureValue> >&,
                           shared_ptr<VentureValue>);

public:
  //XRP__CRPmaker::XRP__CRPmaker();
  shared_ptr<VentureValue> XRP__CRPmaker::Sample(vector< shared_ptr<VentureValue> >&,
                                       shared_ptr<NodeXRPApplication>);
  shared_ptr<RescorerResamplerResult>
  XRP__CRPmaker::RescorerResampler(vector< shared_ptr<VentureValue> >&,
                         vector< shared_ptr<VentureValue> >&,
                         shared_ptr<NodeXRPApplication>,
                         bool);
  virtual bool XRP__CRPmaker::IsRandomChoice();
  virtual bool XRP__CRPmaker::CouldBeRescored();
  virtual string XRP__CRPmaker::GetName();
};

class XRP__CRPsampler : public XRP {
  virtual shared_ptr<VentureValue> XRP__CRPsampler::Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller);
  virtual real XRP__CRPsampler::GetSampledLoglikelihood(vector< shared_ptr<VentureValue> >&,
                                       shared_ptr<VentureValue>);
  virtual void XRP__CRPsampler::Incorporate(vector< shared_ptr<VentureValue> >&,
                                shared_ptr<VentureValue>);
  virtual void XRP__CRPsampler::Remove(vector< shared_ptr<VentureValue> >&,
                           shared_ptr<VentureValue>);

public:
  //XRP__CRPsampler::XRP__CRPsampler();
  shared_ptr<VentureValue> XRP__CRPsampler::Sample(vector< shared_ptr<VentureValue> >&,
                                       shared_ptr<NodeXRPApplication>);
  shared_ptr<RescorerResamplerResult>
  XRP__CRPsampler::RescorerResampler(vector< shared_ptr<VentureValue> >&,
                         vector< shared_ptr<VentureValue> >&,
                         shared_ptr<NodeXRPApplication>,
                         bool);
  virtual bool XRP__CRPsampler::IsRandomChoice();
  virtual bool XRP__CRPsampler::CouldBeRescored();
  virtual bool XRP__CRPsampler::SaveReferencesToItsSamplers() { return true; } // FIXME: Should be in the XRP.cpp.
  virtual string XRP__CRPsampler::GetName();

  map<int, size_t> atoms; // next_gensym_atom
  size_t current_number_of_clients;
  real alpha; // Some XRPs do not save arguments.
};

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
      result(shared_ptr<VentureValue>())
  {}

  shared_ptr<NodeApplicationCaller> application_caller_node;
  size_t hidden_uses;
  size_t active_uses;
  size_t frozen_elements;
  shared_ptr<VentureValue> result;
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
  shared_ptr<RescorerResamplerResult>
  XRP__memoizer::RescorerResampler(vector< shared_ptr<VentureValue> >&,
                         vector< shared_ptr<VentureValue> >&,
                         shared_ptr<NodeXRPApplication>,
                         bool);
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
  virtual void XRP__memoized_procedure::Freeze(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller);
  virtual void XRP__memoized_procedure::Unfreeze(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller);
  shared_ptr<RescorerResamplerResult>
  XRP__memoized_procedure::RescorerResampler(vector< shared_ptr<VentureValue> >&,
                         vector< shared_ptr<VentureValue> >&,
                         shared_ptr<NodeXRPApplication>,
                         bool);
  virtual bool XRP__memoized_procedure::IsRandomChoice();
  virtual bool XRP__memoized_procedure::CouldBeRescored();
  virtual string XRP__memoized_procedure::GetName();
  
  shared_ptr<VentureValue> operator_value; // FIXME: VentureValue is not too ambiguous?
  map<string, XRP__memoizer_map_element> mem_table;
};

#endif
