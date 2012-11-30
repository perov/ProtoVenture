
#ifndef VENTURE___VENTURE_VALUES_H
#define VENTURE___VENTURE_VALUES_H

#include "Header.h"

enum VentureDataTypes { UNDEFINED_TYPE, BOOLEAN, COUNT, REAL, PROBABILITY, ATOM, SIMPLEXPOINT, SMOOTHEDCONTINUOUS, NIL, LIST, SYMBOL, LAMBDA, XRP_REFERENCE, NODE };

struct VentureValue : public boost::enable_shared_from_this<VentureValue> {
  VentureValue() {};
  virtual VentureDataTypes GetType() { return UNDEFINED_TYPE; }
  virtual bool CompareByValue(shared_ptr<VentureValue> another) {
    throw std::exception("Should not be called.");
  }
  virtual string GetString() { return "UNDEFINED"; }
  virtual ~VentureValue() {};
};

template <typename T>
bool VerifyVentureType(shared_ptr<VentureValue>);

template <typename T>
void AssertVentureType(shared_ptr<VentureValue>);

template <typename T>
shared_ptr<T> ToVentureType(shared_ptr<VentureValue>);

struct VentureBoolean : public VentureValue {
  VentureBoolean(const bool data) : data(data) {}
  VentureDataTypes GetType() { return BOOLEAN; }
  bool CompareByValue(shared_ptr<VentureValue>);
  string GetString() {
    if (data == false) {
      return "#f";
    } else {
      return "#t";
    }
  }

  bool data;
};

struct VentureCount : public VentureValue {
  VentureCount(const int data) : data(data) {
    if (data < 0) {
      throw std::exception("VentureCount should be non-negative.");
    }
  }
  // Question: where would be the type transformation happen?
  //           Before this function, it seems?
  VentureDataTypes GetType() { return COUNT; }
  bool CompareByValue(shared_ptr<VentureValue>);
  string GetString() { return boost::lexical_cast<string>(data); }

  int data;
};

struct VentureReal : public VentureValue {
  VentureReal(const real data) : data(data) {}
  VentureDataTypes GetType() { return REAL; }
  bool CompareByValue(shared_ptr<VentureValue>);
  string GetString() { return boost::lexical_cast<string>(data); }

  real data;
};

struct VentureProbability : public VentureValue {
  VentureProbability(const real data) : data(data) {
    if (data < 0.0 || data > 1.0) { // Add acceptable epsilon error.
      throw std::exception("VentureProbability should be non-negative.");
    }
  }
  VentureDataTypes GetType() { return PROBABILITY; }
  bool CompareByValue(shared_ptr<VentureValue>);
  string GetString() { return boost::lexical_cast<string>(data); }

  real data;
};

struct VentureAtom : public VentureValue {
  VentureAtom(const int data) : data(data) {
    if (data < 0) {
      throw std::exception("VentureAtom should be non-negative.");
    }
  }
  // Question: where would be the type transformation happen?
  //           Before this function, it seems?
  VentureDataTypes GetType() { return ATOM; }
  bool CompareByValue(shared_ptr<VentureValue>);
  string GetString() { return boost::lexical_cast<string>(data); }

  int data;
};

struct VentureSimplexPoint : public VentureValue {
  VentureSimplexPoint(vector<real>& input_data)
    // : data(SOMEFUNCTION(data)) -- it should be implemented in this way? 
  {
    if (input_data.size() <= 1) {
      throw std::exception("VentureSimplexPoint should be at least two-dimensional.");
    }
    double sum = 0.0;
    // Change the data size preliminary?
    for (size_t index = 0; index < input_data.size(); index++)
    {
      if (input_data[index] < 0.0) { // Add acceptable epsilon error?
        throw std::exception("VentureSimplexPoint element should be non-negative.");
      }
      if (index + 1 != input_data.size())
      {
        data.push_back(input_data[index]);
      }
      sum += input_data[index];
    }
    if (fabs(sum - 1.0) > comparison_epsilon) {
      throw std::exception("Sum of VentureSimplexPoint elements should be equal to 1.0.");
    }
  }
  // Question: where would be the type transformation happen?
  //           Before this function, it seems?
  VentureDataTypes GetType() { return SIMPLEXPOINT; }
  bool CompareByValue(shared_ptr<VentureValue>);
  string GetString() {
    double sum = 0.0;
    string output = "sp[";
    for (size_t index = 0; index < data.size(); index++)
    {
      output += boost::lexical_cast<string>(data[index]) + ";";
      sum += data[index];
    }
    output += boost::lexical_cast<string>(1 - sum) + "]";
    return output;
  }

  vector<real> data;
};

struct VentureSmoothedContinuous : public VentureValue {
  VentureSmoothedContinuous(const real data) : data(data) {
    if (data <= 0.0) { // Add acceptable epsilon error.
                       // What about zero?
      throw std::exception("VentureProbability should be positive.");
    }
  }
  VentureDataTypes GetType() { return SMOOTHEDCONTINUOUS; }
  bool CompareByValue(shared_ptr<VentureValue>);
  string GetString() { return boost::lexical_cast<string>(data); }

  real data;
};

struct VentureList;

extern shared_ptr<VentureList> const NIL_INSTANCE;

// Should be references constants?
// Should be renamed to the VentureCons!
struct VentureList : public VentureValue {
  VentureList(shared_ptr<VentureValue> car = shared_ptr<VentureValue>())
    : car(car), cdr(NIL_INSTANCE) {}
  VentureDataTypes GetType() { return LIST; } // Should be virtual for NIL?..
  // FIXME: add CompareByValue? Do not forget about the NIL, that it has another type?
  string GetString();

  shared_ptr<VentureValue> car;
  shared_ptr<VentureList> cdr;
};

struct VentureNil : public VentureList {
  VentureNil() {}
  VentureDataTypes GetType() { return NIL; }
  // FIXME: add CompareByValue? Do not forget about the LIST, that it has another type?
  string GetString() { return "#nil"; }
};

// http://stackoverflow.com/questions/2926878/determine-if-a-string-contains-only-alphanumeric-characters-or-a-space
static inline bool is_not_legal_SYMBOL_character(char c)
{
  //return !(isalnum(c) || (c == '-'));
  return !(isgraph(c));
}
static inline bool legal_SYMBOL_name(const string& str)
{
  return find_if(str.begin(), str.end(), is_not_legal_SYMBOL_character) == str.end();
}

struct VentureSymbol : public VentureValue {
  VentureSymbol(const string& symbol) : symbol(symbol) {}
  VentureDataTypes GetType() { return SYMBOL; }
  bool CompareByValue(shared_ptr<VentureValue>);
  string GetString() { return symbol; }

  string symbol;
};

struct NodeEvaluation;
struct NodeEnvironment;
class XRP;

struct VentureLambda : public VentureValue {
  VentureLambda(shared_ptr<VentureList> formal_arguments,
                shared_ptr<NodeEvaluation> expressions,
                shared_ptr<NodeEnvironment> scope_environment)
    : formal_arguments(formal_arguments), expressions(expressions), scope_environment(scope_environment) {}
  VentureDataTypes GetType() { return LAMBDA; }
  bool CompareByValue(shared_ptr<VentureValue>); // We really do not need this function?
  string GetString() { return "LAMBDA"; }

  shared_ptr<VentureList> formal_arguments;
  shared_ptr<NodeEvaluation> expressions;
  shared_ptr<NodeEnvironment> scope_environment;
};

struct VentureXRP : public VentureValue {
  VentureXRP(shared_ptr<XRP> xrp)
    : xrp(xrp) {}
  VentureDataTypes GetType() { return XRP_REFERENCE; }
  bool CompareByValue(shared_ptr<VentureValue>); // We really do not need this function?
  string GetString() { return "XRP_REFERENCE"; }

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

/*
struct VentureReference : public VentureValue {
  VentureReference(const shared_ptr<VentureValue> reference) : reference(reference) {}
  VentureDataTypes GetType() { return REFERENCE; }
  string GetString() { return boost::lexical_cast<string>(reference); }

  shared_ptr<VentureValue> reference;
};
*/

#endif
