#include "Header.h"
#include "VentureParser.h"
#include "VentureValues.h"
#include "boost/shared_ptr.hpp"
#include "boost/lexical_cast.hpp"
#include "boost/algorithm/string.hpp"

// Parsed implementation is inspired by: http://howtowriteaprogram.blogspot.com/2010/11/lisp-interpreter-in-90-lines-of-c.html

bool IsInteger(const string& input) {
  try {
    boost::lexical_cast<int>(input);
  } catch(boost::bad_lexical_cast& e) {
    return false;
  }
  return true;
}

bool IsReal(const string& input) {
  try {
    boost::lexical_cast<real>(input);
  } catch(boost::bad_lexical_cast& e) {
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
      const char * t = s;
      while (*t && *t != ' ' && *t != '(' && *t != ')')
        ++t;
      tokens.push_back(string(s, t));
      s = t;
    }
  }
  return tokens;
}

shared_ptr<VentureValue> ProcessAtom(const std::string& token) // 
{
  if (IsInteger(token)) {
    return shared_ptr<VentureValue>(new VentureInteger(boost::lexical_cast<int>(token)));
  } else if (IsReal(token)) {
    return shared_ptr<VentureValue>(new VentureReal(boost::lexical_cast<real>(token)));
  } else if (ToLower(token) == "nil") {
    return shared_ptr<VentureValue>(new VentureNil());
  } else if (ToLower(token) == "false") {
    return shared_ptr<VentureValue>(new VentureBoolean(false));
  } else if (ToLower(token) == "true") {
    return shared_ptr<VentureValue>(new VentureBoolean(true));
  } else {
    return shared_ptr<VentureValue>(new VentureToken(token));
  }
}

shared_ptr<VentureValue> ProcessTokens(list<string>& tokens)
{
  const std::string token(tokens.front());
  tokens.pop_front();
  if (token == "(") {
    shared_ptr<VentureValue> new_list = shared_ptr<VentureValue>(new VentureNil());
    shared_ptr<VentureValue> list_end = new_list;
    while (tokens.front() != ")")
    {
      // Function *AddToList* should return the reference
      // to the "cdr" of updated last list element.
      list_end = AddToList(list_end, ProcessTokens(tokens));
    }
    tokens.pop_front();
    return list_end;
  } else {
    return ProcessAtom(token);
  }
}

shared_ptr<VentureValue> ReadCode(const std::string& s)
{
  std::list<std::string> tokens(Tokenize(s));
  return ProcessTokens(tokens);
}
