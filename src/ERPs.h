
#ifndef VENTURE___ERPS_H
#define VENTURE___ERPS_H

#include "Header.h"
#include "VentureValues.h"
#include "VentureParser.h"
#include "XRP.h"

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
  virtual shared_ptr<VentureValue> ERP__Flip::Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller);

public:
  virtual string ERP__Flip::GetName();
};

class ERP__Normal : public ERP {
  virtual real ERP__Normal::GetSampledLoglikelihood(vector< shared_ptr<VentureValue> >& arguments,
                                 shared_ptr<VentureValue> sampled_value);
  virtual shared_ptr<VentureValue> ERP__Normal::Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller);

public:
  virtual string ERP__Normal::GetName();
};

class ERP__Beta : public ERP {
  virtual real ERP__Beta::GetSampledLoglikelihood(vector< shared_ptr<VentureValue> >& arguments,
                                 shared_ptr<VentureValue> sampled_value);
  virtual shared_ptr<VentureValue> ERP__Beta::Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller);

public:
  virtual string ERP__Beta::GetName();
};

class ERP__UniformDiscrete : public ERP {
  virtual real ERP__UniformDiscrete::GetSampledLoglikelihood(vector< shared_ptr<VentureValue> >& arguments,
                                 shared_ptr<VentureValue> sampled_value);
  virtual shared_ptr<VentureValue> ERP__UniformDiscrete::Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller);

public:
  virtual string ERP__UniformDiscrete::GetName();
};

class ERP__UniformContinuous : public ERP {
  virtual real ERP__UniformContinuous::GetSampledLoglikelihood(vector< shared_ptr<VentureValue> >& arguments,
                                 shared_ptr<VentureValue> sampled_value);
  virtual shared_ptr<VentureValue> ERP__UniformContinuous::Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller);

public:
  virtual string ERP__UniformContinuous::GetName();
};

class ERP__NoisyNegate : public ERP {
  virtual real ERP__NoisyNegate::GetSampledLoglikelihood(vector< shared_ptr<VentureValue> >& arguments,
                                 shared_ptr<VentureValue> sampled_value);
  virtual shared_ptr<VentureValue> ERP__NoisyNegate::Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller);

public:
  virtual string ERP__NoisyNegate::GetName();
};

class ERP__ConditionERP : public ERP {
  virtual real ERP__ConditionERP::GetSampledLoglikelihood(vector< shared_ptr<VentureValue> >& arguments,
                                 shared_ptr<VentureValue> sampled_value);
  virtual shared_ptr<VentureValue> ERP__ConditionERP::Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller);

public:
  virtual bool ERP__ConditionERP::IsRandomChoice();
  virtual bool ERP__ConditionERP::CouldBeRescored();
  virtual string ERP__ConditionERP::GetName();
};

#endif
