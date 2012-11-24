
#ifndef VENTURE___VENTURE_PARSER_H
#define VENTURE___VENTURE_PARSER_H

// Parsed implementation is inspired by: http://howtowriteaprogram.blogspot.com/2010/11/lisp-interpreter-in-90-lines-of-c.html

#include "Header.h"
#include "VentureValues.h"

bool IsInteger(const string&);

bool IsReal(const string&);

string ToLower(string);

list<string> Tokenize(const string&);

VentureValue* ProcessAtom(const string&);

VentureValue* ProcessTokens(list<string>&);

VentureValue* ReadCode(const string&);

string Stringify(VentureValue* const);

bool CompareValue(VentureSymbol*, string);
bool CompareValue(string, VentureSymbol* const);
bool CompareValue(VentureSymbol* const, VentureSymbol* const);

#endif
