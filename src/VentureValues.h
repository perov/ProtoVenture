
#ifndef VENTURE___VENTURE_VALUES_H
#define VENTURE___VENTURE_VALUES_H

#include "Header.h"

class VentureValue {
public:
  VentureValue() {};
  virtual string GetType() { return "NotDefined"; };
  virtual ~VentureValue() {};
};

class VentureInteger : public VentureValue {
public:
  VentureInteger(const int data) : data(data) {};
  // Question: where would be the type transformation happen?
  //           Before this function, it seems?
  string GetType() { return "Integer"; }
private:
  int data; // Const?
};

class VentureReal : public VentureValue {
public:
  VentureReal(const real data) : data(data) {};
  string GetType() { return "Real"; }
private:
  real data; // Const?
};

class VentureBoolean : public VentureValue {
public:
  VentureBoolean(const bool data) : data(data) {};
  string GetType() { return "Boolean"; }
private:
  bool data; // Const?
};

class VentureList : public VentureValue { // Should be references constants?
public:
  VentureList(shared_ptr<VentureValue> car, shared_ptr<VentureValue> cdr)
  : car(car), cdr(cdr) {};
  string GetType() { return "List"; }
private:
  shared_ptr<VentureValue> car; // Const?
  shared_ptr<VentureValue> cdr; // Const?
};

class VentureNil : public VentureValue {
public:
  VentureNil() {};
  string GetType() { return "Nil"; }
private:
};

class VentureProbabilityVector : public VentureValue {
public:
  VentureProbabilityVector(const vector<real> data) // Passing by value. Not efficiently!
  : data(data) {};
  string GetType() { return "ProbabilityVector"; }
private:
  vector<real> data; // Const?
};

class VentureReference : public VentureValue {
public:
  VentureReference(const shared_ptr<VentureValue> reference) : reference(reference) {};
  string GetType() { return "Reference"; }
private:
  shared_ptr<VentureValue> reference; // Const?
};

class VentureToken : public VentureValue {
public:
  VentureToken(const string& token) : token(token) {};
  string GetType() { return "Reference"; }
private:
  const string& token; // Const?
};

#endif
