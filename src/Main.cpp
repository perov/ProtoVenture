
#include "Header.h"

#include "VentureValues.h"
#include "VentureParser.h"
#include "Analyzer.h"
#include "Evaluator.h"
#include "XRP.h"

shared_ptr<VentureList> const NIL_INSTANCE = shared_ptr<VentureList>(new VentureNil());
size_t NEXT_UNIQUE_ID = 0;
gsl_rng* random_generator = 0;
set< shared_ptr<NodeXRPApplication> > random_choices;
size_t DIRECTIVE_COUNTER = 0;

int main()
{
  cout << "SMOOTHEDCONTINUOUS should be SMOOTHEDCOUNT" << endl;
  cout << "See why: Or just NULL? does not work!" << endl;

  random_generator = gsl_rng_alloc(gsl_rng_mt19937);
  gsl_rng_set(random_generator, abs(time(NULL)));

  shared_ptr<NodeEnvironment> global_environment =
    shared_ptr<NodeEnvironment>(new NodeEnvironment(shared_ptr<NodeEnvironment>()));
  
  BindToEnvironment(global_environment,
                    shared_ptr<VentureSymbol>(new VentureSymbol("real+")), // Make just via the std::string?
                    shared_ptr<VentureXRP>(new VentureXRP(shared_ptr<XRP>(new Primitive__RealPlus()))));
  
  BindToEnvironment(global_environment,
                    shared_ptr<VentureSymbol>(new VentureSymbol("flip")), // Make just via the std::string?
                    shared_ptr<VentureXRP>(new VentureXRP(shared_ptr<XRP>(new ERP__Flip()))));

  BindToEnvironment(global_environment,
                    shared_ptr<VentureSymbol>(new VentureSymbol("condition-ERP")), // Make just via the std::string?
                    shared_ptr<VentureXRP>(new VentureXRP(shared_ptr<XRP>(new ERP__ConditionERP()))));
  
  try {
    //string directive_code = "(PREDICT ((lambda (x) r[0.3] (real+ x r[0.5] r[0.2])) r[7]))";
    string directive_code; // = "(PREDICT (if (flip) r[0.2] r[0.3]))";
    shared_ptr<NodeEvaluation> directive_node, new_directive_node;
    shared_ptr<VentureValue> code;

    directive_code = "(ASSUME cloudy (flip r[0.5]))";
    directive_node = AnalyzeDirective(ReadCode(directive_code));
    Evaluator(directive_node, global_environment, shared_ptr<Node>(), shared_ptr<NodeEvaluation>());
    
    directive_code = "(ASSUME sprinkler (flip (if cloudy r[0.1] r[0.5])))";
    new_directive_node = AnalyzeDirective(ReadCode(directive_code));
    Evaluator(new_directive_node, global_environment, shared_ptr<Node>(), shared_ptr<NodeEvaluation>());
    new_directive_node->earlier_evaluation_nodes = directive_node;
    directive_node = new_directive_node;
    
    directive_code = "(ASSUME rain (flip (if cloudy r[0.8] r[0.2])))";
    new_directive_node = AnalyzeDirective(ReadCode(directive_code));
    Evaluator(new_directive_node, global_environment, shared_ptr<Node>(), shared_ptr<NodeEvaluation>());
    new_directive_node->earlier_evaluation_nodes = directive_node;
    directive_node = new_directive_node;

    directive_code = "(ASSUME wet (flip (if sprinkler (if rain r[0.99] r[0.9]) (if rain r[0.9] r[0.01]))))";
    new_directive_node = AnalyzeDirective(ReadCode(directive_code));
    Evaluator(new_directive_node, global_environment, shared_ptr<Node>(), shared_ptr<NodeEvaluation>());
    new_directive_node->earlier_evaluation_nodes = directive_node;
    directive_node = new_directive_node;
    
    for (size_t attempt = 0; attempt < 100; attempt++) {
      clock_t t = clock();

      double count = 0.0;
      double trues = 0.0;
      for (size_t index = 0; index < 100000; index++) {
        MakeMHProposal();
        //cout << global_environment->variables["wet"]->value->GetString() << endl;
        count++;
        if (CompareValue(global_environment->variables["wet"]->value, shared_ptr<VentureBoolean>(new VentureBoolean(true)))) {
          trues++;
        }
      }
      cout << (trues / count) << endl;

      t = clock() - t;
      cout << "Clicks: " << t << "; seconds: " << ((float)t)/CLOCKS_PER_SEC << endl;
    }

    char tmp;
    cin >> tmp;

    /*
    cout << "Finished with result: '" << result << "'" << endl;
    cout << random_choices.size() << endl;
    */
  } catch(std::exception& e) {
    cout << "Error: " << e.what() << endl;
  } catch(...) {
    cout << "Strange exception." << endl;
  }

  return 0;
}
