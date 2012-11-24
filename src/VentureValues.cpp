
#include "Header.h"
#include "VentureValues.h"
#include "VentureParser.h"

string VentureList::GetString() {
  std::string output("(");
  VentureList* iterator = this;
  while (iterator->GetType() != NIL) {
    if (iterator != this) { output += " "; }
    output += Stringify(iterator->car);
    iterator = iterator->cdr;
  }
  output += ")";
  return output;
}

void AssertVentureSymbol(VentureValue* value) {
  if (dynamic_cast<VentureSymbol*>(value) == 0 || value->GetType() != SYMBOL) {
    throw std::exception("Assertion: it is not a symbol.");
  }
}

VentureSymbol* ToVentureSymbol(VentureValue* value_reference) {
  AssertVentureSymbol(value_reference);
  VentureSymbol* return_reference = dynamic_cast<VentureSymbol*>(value_reference);
  return return_reference;
}

void AssertVentureList(VentureValue* value) {
  if (dynamic_cast<VentureList*>(value) == 0) { // If dynamic_cast returns NULL.
    throw std::exception("Assertion: it is not a list.");
  }
}

VentureList* ToVentureList(VentureValue* value_reference) {
  AssertVentureList(value_reference);
  VentureList* return_reference = dynamic_cast<VentureList*>(value_reference);
  return return_reference;
}
