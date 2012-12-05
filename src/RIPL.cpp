
#include "RIPL.h"

// Should be called DirectiveEntry!
directive_entry::directive_entry(string directice_as_string,
                shared_ptr<NodeEvaluation> directive_node)
  : directice_as_string(directice_as_string),
    directive_node(directive_node)
{}
directive_entry::directive_entry()
{
  throw std::exception("Do not have default constructor.");
}

shared_ptr<NodeEvaluation> GetLastDirectiveNode() {
  if (directives.size() == 0) {
    throw std::exception("There is no any directive in the trace.");
  }
  return directives.rbegin()->second.directive_node;
}
