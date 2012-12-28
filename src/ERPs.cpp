
#include "ERPs.h"

void ERP::Incorporate(vector< shared_ptr<VentureValue> >& arguments,
                  shared_ptr<VentureValue> sampled_value) { // inline?

}
void ERP::Remove(vector< shared_ptr<VentureValue> >& arguments,
            shared_ptr<VentureValue> sampled_value) { // inline?

}

bool ERP::IsRandomChoice() { return true; }
bool ERP::CouldBeRescored() { return true; }
string ERP::GetName() { return "ERPClass"; }
  
real ERP__Flip::GetSampledLoglikelihood(vector< shared_ptr<VentureValue> >& arguments,
                                shared_ptr<VentureValue> sampled_value) { // inline?
  real weight;
  if (arguments.size() == 0) {
    weight = 0.5;
  } else if (arguments.size() == 1) {
    VentureProbability::CheckMyData(arguments[0].get());
    weight = arguments[0]->GetReal();
  } else {
    throw std::runtime_error("Wrong number of arguments.");
  }
  if (VerifyVentureType<VentureBoolean>(sampled_value) == true) {
    if (CompareValue(sampled_value, shared_ptr<VentureValue>(new VentureBoolean(true)))) {
      return log(weight);
    } else {
      return log(1.0 - weight);
    }
  } else {
    cout << " " << sampled_value << endl;
    throw std::runtime_error("Wrong value.");
  }
}
shared_ptr<VentureValue> ERP__Flip::Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller) {
  real weight;
  if (arguments.size() == 0) {
    weight = 0.5;
  } else if (arguments.size() == 1) {
    VentureProbability::CheckMyData(arguments[0].get());
    weight = arguments[0]->GetReal();
  } else {
    throw std::runtime_error("Wrong number of arguments.");
  }
  unsigned int result_int = gsl_ran_bernoulli(random_generator, weight);
  bool result_bool = (result_int == 1);
  return shared_ptr<VentureBoolean>(new VentureBoolean(result_bool));
}
string ERP__Flip::GetName() { return "ERP__Flip"; }

real ERP__Normal::GetSampledLoglikelihood(vector< shared_ptr<VentureValue> >& arguments,
                                shared_ptr<VentureValue> sampled_value) {
  real average;
  real sigma;
  if (arguments.size() == 2) {
    VentureReal::CheckMyData(arguments[0].get());
    average = arguments[0]->GetReal();
    VentureSmoothedCount::CheckMyData(arguments[1].get());
    sigma = arguments[1]->GetReal();
    double likelihood =
           gsl_ran_gaussian_pdf(ToVentureType<VentureReal>(sampled_value)->GetReal() - // Should be ToVentureType<VentureReal>(sampled_value) in a good way.
                                  average,
                                sigma);
    return log(likelihood);
  } else {
    throw std::runtime_error("Wrong number of arguments.");
  }
}
shared_ptr<VentureValue> ERP__Normal::Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller) {
  real average;
  real sigma;
  if (arguments.size() == 2) {
    VentureReal::CheckMyData(arguments[0].get());
    average = arguments[0]->GetReal();
    VentureSmoothedCount::CheckMyData(arguments[1].get());
    sigma = arguments[1]->GetReal();
    double random_value =
           gsl_ran_gaussian(random_generator,
                            sigma) +
             average;
    return shared_ptr<VentureReal>(new VentureReal(random_value));
  } else {
    throw std::runtime_error("Wrong number of arguments.");
  }
}
string ERP__Normal::GetName() { return "ERP__Normal"; }

real ERP__Beta::GetSampledLoglikelihood(vector< shared_ptr<VentureValue> >& arguments,
                                shared_ptr<VentureValue> sampled_value) {
  real alpha;
  real beta;
  if (arguments.size() == 2) {
    VentureSmoothedCount::CheckMyData(arguments[0].get());
    alpha = arguments[0]->GetReal();
    VentureSmoothedCount::CheckMyData(arguments[1].get());
    beta = arguments[1]->GetReal();
    double likelihood =
           gsl_ran_beta_pdf(ToVentureType<VentureReal>(sampled_value)->GetReal(),
                            alpha,
                            beta);
    return log(likelihood);
  } else {
    throw std::runtime_error("Wrong number of arguments.");
  }
}
shared_ptr<VentureValue> ERP__Beta::Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller) {
  real alpha;
  real beta;
  if (arguments.size() == 2) {
    VentureSmoothedCount::CheckMyData(arguments[0].get());
    alpha = arguments[0]->GetReal();
    VentureSmoothedCount::CheckMyData(arguments[1].get());
    beta = arguments[1]->GetReal();
    double random_value =
           gsl_ran_beta(random_generator,
                        alpha,
                        beta);
    return shared_ptr<VentureReal>(new VentureReal(random_value));
  } else {
    throw std::runtime_error("Wrong number of arguments.");
  }
}
string ERP__Beta::GetName() { return "ERP__Beta"; }

real ERP__UniformDiscrete::GetSampledLoglikelihood(vector< shared_ptr<VentureValue> >& arguments,
                                 shared_ptr<VentureValue> sampled_value) {
  int left_bound;
  int right_bound;
  if (arguments.size() == 2) {
    VentureCount::CheckMyData(arguments[0].get()); // Should be on! Just for the curve fitting!
    left_bound = arguments[0]->GetInteger(); // Should be GetInteger! Just for the curve fitting!
    VentureCount::CheckMyData(arguments[1].get()); // Should be on! Just for the curve fitting!
    right_bound = arguments[1]->GetInteger(); // Should be GetInteger! Just for the curve fitting!
    if (ToVentureType<VentureCount>(sampled_value)->GetInteger() >= left_bound &&
      ToVentureType<VentureCount>(sampled_value)->GetInteger() <= right_bound) {
      return log(1.0 / (1 + right_bound -
              left_bound));
    } else {
      return log(0.0);
    }
  } else {
    throw std::runtime_error("Wrong number of arguments.");
  }
}
shared_ptr<VentureValue> ERP__UniformDiscrete::Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller) {
  int left_bound;
  int right_bound;
  if (arguments.size() == 2) {
    VentureCount::CheckMyData(arguments[0].get()); // Should be on! Just for the curve fitting!
    left_bound = arguments[0]->GetInteger(); // Should be GetInteger! Just for the curve fitting!
    VentureCount::CheckMyData(arguments[1].get()); // Should be on! Just for the curve fitting!
    right_bound = arguments[1]->GetInteger(); // Should be GetInteger! Just for the curve fitting!
    int random_value =
           UniformDiscrete(left_bound,
                           right_bound);
    return shared_ptr<VentureCount>(new VentureCount(random_value));
  } else {
    throw std::runtime_error("Wrong number of arguments.");
  }
}
string ERP__UniformDiscrete::GetName() { return "ERP__UniformDiscrete"; }

real ERP__UniformContinuous::GetSampledLoglikelihood(vector< shared_ptr<VentureValue> >& arguments,
                                 shared_ptr<VentureValue> sampled_value) {
  real left_bound;
  real right_bound;
  if (arguments.size() == 2) {
    VentureReal::CheckMyData(arguments[0].get());
    left_bound = arguments[0]->GetReal();
    VentureReal::CheckMyData(arguments[1].get());
    right_bound = arguments[1]->GetReal();
    if (ToVentureType<VentureReal>(sampled_value)->GetReal() >= left_bound &&
      ToVentureType<VentureReal>(sampled_value)->GetReal() <= right_bound) {
      return log(1.0 / (right_bound -
              left_bound));
    } else {
      return log(0.0);
    }
  } else {
    throw std::runtime_error("Wrong number of arguments.");
  }
}
shared_ptr<VentureValue> ERP__UniformContinuous::Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller) {
  real left_bound;
  real right_bound;
  if (arguments.size() == 2) {
    VentureReal::CheckMyData(arguments[0].get());
    left_bound = arguments[0]->GetReal();
    VentureReal::CheckMyData(arguments[1].get());
    right_bound = arguments[1]->GetReal();
    double random_value =
          gsl_ran_flat(random_generator,
                         left_bound,
                         right_bound);
    return shared_ptr<VentureReal>(new VentureReal(random_value));
  } else {
    throw std::runtime_error("Wrong number of arguments.");
  }
}
string ERP__UniformContinuous::GetName() { return "ERP__UniformContinuous"; }

real ERP__NoisyNegate::GetSampledLoglikelihood(vector< shared_ptr<VentureValue> >& arguments,
                                shared_ptr<VentureValue> sampled_value) { // inline?
  bool boolean_expression;
  real weight;
  if (arguments.size() == 2) {
    boolean_expression = ToVentureType<VentureBoolean>(arguments[0])->data;
    VentureProbability::CheckMyData(arguments[1].get());
    weight = arguments[1]->GetReal();
  } else {
    throw std::runtime_error("Wrong number of arguments.");
  }
  if (VerifyVentureType<VentureBoolean>(arguments[0]) == true) {
    if (CompareValue(sampled_value, shared_ptr<VentureValue>(new VentureBoolean(boolean_expression)))) {
      return log(1.0 - weight);
    } else {
      return log(weight);
    }
  } else {
    cout << " " << sampled_value << endl;
    throw std::runtime_error("Wrong value.");
  }
}
shared_ptr<VentureValue> ERP__NoisyNegate::Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller) {
  bool boolean_expression;
  real weight;
  if (arguments.size() == 2) {
    boolean_expression = ToVentureType<VentureBoolean>(arguments[0])->data;
    VentureProbability::CheckMyData(arguments[1].get());
    weight = arguments[1]->GetReal();
  } else {
    throw std::runtime_error("Wrong number of arguments.");
  }
  unsigned int result_int = gsl_ran_bernoulli(random_generator, weight);
  if (result_int == 1) {
    return shared_ptr<VentureBoolean>(new VentureBoolean(!boolean_expression));
  } else {
    return shared_ptr<VentureBoolean>(new VentureBoolean(boolean_expression));
  }
}
string ERP__NoisyNegate::GetName() { return "ERP__NoisyNegate"; }

real ERP__ConditionERP::GetSampledLoglikelihood(vector< shared_ptr<VentureValue> >& arguments,
                                                shared_ptr<VentureValue> sampled_value) { // inline?
  if (arguments.size() != 3) {
    throw std::runtime_error("Wrong number of arguments.");
  }
  if (StandardPredicate(arguments[0]) &&
      arguments[1].get() == sampled_value.get()) { // Comparing by reference.
    return log(1.0);
  } else if (!StandardPredicate(arguments[0])
              && arguments[2].get() == sampled_value.get()) { // Comparing by reference.
    return log(1.0);
  } else {
    return log(0.0);
  }
}
shared_ptr<VentureValue> ERP__ConditionERP::Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller) {
  if (arguments.size() != 3) {
    throw std::runtime_error("Wrong number of arguments.");
  }
  if (StandardPredicate(arguments[0])) {
    return arguments[1];
  } else {
    return arguments[2];
  }
}
  
bool ERP__ConditionERP::IsRandomChoice() { return false; }
bool ERP__ConditionERP::CouldBeRescored() { return false; }
string ERP__ConditionERP::GetName() { return "ERP__ConditionERP"; }
