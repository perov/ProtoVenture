
#include "Header.h"
#include "VentureParser.h"
#include "VentureValues.h"
#include "boost/shared_ptr.hpp"
#include "boost/lexical_cast.hpp"
#include "boost/algorithm/string.hpp"

// Parser implementation is inspired by: http://howtowriteaprogram.blogspot.com/2010/11/lisp-interpreter-in-90-lines-of-c.html

bool IsInteger(const string& input) {
  try {
    boost::lexical_cast<int>(input);
  } catch(boost::bad_lexical_cast&) {
    return false;
  }
  return true;
}

bool IsReal(const string& input) {
  try {
    boost::lexical_cast<real>(input);
  } catch(boost::bad_lexical_cast&) {
    return false;
  }
  return true;
}

string ToLower(string processing_string) {
  boost::algorithm::to_lower(processing_string);
  return processing_string;
}

list<string> Tokenize(const string& code)
{
  list<string> tokens;
  const char* s = code.c_str();
  while (*s) {
    while (*s == ' ')
      ++s;
    if (*s == '(' || *s == ')')
      tokens.push_back(*s++ == '(' ? "(" : ")");
    else {
      const char* t = s;
      while (*t && *t != ' ' && *t != '(' && *t != ')')
        ++t;
      tokens.push_back(string(s, t));
      s = t;
    }
  }
  return tokens;
}

VentureValue* ProcessAtom(const string& token)
{
  if (IsInteger(token)) {
    return new VentureInteger(boost::lexical_cast<int>(token));
  } else if (IsReal(token)) {
    return new VentureReal(boost::lexical_cast<real>(token));
  } else if (ToLower(token) == "nil") {
    return (VentureValue*)(NIL_INSTANCE);
  } else if (ToLower(token) == "false") {
    return new VentureBoolean(false);
  } else if (ToLower(token) == "true") {
    return new VentureBoolean(true);
  } else { // FIXME: make check on the symbol format!
    return new VentureSymbol(token);
  }
}

VentureValue* ProcessTokens(list<string>& tokens)
{
  const std::string token(tokens.front());
  tokens.pop_front();
  if (token == "(") {
    VentureList* new_list = NIL_INSTANCE;
    VentureList* last_cons = NIL_INSTANCE;
    while (tokens.front() != ")")
    {
      VentureValue* next_element = ProcessTokens(tokens);
      if (new_list == NIL_INSTANCE) { // First element.
        new_list = new VentureList(next_element, NIL_INSTANCE);
        last_cons = new_list;
      } else {
        last_cons->cdr = new VentureList(next_element, NIL_INSTANCE);
        last_cons = last_cons->cdr;
      }
    }
    tokens.pop_front();
    return new_list;
  } else {
    return ProcessAtom(token);
  }
}

VentureValue* ReadCode(const string& input)
{
  list<string> tokens(Tokenize(input));
  return ProcessTokens(tokens);
}

string Stringify(VentureValue* const value) {
  if (value->GetType() == NIL) {
    return "#nil";
  } else {
    return value->GetString();
  }
}

bool CompareValue(VentureSymbol* first_value, string second_value) { // Should be inline.
  return first_value->symbol == second_value;
}

bool CompareValue(string first_value, VentureSymbol* const second_value) { // Should be inline.
  return first_value == second_value->symbol;
}

bool CompareValue(VentureSymbol* const first_value, VentureSymbol* const second_value) { // Should be inline.
  return first_value->symbol == second_value->symbol;
}
