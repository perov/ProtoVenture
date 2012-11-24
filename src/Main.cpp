
#include "Header.h"

#include "VentureValues.h"
#include "VentureParser.h"
#include "Analyzer.h"

VentureList* const NIL_INSTANCE = new VentureNil();
size_t NEXT_UNIQUE_ID = 0;

int main()
{
  cout << "Hello\n\n";
  
  /*
  VentureValue* code = ReadCode("(ASSUME a (+ 1 5))");

  try {
    AnalyzeDirective(code);
    cout << "Finished" << endl;
  } catch(std::exception& e) {
    cout << "Error: " << e.what() << endl;
  } catch(...) {
    cout << "Strange exception." << endl;
  }
  */
  
  cout << "\n!: " << NIL_INSTANCE->GetType() << endl;

  return 0;
}
