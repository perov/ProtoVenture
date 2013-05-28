#ifndef VENTURE___SHELL__PRIMITIVE_PROBABILISTIC_PROCEDURES_H
#define VENTURE___SHELL__PRIMITIVE_PROBABILISTIC_PROCEDURES_H

#include "Header.h"
#include "VentureValues.h"
#include "VentureParser.h"
#include "XRPCore.h"
#include "ERPs.h"

// Deterministic procedures.

class Primitive__GenerateEmptySurfaceAndPMapPrior : public Primitive {
  virtual shared_ptr<VentureValue> Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller, EvaluationConfig& evaluation_config);
public:
  virtual string GetName();
};

class Primitive__UpdatePMapAndAddLobe : public Primitive {
  virtual shared_ptr<VentureValue> Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller, EvaluationConfig& evaluation_config);
public:
  virtual string GetName();
};

class Primitive__SaveToFile : public Primitive {
  virtual shared_ptr<VentureValue> Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller, EvaluationConfig& evaluation_config);
public:
  virtual string GetName();
};

// Elementary probabilistic procedure.

class ERP__GetLobePosX : public ERP {
  virtual real GetSampledLoglikelihood(vector< shared_ptr<VentureValue> >& arguments,
                                 shared_ptr<VentureValue> sampled_value);
  virtual shared_ptr<VentureValue> Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller, EvaluationConfig& evaluation_config);

public:
  virtual string GetName();
};

class ERP__GetLobePosY : public ERP {
  virtual real GetSampledLoglikelihood(vector< shared_ptr<VentureValue> >& arguments,
                                 shared_ptr<VentureValue> sampled_value);
  virtual shared_ptr<VentureValue> Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller, EvaluationConfig& evaluation_config);

public:
  virtual string GetName();
};

class ERP__NoisyDrillWell : public ERP {
  virtual real GetSampledLoglikelihood(vector< shared_ptr<VentureValue> >& arguments,
                                 shared_ptr<VentureValue> sampled_value);
  virtual shared_ptr<VentureValue> Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller, EvaluationConfig& evaluation_config);

public:
  virtual string GetName();
};

#endif
