
#ifndef VENTURE___VENTURE_VALUES_H
#define VENTURE___VENTURE_VALUES_H

#include "Header.h"

enum VentureDataTypes { UNDEFINED_TYPE, BOOLEAN, COUNT, REAL, PROBABILITY, ATOM, SIMPLEXPOINT, SMOOTHEDCOUNT, NIL, LIST, SYMBOL, LAMBDA, XRP_REFERENCE, NODE };

struct VentureValue : public boost::enable_shared_from_this<VentureValue> {
  VentureValue::VentureValue();
  void CheckMyData();
  virtual VentureDataTypes VentureValue::GetType();
  virtual bool VentureValue::CompareByValue(shared_ptr<VentureValue>);
  virtual string VentureValue::GetString();
  ~VentureValue();
};

template <typename T>
bool VerifyVentureType(shared_ptr<VentureValue>);

template <typename T>
void AssertVentureType(shared_ptr<VentureValue>);

template <typename T>
shared_ptr<T> ToVentureType(shared_ptr<VentureValue>);

struct VentureBoolean : public VentureValue {
  VentureBoolean::VentureBoolean(const bool);
  virtual VentureDataTypes VentureBoolean::GetType();
  virtual bool VentureBoolean::CompareByValue(shared_ptr<VentureValue>);
  virtual string VentureBoolean::GetString();
  ~VentureBoolean();

  bool data;
};

struct VentureCount : public VentureValue {
  VentureCount::VentureCount(const int);
  VentureCount::VentureCount(const string&);
  void CheckMyData();
  // Question: where would be the type transformation happen?
  //           Before this function, it seems?
  virtual VentureDataTypes VentureCount::GetType();
  virtual bool VentureCount::CompareByValue(shared_ptr<VentureValue>);
  virtual string VentureCount::GetString();
  ~VentureCount();

  int data;
};

struct VentureReal : public VentureValue {
  VentureReal::VentureReal(const real);
  virtual VentureDataTypes VentureReal::GetType();
  virtual bool VentureReal::CompareByValue(shared_ptr<VentureValue>);
  virtual string VentureReal::GetString();
  ~VentureReal();

  real data;
};

struct VentureProbability : public VentureValue {
  VentureProbability::VentureProbability(const real);
  virtual VentureDataTypes VentureProbability::GetType();
  virtual bool VentureProbability::CompareByValue(shared_ptr<VentureValue>);
  virtual string VentureProbability::GetString();
  ~VentureProbability();

  real data;
};

struct VentureAtom : public VentureValue {
  VentureAtom::VentureAtom(const int);
  // Question: where would be the type transformation happen?
  //           Before this function, it seems?
  virtual VentureDataTypes VentureAtom::GetType();
  virtual bool VentureAtom::CompareByValue(shared_ptr<VentureValue>);
  virtual string VentureAtom::GetString();
  ~VentureAtom();

  int data;
};

struct VentureSimplexPoint : public VentureValue {
  VentureSimplexPoint::VentureSimplexPoint(vector<real>&);
  // Question: where would be the type transformation happen?
  //           Before this function, it seems?
  virtual VentureDataTypes VentureSimplexPoint::GetType();
  virtual bool VentureSimplexPoint::CompareByValue(shared_ptr<VentureValue>);
  virtual string VentureSimplexPoint::GetString();
  ~VentureSimplexPoint();

  vector<real> data;
};

struct VentureSmoothedCount : public VentureValue {
  VentureSmoothedCount::VentureSmoothedCount(const real);
  virtual VentureDataTypes VentureSmoothedCount::GetType();
  virtual bool VentureSmoothedCount::CompareByValue(shared_ptr<VentureValue>);
  virtual string VentureSmoothedCount::GetString();
  ~VentureSmoothedCount();

  real data;
};

struct VentureList;

extern shared_ptr<VentureList> const NIL_INSTANCE;

// Should be references constants?
// Should be renamed to the VentureCons!
struct VentureList : public VentureValue {
  VentureList::VentureList(shared_ptr<VentureValue> car);
  virtual VentureDataTypes VentureList::GetType(); // Should be virtual for NIL?..
  // FIXME: add CompareByValue? Do not forget about the NIL, that it has another type?
  virtual string VentureList::GetString();
  ~VentureList();

  shared_ptr<VentureValue> car;
  shared_ptr<VentureList> cdr;
};

struct VentureNil : public VentureList {
  VentureNil::VentureNil();
  virtual VentureDataTypes VentureNil::GetType();
  virtual string VentureNil::GetString();
  ~VentureNil();
};

// http://stackoverflow.com/questions/2926878/determine-if-a-string-contains-only-alphanumeric-characters-or-a-space
bool is_not_legal_SYMBOL_character(char); // static inline?
bool legal_SYMBOL_name(const string&); // static inline?

struct VentureSymbol : public VentureValue {
  VentureSymbol::VentureSymbol(const string&);
  virtual VentureDataTypes VentureSymbol::GetType();
  virtual bool VentureSymbol::CompareByValue(shared_ptr<VentureValue>);
  virtual string VentureSymbol::GetString();
  ~VentureSymbol();

  string symbol;
};

struct NodeEvaluation;
struct NodeEnvironment;
class XRP;

struct VentureLambda : public VentureValue {
  VentureLambda::VentureLambda(shared_ptr<VentureList>,
                shared_ptr<NodeEvaluation>,
                shared_ptr<NodeEnvironment>);
  virtual VentureDataTypes VentureLambda::GetType();
  virtual bool VentureLambda::CompareByValue(shared_ptr<VentureValue>); // We really do not need this function?
  virtual string VentureLambda::GetString();
  ~VentureLambda();

  shared_ptr<VentureList> formal_arguments;
  shared_ptr<NodeEvaluation> expressions;
  shared_ptr<NodeEnvironment> scope_environment;
};

struct VentureXRP : public VentureValue {
  VentureXRP::VentureXRP(shared_ptr<XRP>);
  virtual VentureDataTypes VentureXRP::GetType();
  virtual bool VentureXRP::CompareByValue(shared_ptr<VentureValue>); // We really do not need this function?
  virtual string VentureXRP::GetString();
  ~VentureXRP();

  shared_ptr<XRP> xrp;
};

void AssertVentureSymbol(shared_ptr<VentureValue>);

shared_ptr<VentureSymbol> ToVentureSymbol(shared_ptr<VentureValue>);

void AssertVentureList(shared_ptr<VentureValue>);

shared_ptr<VentureList> ToVentureList(shared_ptr<VentureValue>);

shared_ptr<VentureValue> GetFirst(shared_ptr<VentureList>);

shared_ptr<VentureList> GetNext(shared_ptr<VentureList>);

shared_ptr<VentureValue> GetNth(shared_ptr<VentureList>, size_t);

void AddToList(shared_ptr<VentureList>, shared_ptr<VentureValue>);

bool StandardPredicate(shared_ptr<VentureValue>);

#endif
