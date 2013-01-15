
#include "Header.h"
#include "VentureValues.h"
#include "VentureParser.h"
#include "Analyzer.h"
#include "XRPCore.h"
#include "RIPL.h"
#include "ERPs.h"
#include "Primitives.h"

real Primitive::GetSampledLoglikelihood(vector< shared_ptr<VentureValue> >& arguments,
                                shared_ptr<VentureValue> sampled_value) { // inline?
  return log(1.0);
}
  
bool Primitive::IsRandomChoice() { return false; }
bool Primitive::CouldBeRescored() { return false; }
string Primitive::GetName() { return "PrimitiveClass"; }
  
shared_ptr<VentureValue> Primitive__RealPlus::Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller) {
  shared_ptr<VentureReal> result = shared_ptr<VentureReal>(new VentureReal(0.0));
  for (size_t index = 0; index < arguments.size(); index++) {
    result->data += arguments[index]->GetReal();
  }
  return result;
}
string Primitive__RealPlus::GetName() { return "Primitive__RealPlus"; }

shared_ptr<VentureValue> Primitive__RealMultiply::Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller) {
  shared_ptr<VentureReal> result = shared_ptr<VentureReal>(new VentureReal(1.0));
  for (size_t index = 0; index < arguments.size(); index++) {
    result->data *= arguments[index]->GetReal();
  }
  return result;
}
string Primitive__RealMultiply::GetName() { return "Primitive__RealMultiply"; }

shared_ptr<VentureValue> Primitive__RealPower::Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller) {
  if (arguments.size() != 2) {
    throw std::runtime_error("Wrong number of arguments.");
  }
  shared_ptr<VentureReal> result = shared_ptr<VentureReal>(new VentureReal(0.0));
  result->data
    = pow(arguments[0]->GetReal(),
          arguments[1]->GetReal());
  return result;
}
string Primitive__RealPower::GetName() { return "Primitive__RealPower"; }

shared_ptr<VentureValue> Primitive__RealMinus::Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller) {
  if (arguments.size() != 2) {
    throw std::runtime_error("Wrong number of arguments.");
  }
  shared_ptr<VentureReal> result = shared_ptr<VentureReal>(new VentureReal(0.0));
  result->data
    = arguments[0]->GetReal() -
        arguments[1]->GetReal();
  return result;
}
string Primitive__RealMinus::GetName() { return "Primitive__RealMinus"; }

shared_ptr<VentureValue> Primitive__RealDivide::Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller) {
  if (arguments.size() != 2) {
    throw std::runtime_error("Wrong number of arguments.");
  }
  shared_ptr<VentureReal> result = shared_ptr<VentureReal>(new VentureReal(0.0));
  result->data
    = arguments[0]->GetReal() /
        arguments[1]->GetReal();
  return result;
}
string Primitive__RealDivide::GetName() { return "Primitive__RealDivide"; }

shared_ptr<VentureValue> Primitive__RealCos::Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller) {
  if (arguments.size() != 1) {
    throw std::runtime_error("Wrong number of arguments.");
  }
  shared_ptr<VentureReal> result = shared_ptr<VentureReal>(new VentureReal(0.0));
  result->data
    = cos(arguments[0]->GetReal());
  return result;
}
string Primitive__RealCos::GetName() { return "Primitive__RealCos"; }

shared_ptr<VentureValue> Primitive__RealSin::Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller) {
  if (arguments.size() != 1) {
    throw std::runtime_error("Wrong number of arguments.");
  }
  shared_ptr<VentureReal> result = shared_ptr<VentureReal>(new VentureReal(0.0));
  result->data
    = sin(arguments[0]->GetReal());
  return result;
}
string Primitive__RealSin::GetName() { return "Primitive__RealSin"; }

shared_ptr<VentureValue> Primitive__RealEqualOrGreater::Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller) {
  if (arguments.size() != 2) {
    throw std::runtime_error("Wrong number of arguments.");
  }

  bool result;
  if (arguments[0]->GetReal() >= arguments[1]->GetReal()) {
    result = true;
  } else {
    result = false;
  }
  return shared_ptr<VentureBoolean>(new VentureBoolean(result));
}
string Primitive__RealEqualOrGreater::GetName() { return "Primitive__RealEqualOrGreater"; }

shared_ptr<VentureValue> Primitive__RealEqualOrLesser::Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller) {
  if (arguments.size() != 2) {
    throw std::runtime_error("Wrong number of arguments.");
  }

  bool result;
  if (arguments[0]->GetReal() <= arguments[1]->GetReal()) {
    result = true;
  } else {
    result = false;
  }
  return shared_ptr<VentureBoolean>(new VentureBoolean(result));
}
string Primitive__RealEqualOrLesser::GetName() { return "Primitive__RealEqualOrLesser"; }

shared_ptr<VentureValue> Primitive__RealGreater::Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller) {
  if (arguments.size() != 2) {
    throw std::runtime_error("Wrong number of arguments.");
  }

  bool result;
  if (arguments[0]->GetReal() > arguments[1]->GetReal()) {
    result = true;
  } else {
    result = false;
  }
  return shared_ptr<VentureBoolean>(new VentureBoolean(result));
}
string Primitive__RealGreater::GetName() { return "Primitive__RealGreater"; }

shared_ptr<VentureValue> Primitive__RealLesser::Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller) {
  if (arguments.size() != 2) {
    throw std::runtime_error("Wrong number of arguments.");
  }

  bool result;
  if (arguments[0]->GetReal() < arguments[1]->GetReal()) {
    result = true;
  } else {
    result = false;
  }
  return shared_ptr<VentureBoolean>(new VentureBoolean(result));
}
string Primitive__RealLesser::GetName() { return "Primitive__RealLesser"; }

shared_ptr<VentureValue> Primitive__RealEqual::Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller) {
  if (arguments.size() != 2) {
    throw std::runtime_error("Wrong number of arguments.");
  }

  bool result;
  if (fabs(arguments[0]->GetReal() - arguments[1]->GetReal()) < comparison_epsilon) {
    result = true;
  } else {
    result = false;
  }
  return shared_ptr<VentureBoolean>(new VentureBoolean(result));
}
string Primitive__RealEqual::GetName() { return "Primitive__RealEqual"; }

shared_ptr<VentureValue> Primitive__List::Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller) {
  if (arguments.size() == 0) {
    return NIL_INSTANCE;
  }
  shared_ptr<VentureList> new_list =
    shared_ptr<VentureList>(new VentureList(arguments[0]));
  for (size_t index = 1; index < arguments.size(); index++) {
    // AddToList() is not efficient!
    AddToList(new_list,
              arguments[index]);
  }
  return new_list;
}

string Primitive__List::GetName() { return "Primitive__List"; }
