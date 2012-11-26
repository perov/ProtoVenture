
#ifndef VENTURE___VENTURE_VALUES_H
#define VENTURE___VENTURE_VALUES_H

#include "Header.h"
#include "XRP.h"

enum VentureDataTypes { UNDEFINED_TYPE, INTEGER, REAL, BOOLEAN, NIL, LIST, SYMBOL, LAMBDA, XRP, NODE };

struct VentureValue {
  VentureValue() {};
  virtual VentureDataTypes GetType() { return UNDEFINED_TYPE; }
  virtual string GetString() { return "UNDEFINED"; }
  virtual ~VentureValue() {};
};

struct VentureInteger : public VentureValue {
  VentureInteger(const int data) : data(data) {}
  // Question: where would be the type transformation happen?
  //           Before this function, it seems?
  VentureDataTypes GetType() { return INTEGER; }
  string GetString() { return boost::lexical_cast<string>(data); }

  int data;
};

struct VentureReal : public VentureValue {
  VentureReal(const real data) : data(data) {}
  VentureDataTypes GetType() { return REAL; }
  string GetString() { return boost::lexical_cast<string>(data); }

  real data;
};

struct VentureBoolean : public VentureValue {
  VentureBoolean(const bool data) : data(data) {}
  VentureDataTypes GetType() { return BOOLEAN; }
  string GetString() {
    if (data == false) {
      return "#f";
    } else {
      return "#t";
    }
  }

  bool data;
};

// Should be references constants?
// Should be renamed to the VentureCons!
struct VentureList : public VentureValue {
  VentureList(shared_ptr<VentureValue> car = shared_ptr<VentureValue>(),
              shared_ptr<VentureList> cdr = shared_ptr<VentureList>())
    : car(car), cdr(cdr) {}
  VentureDataTypes GetType() { return LIST; } // Should be virtual for NIL?..
  string GetString();

  shared_ptr<VentureValue> car;
  shared_ptr<VentureList> cdr;
};

struct VentureNil : public VentureList {
  VentureNil() {}
  VentureDataTypes GetType() { return NIL; }
  string GetString() { return "#nil"; }
};

extern shared_ptr<VentureList> const NIL_INSTANCE;

struct VentureSymbol : public VentureValue {
  VentureSymbol(const string& symbol) : symbol(symbol) {}
  VentureDataTypes GetType() { return SYMBOL; }
  string GetString() { return symbol; }

  string symbol;
};

struct VentureLambda : public VentureValue {
  VentureLambda(shared_ptr<VentureList> formal_arguments,
                shared_ptr<NodeEvaluation> expressions,
                shared_ptr<NodeEnvironment> scope_environment)
    : formal_arguments(formal_arguments), expressions(expressions), scope_environment(scope_environment) {}
  VentureDataTypes GetType() { return LAMBDA; }
  string GetString() { return "LAMBDA"; }

  shared_ptr<VentureList> formal_arguments;
  shared_ptr<NodeEvaluation> expressions;
  shared_ptr<NodeEnvironment> scope_environment;
};

struct VentureXRP : public VentureValue {
  VentureXRP(shared_ptr<XRP> xrp)
    : xrp(xrp) {}
  VentureDataTypes GetType() { return XRP; }
  string GetString() { return "XRP"; }

  shared_ptr<XRP> xrp;
};

void AssertVentureSymbol(shared_ptr<VentureValue>);

shared_ptr<VentureSymbol> ToVentureSymbol(shared_ptr<VentureValue>);

void AssertVentureList(shared_ptr<VentureValue>);

shared_ptr<VentureList> ToVentureList(shared_ptr<VentureValue>);

/*
struct VentureReference : public VentureValue {
  VentureReference(const shared_ptr<VentureValue> reference) : reference(reference) {}
  VentureDataTypes GetType() { return REFERENCE; }
  string GetString() { return boost::lexical_cast<string>(reference); }

  shared_ptr<VentureValue> reference;
};
*/

#endif
