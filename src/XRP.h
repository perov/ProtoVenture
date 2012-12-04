
#ifndef VENTURE___XRP_H
#define VENTURE___XRP_H

#include "Header.h"
#include "VentureValues.h"
#include "VentureParser.h"

struct NodeXRPApplication;
extern set< shared_ptr<NodeXRPApplication> > random_choices;

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

class XRP {
  virtual shared_ptr<VentureValue> XRP::Sampler(vector< shared_ptr<VentureValue> >&);
  virtual real XRP::GetSampledLoglikelihood(vector< shared_ptr<VentureValue> >&,
                                       shared_ptr<VentureValue>);
  virtual void XRP::Incorporate(vector< shared_ptr<VentureValue> >&,
                                shared_ptr<VentureValue>);
  virtual void XRP::Remove(vector< shared_ptr<VentureValue> >&,
                           shared_ptr<VentureValue>);

public:
  //XRP(shared_ptr<XRP> maker) : maker(maker) {}
  XRP::XRP();
  shared_ptr<VentureValue> XRP::Sample(vector< shared_ptr<VentureValue> >&,
                                       shared_ptr<NodeXRPApplication>);
  shared_ptr<RescorerResamplerResult>
  XRP::RescorerResampler(vector< shared_ptr<VentureValue> >&,
                         vector< shared_ptr<VentureValue> >&,
                         shared_ptr<NodeXRPApplication>,
                         bool);
  virtual bool XRP::IsRandomChoice();
  virtual bool XRP::CouldBeRescored();
  virtual string XRP::GetName();

private:
  shared_ptr<XRP> maker;
};

class ERP : public XRP {
  virtual void ERP::Incorporate(vector< shared_ptr<VentureValue> >& arguments,
                   shared_ptr<VentureValue> sampled_value);
  virtual void ERP::Remove(vector< shared_ptr<VentureValue> >& arguments,
              shared_ptr<VentureValue> sampled_value);
  
public:
  virtual bool ERP::IsRandomChoice();
  virtual bool ERP::CouldBeRescored();
  virtual string ERP::GetName();
};

// IMPORTANT FIXME:
// weight SHOULD BE IMPLEMENTED NOT VIA REAL!
class ERP__Flip : public ERP {
  virtual real ERP__Flip::GetSampledLoglikelihood(vector< shared_ptr<VentureValue> >& arguments,
                                 shared_ptr<VentureValue> sampled_value);
  virtual shared_ptr<VentureValue> ERP__Flip::Sampler(vector< shared_ptr<VentureValue> >& arguments);

public:
  virtual string ERP__Flip::GetName();
};

class ERP__Normal : public ERP {
  virtual real ERP__Normal::GetSampledLoglikelihood(vector< shared_ptr<VentureValue> >& arguments,
                                 shared_ptr<VentureValue> sampled_value);
  virtual shared_ptr<VentureValue> ERP__Normal::Sampler(vector< shared_ptr<VentureValue> >& arguments);

public:
  virtual string ERP__Normal::GetName();
};

class ERP__UniformDiscrete : public ERP {
  virtual real ERP__UniformDiscrete::GetSampledLoglikelihood(vector< shared_ptr<VentureValue> >& arguments,
                                 shared_ptr<VentureValue> sampled_value);
  virtual shared_ptr<VentureValue> ERP__UniformDiscrete::Sampler(vector< shared_ptr<VentureValue> >& arguments);

public:
  virtual string ERP__UniformDiscrete::GetName();
};

class ERP__UniformContinuous : public ERP {
  virtual real ERP__UniformContinuous::GetSampledLoglikelihood(vector< shared_ptr<VentureValue> >& arguments,
                                 shared_ptr<VentureValue> sampled_value);
  virtual shared_ptr<VentureValue> ERP__UniformContinuous::Sampler(vector< shared_ptr<VentureValue> >& arguments);

public:
  virtual string ERP__UniformContinuous::GetName();
};

class ERP__NoisyNegate : public ERP {
  virtual real ERP__NoisyNegate::GetSampledLoglikelihood(vector< shared_ptr<VentureValue> >& arguments,
                                 shared_ptr<VentureValue> sampled_value);
  virtual shared_ptr<VentureValue> ERP__NoisyNegate::Sampler(vector< shared_ptr<VentureValue> >& arguments);

public:
  virtual string ERP__NoisyNegate::GetName();
};

class ERP__ConditionERP : public ERP {
  virtual real ERP__ConditionERP::GetSampledLoglikelihood(vector< shared_ptr<VentureValue> >& arguments,
                                 shared_ptr<VentureValue> sampled_value);
  virtual shared_ptr<VentureValue> ERP__ConditionERP::Sampler(vector< shared_ptr<VentureValue> >& arguments);

public:
  virtual bool ERP__ConditionERP::IsRandomChoice();
  virtual bool ERP__ConditionERP::CouldBeRescored();
  virtual string ERP__ConditionERP::GetName();
};

class Primitive : public ERP {
  virtual real Primitive::GetSampledLoglikelihood(vector< shared_ptr<VentureValue> >& arguments,
                                 shared_ptr<VentureValue> sampled_value);
  
public:
  virtual bool Primitive::IsRandomChoice();
  virtual bool Primitive::CouldBeRescored();
  virtual string Primitive::GetName();
};

class Primitive__RealPlus : public Primitive {
  virtual shared_ptr<VentureValue> Primitive__RealPlus::Sampler(vector< shared_ptr<VentureValue> >& arguments);

public:
  virtual string Primitive__RealPlus::GetName();
};

class Primitive__RealMultiply : public Primitive {
  virtual shared_ptr<VentureValue> Primitive__RealMultiply::Sampler(vector< shared_ptr<VentureValue> >& arguments);

public:
  virtual string Primitive__RealMultiply::GetName();
};

class Primitive__RealPower : public Primitive {
  virtual shared_ptr<VentureValue> Primitive__RealPower::Sampler(vector< shared_ptr<VentureValue> >& arguments);

public:
  virtual string Primitive__RealPower::GetName();
};

class Primitive__CountEqualOrGreater : public Primitive {
  virtual shared_ptr<VentureValue> Primitive__CountEqualOrGreater::Sampler(vector< shared_ptr<VentureValue> >& arguments);

public:
  virtual string Primitive__CountEqualOrGreater::GetName();
};

class Primitive__List : public Primitive {
  virtual shared_ptr<VentureValue> Primitive__List::Sampler(vector< shared_ptr<VentureValue> >& arguments);

public:
  virtual string Primitive__List::GetName();
};

int UniformDiscrete(int, int);

bool VerifyOrderPattern(vector<size_t>&,
                        vector<size_t>&);

struct Node;

void DeleteNode(shared_ptr<Node>);

void DeleteBranch(shared_ptr<Node>);

void MakeMHProposal();

enum MHDecision { MH_DECLINED, MH_APPROVED };

#endif
