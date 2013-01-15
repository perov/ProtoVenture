
#ifndef VENTURE___ERPS_H
#define VENTURE___ERPS_H

#include "Header.h"
#include "VentureValues.h"
#include "VentureParser.h"
#include "Analyzer.h"
#include "XRPCore.h"

class ERP : public XRP {
  virtual void Incorporate(vector< shared_ptr<VentureValue> >& arguments,
                   shared_ptr<VentureValue> sampled_value);
  virtual void Remove(vector< shared_ptr<VentureValue> >& arguments,
              shared_ptr<VentureValue> sampled_value);
  
public:
  virtual bool IsRandomChoice();
  virtual bool CouldBeRescored();
  virtual string GetName();
};

// IMPORTANT FIXME:
// weight SHOULD BE IMPLEMENTED NOT VIA REAL!
class ERP__Flip : public ERP {
  virtual real GetSampledLoglikelihood(vector< shared_ptr<VentureValue> >& arguments,
                                 shared_ptr<VentureValue> sampled_value);
  virtual shared_ptr<VentureValue> Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller, EvaluationConfig& evaluation_config);

public:
  virtual string GetName();
};

class ERP__Normal : public ERP {
  virtual real GetSampledLoglikelihood(vector< shared_ptr<VentureValue> >& arguments,
                                 shared_ptr<VentureValue> sampled_value);
  virtual shared_ptr<VentureValue> Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller, EvaluationConfig& evaluation_config);

public:
  virtual string GetName();
};

class ERP__Beta : public ERP {
  virtual real GetSampledLoglikelihood(vector< shared_ptr<VentureValue> >& arguments,
                                 shared_ptr<VentureValue> sampled_value);
  virtual shared_ptr<VentureValue> Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller, EvaluationConfig& evaluation_config);

public:
  virtual string GetName();
};

class ERP__UniformDiscrete : public ERP {
  virtual real GetSampledLoglikelihood(vector< shared_ptr<VentureValue> >& arguments,
                                 shared_ptr<VentureValue> sampled_value);
  virtual shared_ptr<VentureValue> Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller, EvaluationConfig& evaluation_config);

public:
  virtual string GetName();
};

class ERP__UniformContinuous : public ERP {
  virtual real GetSampledLoglikelihood(vector< shared_ptr<VentureValue> >& arguments,
                                 shared_ptr<VentureValue> sampled_value);
  virtual shared_ptr<VentureValue> Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller, EvaluationConfig& evaluation_config);

public:
  virtual string GetName();
};

class ERP__NoisyNegate : public ERP {
  virtual real GetSampledLoglikelihood(vector< shared_ptr<VentureValue> >& arguments,
                                 shared_ptr<VentureValue> sampled_value);
  virtual shared_ptr<VentureValue> Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller, EvaluationConfig& evaluation_config);

public:
  virtual string GetName();
};

class ERP__Categorical : public ERP {
  virtual real GetSampledLoglikelihood(vector< shared_ptr<VentureValue> >& arguments,
                                 shared_ptr<VentureValue> sampled_value);
  virtual shared_ptr<VentureValue> Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller, EvaluationConfig& evaluation_config);

public:
  virtual string GetName();
};

class ERP__ConditionERP : public ERP {
  virtual real GetSampledLoglikelihood(vector< shared_ptr<VentureValue> >& arguments,
                                 shared_ptr<VentureValue> sampled_value);
  virtual shared_ptr<VentureValue> Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller, EvaluationConfig& evaluation_config);

public:
  virtual bool IsRandomChoice();
  virtual bool CouldBeRescored();
  virtual string GetName();
};

#endif
