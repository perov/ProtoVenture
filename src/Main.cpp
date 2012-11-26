
#include "Header.h"

#include "VentureValues.h"
#include "VentureParser.h"
#include "Analyzer.h"
#include "Evaluator.h"
#include "XRP.h"

shared_ptr<VentureList> const NIL_INSTANCE = shared_ptr<VentureList>(new VentureNil());
size_t NEXT_UNIQUE_ID = 0;

int main()
{
  cout << "Hello\n\n";

  shared_ptr<NodeEnvironment> global_environment =
    shared_ptr<NodeEnvironment>(new NodeEnvironment(shared_ptr<NodeEnvironment>()));

  BindToEnvironment(global_environment,
                    shared_ptr<VentureSymbol>(new VentureSymbol("+")), // Make just via the std::string?
                    shared_ptr<VentureXRP>(new Primitive__Sum()));
  
  //shared_ptr<VentureValue> code = ReadCode("(ASSUME a (+ 1 5))");
  shared_ptr<VentureValue> code = ReadCode("(ASSUME a (+ 1 5))");

  ///*
  try {
    AnalyzeDirective(code);
    cout << "Finished" << endl;
  } catch(std::exception& e) {
    cout << "Error: " << e.what() << endl;
  } catch(...) {
    cout << "Strange exception." << endl;
  }
  //*/

  cout << "\n!: " << NIL_INSTANCE->GetType() << endl;

  return 0;
}
