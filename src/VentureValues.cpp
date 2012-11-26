
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
