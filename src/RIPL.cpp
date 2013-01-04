
#include "Header.h"
#include "RIPL.h"

// Should be called DirectiveEntry!
directive_entry::directive_entry(string directice_as_string,
                shared_ptr<NodeEvaluation> directive_node)
  : directice_as_string(directice_as_string),
    directive_node(directive_node)
{}
directive_entry::directive_entry()
{
  throw std::runtime_error("Do not have default constructor.");
}

shared_ptr<NodeEvaluation> GetLastDirectiveNode() {
  if (directives.size() == 0) {
    throw std::runtime_error("There is no any directive in the trace.");
  }
  return directives.rbegin()->second.directive_node;
}

// http://cs.gmu.edu/~white/CS571/Examples/Pthread/create.c
void* ContinuousInference(void* arguments) {
  while (continuous_inference_status != 0) {
    //cout << " <I> ";
    if (continuous_inference_status == 1) {
      try {
        MakeMHProposal(0);
      } catch (std::runtime_error& e) {
        continuous_inference_status = 0;
        cout << "Exception has been raised during continuous inference: " << e.what() << endl;
        return NULL;
      }
    } else if (continuous_inference_status == 2) {
      continuous_inference_status = 3;
      while (continuous_inference_status == 3) {
        //cout << "SleepingC" << endl;
        struct timespec ts = {0, 1}; // 1 ns -- is it okay? :)
#ifdef _MSC_VER
        pthread_delay_np(&ts);
#else
        nanosleep(&ts, NULL);
#endif
      }
    }
  }
  pthread_exit(0);
  return NULL;
}

void PauseInference() {
  if (continuous_inference_status == 1) {
    need_to_return_inference = true;
    continuous_inference_status = 2;
    clock_t start_time = clock();
    while (continuous_inference_status != 3) {
      if ((( static_cast<float>(clock() - start_time)) / CLOCKS_PER_SEC) > 3.0) {
        throw std::runtime_error("Cannot stop the inference for more than 3 seconds.");
      }
      struct timespec ts = {0, 1}; // 1 ns -- is it okay? :)
#ifdef _MSC_VER
      pthread_delay_np(&ts);
#else
      nanosleep(&ts, NULL);
#endif
    }
  } else {
    need_to_return_inference = false;
  }
}

void ReturnInferenceIfNecessary() {
  if (need_to_return_inference == true) {
    continuous_inference_status = 1;
  }
}

void DeleteRIPL() {
  // Reconsider this function.
  for (map<size_t, directive_entry>::iterator iterator = directives.begin(); iterator != directives.end(); iterator++) {
    DeleteBranch(iterator->second.directive_node);
  }

  if (global_environment != shared_ptr<NodeEnvironment>()) { // It should not be in a good way?
    global_environment->DeleteNode();
  }
  global_environment = shared_ptr<NodeEnvironment>();

  last_directive_id = 0;
  directives.clear();
}

void ClearRIPL()
{
  continuous_inference_status = 0;

  DeleteRIPL();
  InitRIPL();
}

void InitRIPL() {
  global_environment = shared_ptr<NodeEnvironment>(new NodeEnvironment(shared_ptr<NodeEnvironment>()));
  
  BindStandardElementsToGlobalEnvironment();

  last_directive_id = 0;
  DIRECTIVE_COUNTER = 0; // For "myorder".
  next_gensym_atom = 0;
}

shared_ptr<VentureValue> ReportValue(size_t directive_id) {
  if (directives.count(directive_id) == 0) {
    throw std::runtime_error("Attempt to report value neither for non-existent directive.");
  }
  if (directives[directive_id].directive_node->GetNodeType() == DIRECTIVE_ASSUME) {
    return dynamic_pointer_cast<NodeDirectiveAssume>(directives[directive_id].directive_node)->my_value;
  } else if (directives[directive_id].directive_node->GetNodeType() == DIRECTIVE_PREDICT) {
    return dynamic_pointer_cast<NodeDirectivePredict>(directives[directive_id].directive_node)->my_value;
  } else {
    throw std::runtime_error("Attempt to report value neither for ASSUME nor PREDICT.");
  }
}

void ForgetDirective(size_t directive_id) {
  if (directives.count(directive_id) == 0) {
    throw std::runtime_error("There is no such directive.");
  }

  if (directives.size() == 1)
  {
    ClearRIPL();
  } else {
    // Check for ASSUME.
    DeleteBranch(directives[directive_id].directive_node);
    directives.erase(directive_id);
  }
}

size_t ExecuteDirective(string& directive_as_string,
                        shared_ptr<NodeEvaluation> directive_node) {
  shared_ptr<NodeEvaluation> last_directive_node;
  if (directives.size() > 0) {
    last_directive_node = GetLastDirectiveNode();
  }
  Evaluator(directive_node, global_environment, shared_ptr<Node>(), shared_ptr<NodeEvaluation>());
  directive_node->earlier_evaluation_nodes = last_directive_node;

  last_directive_id++;
  directives.insert(pair<size_t, directive_entry>(last_directive_id, directive_entry(directive_as_string, directive_node)));
  return last_directive_id;
}

void BindStandardElementsToGlobalEnvironment() {
  BindToEnvironment(global_environment,
                    shared_ptr<VentureSymbol>(new VentureSymbol("CRP/make")), // Make just via the std::string?
                    shared_ptr<VentureXRP>(new VentureXRP(shared_ptr<XRP>(new XRP__CRPmaker()))));
  BindToEnvironment(global_environment,
                    shared_ptr<VentureSymbol>(new VentureSymbol("mem")), // Make just via the std::string?
                    shared_ptr<VentureXRP>(new VentureXRP(shared_ptr<XRP>(new XRP__memoizer()))));
  BindToEnvironment(global_environment,
                    shared_ptr<VentureSymbol>(new VentureSymbol("symmetric-dirichlet-multinomial/make")), // Make just via the std::string?
                    shared_ptr<VentureXRP>(new VentureXRP(shared_ptr<XRP>(new XRP__SymmetricDirichletMultinomial_maker()))));
  BindToEnvironment(global_environment,
                    shared_ptr<VentureSymbol>(new VentureSymbol("dirichlet-multinomial/make")), // Make just via the std::string?
                    shared_ptr<VentureXRP>(new VentureXRP(shared_ptr<XRP>(new XRP__DirichletMultinomial_maker()))));
  BindToEnvironment(global_environment,
                    shared_ptr<VentureSymbol>(new VentureSymbol("beta-binomial/make")), // Make just via the std::string?
                                                                                        // FIXME: add check that there are 2 arguments!
                    shared_ptr<VentureXRP>(new VentureXRP(shared_ptr<XRP>(new XRP__DirichletMultinomial_maker()))));


  BindToEnvironment(global_environment,
                    shared_ptr<VentureSymbol>(new VentureSymbol("flip")), // Make just via the std::string?
                    shared_ptr<VentureXRP>(new VentureXRP(shared_ptr<XRP>(new ERP__Flip()))));
  BindToEnvironment(global_environment,
                    shared_ptr<VentureSymbol>(new VentureSymbol("bernoulli")), // Make just via the std::string?
                    shared_ptr<VentureXRP>(new VentureXRP(shared_ptr<XRP>(new ERP__Flip()))));
  // WARNING: Deprecated:
  BindToEnvironment(global_environment,
                    shared_ptr<VentureSymbol>(new VentureSymbol("noise-negate")), // Make just via the std::string?
                    shared_ptr<VentureXRP>(new VentureXRP(shared_ptr<XRP>(new ERP__NoisyNegate()))));
  BindToEnvironment(global_environment,
                    shared_ptr<VentureSymbol>(new VentureSymbol("noisy-negate")), // Make just via the std::string?
                    shared_ptr<VentureXRP>(new VentureXRP(shared_ptr<XRP>(new ERP__NoisyNegate()))));
  BindToEnvironment(global_environment,
                    shared_ptr<VentureSymbol>(new VentureSymbol("condition-ERP")), // Make just via the std::string?
                    shared_ptr<VentureXRP>(new VentureXRP(shared_ptr<XRP>(new ERP__ConditionERP()))));
  BindToEnvironment(global_environment,
                    shared_ptr<VentureSymbol>(new VentureSymbol("normal")), // Make just via the std::string?
                    shared_ptr<VentureXRP>(new VentureXRP(shared_ptr<XRP>(new ERP__Normal()))));
  BindToEnvironment(global_environment,
                    shared_ptr<VentureSymbol>(new VentureSymbol("beta")), // Make just via the std::string?
                    shared_ptr<VentureXRP>(new VentureXRP(shared_ptr<XRP>(new ERP__Beta()))));
  BindToEnvironment(global_environment,
                    shared_ptr<VentureSymbol>(new VentureSymbol("uniform-discrete")), // Make just via the std::string?
                    shared_ptr<VentureXRP>(new VentureXRP(shared_ptr<XRP>(new ERP__UniformDiscrete()))));
  BindToEnvironment(global_environment,
                    shared_ptr<VentureSymbol>(new VentureSymbol("uniform-continuous")), // Make just via the std::string?
                    shared_ptr<VentureXRP>(new VentureXRP(shared_ptr<XRP>(new ERP__UniformContinuous()))));
  
  BindToEnvironment(global_environment,
                    shared_ptr<VentureSymbol>(new VentureSymbol("list")), // Make just via the std::string?
                    shared_ptr<VentureXRP>(new VentureXRP(shared_ptr<XRP>(new Primitive__List()))));
  BindToEnvironment(global_environment,
                    shared_ptr<VentureSymbol>(new VentureSymbol("+")), // Make just via the std::string?
                    shared_ptr<VentureXRP>(new VentureXRP(shared_ptr<XRP>(new Primitive__RealPlus()))));
  BindToEnvironment(global_environment,
                    shared_ptr<VentureSymbol>(new VentureSymbol("*")), // Make just via the std::string?
                    shared_ptr<VentureXRP>(new VentureXRP(shared_ptr<XRP>(new Primitive__RealMultiply()))));
  BindToEnvironment(global_environment,
                    shared_ptr<VentureSymbol>(new VentureSymbol("-")), // Make just via the std::string?
                    shared_ptr<VentureXRP>(new VentureXRP(shared_ptr<XRP>(new Primitive__RealMinus()))));
  BindToEnvironment(global_environment,
                    shared_ptr<VentureSymbol>(new VentureSymbol("/")), // Make just via the std::string?
                    shared_ptr<VentureXRP>(new VentureXRP(shared_ptr<XRP>(new Primitive__RealDivide()))));
  BindToEnvironment(global_environment,
                    shared_ptr<VentureSymbol>(new VentureSymbol("cos")), // Make just via the std::string?
                    shared_ptr<VentureXRP>(new VentureXRP(shared_ptr<XRP>(new Primitive__RealCos()))));
  BindToEnvironment(global_environment,
                    shared_ptr<VentureSymbol>(new VentureSymbol("sin")), // Make just via the std::string?
                    shared_ptr<VentureXRP>(new VentureXRP(shared_ptr<XRP>(new Primitive__RealSin()))));
  BindToEnvironment(global_environment,
                    shared_ptr<VentureSymbol>(new VentureSymbol("power")), // Make just via the std::string?
                    shared_ptr<VentureXRP>(new VentureXRP(shared_ptr<XRP>(new Primitive__RealPower()))));
  BindToEnvironment(global_environment,
                    shared_ptr<VentureSymbol>(new VentureSymbol(">=")), // Make just via the std::string?
                    shared_ptr<VentureXRP>(new VentureXRP(shared_ptr<XRP>(new Primitive__RealEqualOrGreater()))));
  BindToEnvironment(global_environment,
                    shared_ptr<VentureSymbol>(new VentureSymbol("<=")), // Make just via the std::string?
                    shared_ptr<VentureXRP>(new VentureXRP(shared_ptr<XRP>(new Primitive__RealEqualOrLesser()))));
  BindToEnvironment(global_environment,
                    shared_ptr<VentureSymbol>(new VentureSymbol(">")), // Make just via the std::string?
                    shared_ptr<VentureXRP>(new VentureXRP(shared_ptr<XRP>(new Primitive__RealEqualOrGreater()))));
  BindToEnvironment(global_environment,
                    shared_ptr<VentureSymbol>(new VentureSymbol("<")), // Make just via the std::string?
                    shared_ptr<VentureXRP>(new VentureXRP(shared_ptr<XRP>(new Primitive__RealLesser()))));
  BindToEnvironment(global_environment,
                    shared_ptr<VentureSymbol>(new VentureSymbol("=")), // Make just via the std::string?
                    shared_ptr<VentureXRP>(new VentureXRP(shared_ptr<XRP>(new Primitive__RealEqual()))));
}
