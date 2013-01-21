
#ifndef VENTURE___XRPS_H
#define VENTURE___XRPS_H

#include "Analyzer.h"
#include "Evaluator.h"
#include "MHProposal.h"
#include "XRPCore.h"

class XRP__CRPmaker : public XRP {
  virtual shared_ptr<VentureValue> Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller, EvaluationConfig& evaluation_config);
  virtual real GetSampledLoglikelihood(vector< shared_ptr<VentureValue> >&,
                                       shared_ptr<VentureValue>);
  virtual void Incorporate(vector< shared_ptr<VentureValue> >&,
                                shared_ptr<VentureValue>);
  virtual void Remove(vector< shared_ptr<VentureValue> >&,
                           shared_ptr<VentureValue>);

public:
  virtual bool IsRandomChoice();
  virtual bool CouldBeRescored();
  virtual string GetName();
};

class XRP__CRPsampler : public XRP {
  virtual shared_ptr<VentureValue> Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller, EvaluationConfig& evaluation_config);
  virtual real GetSampledLoglikelihood(vector< shared_ptr<VentureValue> >&,
                                       shared_ptr<VentureValue>);
  virtual void Incorporate(vector< shared_ptr<VentureValue> >&,
                                shared_ptr<VentureValue>);
  virtual void Remove(vector< shared_ptr<VentureValue> >&,
                           shared_ptr<VentureValue>);

public:
  virtual bool IsRandomChoice();
  virtual bool CouldBeRescored();
  virtual string GetName();
  
  map<int, size_t> atoms; // next_gensym_atom
  size_t current_number_of_clients;
  real alpha; // Some XRPs do not save arguments.
};





class XRP__SymmetricDirichletMultinomial_maker : public XRP {
  virtual shared_ptr<VentureValue> Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller, EvaluationConfig& evaluation_config);
  virtual real GetSampledLoglikelihood(vector< shared_ptr<VentureValue> >&,
                                       shared_ptr<VentureValue>);
  virtual void Incorporate(vector< shared_ptr<VentureValue> >&,
                                shared_ptr<VentureValue>);
  virtual void Remove(vector< shared_ptr<VentureValue> >&,
                           shared_ptr<VentureValue>);

public:
  virtual bool IsRandomChoice();
  virtual bool CouldBeRescored();
  virtual string GetName();
};

class XRP__DirichletMultinomial_maker : public XRP {
  virtual shared_ptr<VentureValue> Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller, EvaluationConfig& evaluation_config);
  virtual real GetSampledLoglikelihood(vector< shared_ptr<VentureValue> >&,
                                       shared_ptr<VentureValue>);
  virtual void Incorporate(vector< shared_ptr<VentureValue> >&,
                                shared_ptr<VentureValue>);
  virtual void Remove(vector< shared_ptr<VentureValue> >&,
                           shared_ptr<VentureValue>);

public:
  virtual bool IsRandomChoice();
  virtual bool CouldBeRescored();
  virtual string GetName();
};

class XRP__DirichletMultinomial_sampler : public XRP {
  virtual shared_ptr<VentureValue> Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller, EvaluationConfig& evaluation_config);
  virtual real GetSampledLoglikelihood(vector< shared_ptr<VentureValue> >&,
                                       shared_ptr<VentureValue>);
  virtual void Incorporate(vector< shared_ptr<VentureValue> >&,
                                shared_ptr<VentureValue>);
  virtual void Remove(vector< shared_ptr<VentureValue> >&,
                           shared_ptr<VentureValue>);
  real GetSumOfStatistics();

public:
  virtual bool IsRandomChoice();
  virtual bool CouldBeRescored();
  virtual string GetName();
  virtual bool CouldBeEnumerated();
  virtual set< shared_ptr<VentureValue> > EnumeratingSupport();

  vector<real> statistics;
};

#endif
