
#include "Header.h"
#include "VentureValues.h"
#include "VentureParser.h"

void VentureValue::CheckMyData() {
  // This is standard blank checker.
}

void VentureCount::CheckMyData() {
  if (data < 0) {
    throw std::exception("VentureCount should be non-negative.");
  }
}

// *** Constructors ***
VentureValue::VentureValue() {
  // cout << "Create: " << this << endl;
}
VentureBoolean::VentureBoolean(const bool data) : data(data) {}
VentureCount::VentureCount(const int data) : data(data) {
  this->CheckMyData();
}
VentureCount::VentureCount(const string& input) : data(boost::lexical_cast<int>(data)) {
  this->CheckMyData();
}
VentureReal::VentureReal(const real data) : data(data) {}
VentureProbability::VentureProbability(const real data) : data(data) {
  if (data < 0.0 || data > 1.0) { // Add acceptable epsilon error.
    throw std::exception("VentureProbability should be non-negative.");
  }
}
VentureSimplexPoint::VentureSimplexPoint(vector<real>& input_data)
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
VentureAtom::VentureAtom(const int data) : data(data) {
  if (data < 0) {
    throw std::exception("VentureAtom should be non-negative.");
  }
}
VentureSmoothedCount::VentureSmoothedCount(const real data) : data(data) {
  if (data <= 0.0) { // Add acceptable epsilon error.
                      // What about zero?
    throw std::exception("VentureProbability should be positive.");
  }
}
VentureNil::VentureNil() : VentureList(shared_ptr<VentureValue>()) {}
VentureList::VentureList(shared_ptr<VentureValue> car)
  : car(car), cdr(NIL_INSTANCE) {}
VentureXRP::VentureXRP(shared_ptr<XRP> xrp)
  : xrp(xrp) {}
VentureSymbol::VentureSymbol(const string& symbol) : symbol(symbol) {}
VentureLambda::VentureLambda(shared_ptr<VentureList> formal_arguments,
              shared_ptr<NodeEvaluation> expressions,
              shared_ptr<NodeEnvironment> scope_environment)
  : formal_arguments(formal_arguments), expressions(expressions), scope_environment(scope_environment) {}

// *** Destructors ***
VentureValue::~VentureValue() {
  //cout << "Delete: " << this << " " << GetType() << " " << GetString() << endl;
}
// From VentureValues.h
/*
VentureBoolean::~VentureBoolean() { cout << "Deleting: VentureBoolean" << endl; }
VentureCount::~VentureCount() { cout << "Deleting: VentureCount" << endl; }
VentureReal::~VentureReal() { cout << "Deleting: VentureReal" << endl; }
VentureProbability::~VentureProbability() { cout << "Deleting: VentureProbability" << endl; }
VentureAtom::~VentureAtom() { cout << "Deleting: VentureAtom" << endl; }
VentureSimplexPoint::~VentureSimplexPoint() { cout << "Deleting: VentureSimplexPoint" << endl; }
VentureSmoothedCount::~VentureSmoothedCount() { cout << "Deleting: VentureSmoothedCount" << endl; }
VentureList::~VentureList() { cout << "Deleting: VentureList" << endl; }
VentureNil::~VentureNil() { cout << "Deleting: VentureNil" << endl; }
VentureSymbol::~VentureSymbol() { cout << "Deleting: VentureSymbol" << endl; }
VentureLambda::~VentureLambda() { cout << "Deleting: VentureLambda" << endl; }
VentureXRP::~VentureXRP() { cout << "Deleting: VentureXRP" << endl; }
*/
VentureBoolean::~VentureBoolean() {}
VentureCount::~VentureCount() {}
VentureReal::~VentureReal() {}
VentureProbability::~VentureProbability() {}
VentureAtom::~VentureAtom() {}
VentureSimplexPoint::~VentureSimplexPoint() {}
VentureSmoothedCount::~VentureSmoothedCount() {}
VentureList::~VentureList() {}
VentureNil::~VentureNil() {}
VentureSymbol::~VentureSymbol() {}
VentureLambda::~VentureLambda() {}
VentureXRP::~VentureXRP() {}

// *** GetType ***
VentureDataTypes VentureValue::GetType() { return UNDEFINED_TYPE; }
VentureDataTypes VentureBoolean::GetType() { return BOOLEAN; }
VentureDataTypes VentureCount::GetType() { return COUNT; }
VentureDataTypes VentureReal::GetType() { return REAL; }
VentureDataTypes VentureProbability::GetType() { return PROBABILITY; }
VentureDataTypes VentureAtom::GetType() { return ATOM; }
VentureDataTypes VentureSimplexPoint::GetType() { return SIMPLEXPOINT; }
VentureDataTypes VentureSmoothedCount::GetType() { return SMOOTHEDCOUNT; }
VentureDataTypes VentureList::GetType() { return LIST; }
VentureDataTypes VentureNil::GetType() { return NIL; }
VentureDataTypes VentureSymbol::GetType() { return SYMBOL; }
VentureDataTypes VentureXRP::GetType() { return XRP_REFERENCE; }
VentureDataTypes VentureLambda::GetType() { return LAMBDA; }

// *** GetString ***
string VentureValue::GetString() { return "UNDEFINED"; }
string VentureBoolean::GetString() {
  if (data == false) {
    return "#f";
  } else {
    return "#t";
  }
}
string VentureCount::GetString() { return boost::lexical_cast<string>(data); }
string VentureReal::GetString() { return boost::lexical_cast<string>(data); }
string VentureAtom::GetString() { return boost::lexical_cast<string>(data); }
string VentureProbability::GetString() { return boost::lexical_cast<string>(data); }
string VentureSimplexPoint::GetString() {
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
string VentureSmoothedCount::GetString() { return boost::lexical_cast<string>(data); }
string VentureNil::GetString() { return "#nil"; }
string VentureSymbol::GetString() { return symbol; }
string VentureLambda::GetString() { return "LAMBDA"; }
string VentureXRP::GetString() { return "XRP_REFERENCE"; }
string VentureList::GetString() {
  std::string output("(");
  shared_ptr<VentureList> iterator = dynamic_pointer_cast<VentureList>(this->shared_from_this());
  while (iterator->GetType() != NIL) {
    assert(iterator != NIL_INSTANCE);
    if (iterator != this->shared_from_this()) { output += " "; }
    output += Stringify(iterator->car);
    iterator = iterator->cdr;
  }
  output += ")";
  return output;
}

// *** Function for VentureSymbol ***
bool is_not_legal_SYMBOL_character(char c)
{
  //return !(isalnum(c) || (c == '-'));
  return !(isgraph(c));
}
bool legal_SYMBOL_name(const string& str)
{
  return find_if(str.begin(), str.end(), is_not_legal_SYMBOL_character) == str.end();
}

// *** Function for VentureType control and transfer ***
template <typename T>
bool VerifyVentureType(shared_ptr<VentureValue> value) {
  return dynamic_pointer_cast<T>(value) != shared_ptr<T>();
}
template <typename T>
void AssertVentureType(shared_ptr<VentureValue> value) {
  if (!VerifyVentureType<T>(value)) {
    throw std::exception("Assertion: not the right type.");
  }
}
template <typename T>
shared_ptr<T> ToVentureType(shared_ptr<VentureValue> value) {
  AssertVentureType<T>(value);
  return dynamic_pointer_cast<T>(value);
}

// *** CompareByValue ***
bool VentureValue::CompareByValue(shared_ptr<VentureValue> another) {
  throw std::exception("Should not be called.");
}
bool VentureSymbol::CompareByValue(shared_ptr<VentureValue> another) {
  return (this->symbol == ToVentureType<VentureSymbol>(another)->symbol);
}
bool VentureBoolean::CompareByValue(shared_ptr<VentureValue> another) {
  return (this->data == ToVentureType<VentureBoolean>(another)->data);
}
bool VentureCount::CompareByValue(shared_ptr<VentureValue> another) {
  return (this->data == ToVentureType<VentureCount>(another)->data);
}
bool VentureReal::CompareByValue(shared_ptr<VentureValue> another) {
  return (this->data == ToVentureType<VentureReal>(another)->data);
}
bool VentureProbability::CompareByValue(shared_ptr<VentureValue> another) {
  return (this->data == ToVentureType<VentureProbability>(another)->data);
}
bool VentureAtom::CompareByValue(shared_ptr<VentureValue> another) {
  return (this->data == ToVentureType<VentureAtom>(another)->data);
}
bool VentureSimplexPoint::CompareByValue(shared_ptr<VentureValue> another) {
  return (this->data == ToVentureType<VentureSimplexPoint>(another)->data);
}
bool VentureSmoothedCount::CompareByValue(shared_ptr<VentureValue> another) {
  return (this->data == ToVentureType<VentureSmoothedCount>(another)->data);
}
// We do not need this function?
// But without this function ToVentureType<VentureLambda>(...)
// would be unresolved?
// Added later: I use it for figuring out if the operator has not changed or not.
bool VentureLambda::CompareByValue(shared_ptr<VentureValue> another) {
  // It is enough to compare by expressions: ?
  return (this->expressions == ToVentureType<VentureLambda>(another)->expressions);
}
// We do not need this function?
// But without this function ToVentureType<VentureLambda>(...)
// would be unresolved?
// Added later: I use it for figuring out if the operator has not changed or not.
bool VentureXRP::CompareByValue(shared_ptr<VentureValue> another) {
  // It is enough to compare by expressions: ?
  return (this->xrp == ToVentureType<VentureXRP>(another)->xrp);
}

// *** Functions for list ***
shared_ptr<VentureValue> GetFirst(shared_ptr<VentureList> list) {
  if (list->GetType() == NIL) {
    return list;
  } else {
    return list->car;
  }
}
shared_ptr<VentureList> GetNext(shared_ptr<VentureList> list) {
  if (list->GetType() == NIL) {
    return list;
  } else {
    return list->cdr;
  }
}
shared_ptr<VentureValue> GetNth(shared_ptr<VentureList> list, size_t position) { // Enumeration from "1"!
  for (size_t index = 1; index < position; index++) {
    list = GetNext(list);
  }
  return GetFirst(list);
}
void AddToList(shared_ptr<VentureList> target_list, shared_ptr<VentureValue> element) {
  if (target_list->GetType() == NIL) {
    throw std::exception("Function AddToList should not change NIL_INSTANCE.");
  }
  while (target_list->cdr != NIL_INSTANCE) {
    target_list = GetNext(target_list);
  }
  target_list->cdr = shared_ptr<VentureList>(new VentureList(element));
}

// *** Different things ***
bool StandardPredicate(shared_ptr<VentureValue> value) {
  if (VerifyVentureType<VentureBoolean>(value) == true &&
        CompareValue(value, shared_ptr<VentureValue>(new VentureBoolean(false)))) {
    return false;
  } else {
    return true;
  }
}

// *** DEPRECATED THINGS FOR WORK WITH VentureTypes ***
void AssertVentureSymbol(shared_ptr<VentureValue> value) {
  if (dynamic_cast<VentureSymbol*>(value.get()) == 0 || value->GetType() != SYMBOL) {
    throw std::exception(string(string("Assertion: it is not a symbol.") + value->GetString() + string(boost::lexical_cast<string>(value->GetType()))).c_str());
  }
}
shared_ptr<VentureSymbol> ToVentureSymbol(shared_ptr<VentureValue> value_reference) {
  AssertVentureSymbol(value_reference);
  shared_ptr<VentureSymbol> return_reference = dynamic_pointer_cast<VentureSymbol>(value_reference);
  return return_reference;
}
void AssertVentureList(shared_ptr<VentureValue> value) {;
  if (dynamic_cast<VentureList*>(value.get()) == 0) { // If dynamic_cast returns NULL.
    throw std::exception(string(string("Assertion: it is not a list.") + value->GetString() + string(boost::lexical_cast<string>(value->GetType()))).c_str());
  }
}
shared_ptr<VentureList> ToVentureList(shared_ptr<VentureValue> value_reference) {
  AssertVentureList(value_reference);
  shared_ptr<VentureList> return_reference = dynamic_pointer_cast<VentureList>(value_reference);
  return return_reference;
}
