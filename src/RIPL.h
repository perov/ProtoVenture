
#ifndef VENTURE___RIPL_H
#define VENTURE___RIPL_H

#include "Header.h"
#include "Analyzer.h"

struct directive_entry {
  // Notice:
  // Could be implemented as "class",
  // i.e. has a constructor and a destructor,
  // and i.e. some issues below could be
  // done here (I mean deletion of all nodes).
  directive_entry::directive_entry(string directice_as_string,
                  shared_ptr<NodeEvaluation> directive_node);
  directive_entry::directive_entry();

  string directice_as_string;
  shared_ptr<NodeEvaluation> directive_node;
};
extern shared_ptr<NodeEnvironment> global_environment;
extern size_t last_directive_id;
extern map<size_t, directive_entry> directives;

shared_ptr<NodeEvaluation> GetLastDirectiveNode();

#endif
