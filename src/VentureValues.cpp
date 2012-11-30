
#include "Header.h"
#include "VentureValues.h"
#include "VentureParser.h"

string VentureList::GetString() {
  std::string output("(");
  shared_ptr<VentureList> iterator = shared_ptr<VentureList>(this);
  while (iterator->GetType() != NIL) {
    if (iterator != shared_ptr<VentureList>(this)) { output += " "; }
    output += Stringify(iterator->car);
    iterator = iterator->cdr;
  }
  output += ")";
  return output;
}

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

template <typename T>
bool VerifyVentureType(shared_ptr<VentureValue> value) {
  return dynamic_cast<T*>(value.get()) != 0;
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

bool VentureSmoothedContinuous::CompareByValue(shared_ptr<VentureValue> another) {
  return (this->data == ToVentureType<VentureSmoothedContinuous>(another)->data);
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

bool StandardPredicate(shared_ptr<VentureValue> value) {
  if (VerifyVentureType<VentureBoolean>(value) == true &&
        CompareValue(value, shared_ptr<VentureValue>(new VentureBoolean(false)))) {
    return false;
  } else {
    return true;
  }
}
