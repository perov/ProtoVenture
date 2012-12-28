
#ifndef VENTURE___XRPS_H
#define VENTURE___XRPS_H

#include "Analyzer.h"
#include "Evaluator.h"
#include "MHProposal.h"
#include "XRPCore.h"

class XRP__CRPmaker : public XRP {
  virtual shared_ptr<VentureValue> XRP__CRPmaker::Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller);
  virtual real XRP__CRPmaker::GetSampledLoglikelihood(vector< shared_ptr<VentureValue> >&,
                                       shared_ptr<VentureValue>);
  virtual void XRP__CRPmaker::Incorporate(vector< shared_ptr<VentureValue> >&,
                                shared_ptr<VentureValue>);
  virtual void XRP__CRPmaker::Remove(vector< shared_ptr<VentureValue> >&,
                           shared_ptr<VentureValue>);

public:
  shared_ptr<VentureValue> XRP__CRPmaker::Sample(vector< shared_ptr<VentureValue> >&,
                                       shared_ptr<NodeXRPApplication>);
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
  shared_ptr<VentureValue> XRP__CRPsampler::Sample(vector< shared_ptr<VentureValue> >&,
                                       shared_ptr<NodeXRPApplication>);
  virtual bool XRP__CRPsampler::IsRandomChoice();
  virtual bool XRP__CRPsampler::CouldBeRescored();
  virtual string XRP__CRPsampler::GetName();

  map<int, size_t> atoms; // next_gensym_atom
  size_t current_number_of_clients;
  real alpha; // Some XRPs do not save arguments.
};





class XRP__SymmetricDirichletMultinomial_maker : public XRP {
  virtual shared_ptr<VentureValue> XRP__SymmetricDirichletMultinomial_maker::Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller);
  virtual real XRP__SymmetricDirichletMultinomial_maker::GetSampledLoglikelihood(vector< shared_ptr<VentureValue> >&,
                                       shared_ptr<VentureValue>);
  virtual void XRP__SymmetricDirichletMultinomial_maker::Incorporate(vector< shared_ptr<VentureValue> >&,
                                shared_ptr<VentureValue>);
  virtual void XRP__SymmetricDirichletMultinomial_maker::Remove(vector< shared_ptr<VentureValue> >&,
                           shared_ptr<VentureValue>);

public:
  shared_ptr<VentureValue> XRP__SymmetricDirichletMultinomial_maker::Sample(vector< shared_ptr<VentureValue> >&,
                                       shared_ptr<NodeXRPApplication>);
  virtual bool XRP__SymmetricDirichletMultinomial_maker::IsRandomChoice();
  virtual bool XRP__SymmetricDirichletMultinomial_maker::CouldBeRescored();
  virtual string XRP__SymmetricDirichletMultinomial_maker::GetName();
};

class XRP__DirichletMultinomial_maker : public XRP {
  virtual shared_ptr<VentureValue> XRP__DirichletMultinomial_maker::Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller);
  virtual real XRP__DirichletMultinomial_maker::GetSampledLoglikelihood(vector< shared_ptr<VentureValue> >&,
                                       shared_ptr<VentureValue>);
  virtual void XRP__DirichletMultinomial_maker::Incorporate(vector< shared_ptr<VentureValue> >&,
                                shared_ptr<VentureValue>);
  virtual void XRP__DirichletMultinomial_maker::Remove(vector< shared_ptr<VentureValue> >&,
                           shared_ptr<VentureValue>);

public:
  shared_ptr<VentureValue> XRP__DirichletMultinomial_maker::Sample(vector< shared_ptr<VentureValue> >&,
                                       shared_ptr<NodeXRPApplication>);
  virtual bool XRP__DirichletMultinomial_maker::IsRandomChoice();
  virtual bool XRP__DirichletMultinomial_maker::CouldBeRescored();
  virtual string XRP__DirichletMultinomial_maker::GetName();
};

class XRP__DirichletMultinomial_sampler : public XRP {
  virtual shared_ptr<VentureValue> XRP__DirichletMultinomial_sampler::Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller);
  virtual real XRP__DirichletMultinomial_sampler::GetSampledLoglikelihood(vector< shared_ptr<VentureValue> >&,
                                       shared_ptr<VentureValue>);
  virtual void XRP__DirichletMultinomial_sampler::Incorporate(vector< shared_ptr<VentureValue> >&,
                                shared_ptr<VentureValue>);
  virtual void XRP__DirichletMultinomial_sampler::Remove(vector< shared_ptr<VentureValue> >&,
                           shared_ptr<VentureValue>);
  real XRP__DirichletMultinomial_sampler::GetSumOfStatistics();

public:
  shared_ptr<VentureValue> XRP__DirichletMultinomial_sampler::Sample(vector< shared_ptr<VentureValue> >&,
                                       shared_ptr<NodeXRPApplication>);
  virtual bool XRP__DirichletMultinomial_sampler::IsRandomChoice();
  virtual bool XRP__DirichletMultinomial_sampler::CouldBeRescored();
  virtual string XRP__DirichletMultinomial_sampler::GetName();

  vector<real> statistics;
};

#endif
