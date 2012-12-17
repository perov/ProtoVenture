
#ifndef VENTURE___PRIMITIVES_H
#define VENTURE___PRIMITIVES_H

#include "Header.h"
#include "VentureValues.h"
#include "VentureParser.h"
#include "XRP.h"
#include "ERPs.h"

class Primitive : public ERP {
  virtual real Primitive::GetSampledLoglikelihood(vector< shared_ptr<VentureValue> >& arguments,
                                 shared_ptr<VentureValue> sampled_value);
  
public:
  virtual bool Primitive::IsRandomChoice();
  virtual bool Primitive::CouldBeRescored();
  virtual string Primitive::GetName();
};

class Primitive__RealPlus : public Primitive {
  virtual shared_ptr<VentureValue> Primitive__RealPlus::Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller);
public:
  virtual string Primitive__RealPlus::GetName();
};

class Primitive__RealMultiply : public Primitive {
  virtual shared_ptr<VentureValue> Primitive__RealMultiply::Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller);
public:
  virtual string Primitive__RealMultiply::GetName();
};

class Primitive__RealPower : public Primitive {
  virtual shared_ptr<VentureValue> Primitive__RealPower::Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller);
public:
  virtual string Primitive__RealPower::GetName();
};

class Primitive__RealMinus : public Primitive {
  virtual shared_ptr<VentureValue> Primitive__RealMinus::Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller);
public:
  virtual string Primitive__RealMinus::GetName();
};

class Primitive__RealDivide : public Primitive {
  virtual shared_ptr<VentureValue> Primitive__RealDivide::Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller);
public:
  virtual string Primitive__RealDivide::GetName();
};

class Primitive__RealCos : public Primitive {
  virtual shared_ptr<VentureValue> Primitive__RealCos::Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller);
public:
  virtual string Primitive__RealCos::GetName();
};

class Primitive__RealSin : public Primitive {
  virtual shared_ptr<VentureValue> Primitive__RealSin::Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller);
public:
  virtual string Primitive__RealSin::GetName();
};

class Primitive__RealEqualOrGreater : public Primitive {
  virtual shared_ptr<VentureValue> Primitive__RealEqualOrGreater::Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller);
public:
  virtual string Primitive__RealEqualOrGreater::GetName();
};

class Primitive__RealEqualOrLesser : public Primitive {
  virtual shared_ptr<VentureValue> Primitive__RealEqualOrLesser::Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller);
public:
  virtual string Primitive__RealEqualOrLesser::GetName();
};

class Primitive__RealGreater : public Primitive { // Useless for Real?
  virtual shared_ptr<VentureValue> Primitive__RealGreater::Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller);
public:
  virtual string Primitive__RealGreater::GetName();
};

class Primitive__RealLesser : public Primitive { // Useless for Real?
  virtual shared_ptr<VentureValue> Primitive__RealLesser::Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller);
public:
  virtual string Primitive__RealLesser::GetName();
};

class Primitive__RealEqual : public Primitive { // With epsilon.
  virtual shared_ptr<VentureValue> Primitive__RealEqual::Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller);
public:
  virtual string Primitive__RealEqual::GetName();
};

class Primitive__List : public Primitive {
  virtual shared_ptr<VentureValue> Primitive__List::Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller);
public:
  virtual string Primitive__List::GetName();
};

#endif
