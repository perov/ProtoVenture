
#include "HeaderPre.h"
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
    //cout << " " << sampled_value << endl;
    throw std::runtime_error("Wrong value.");
  }
}
shared_ptr<VentureValue> ERP__Flip::Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller, EvaluationConfig& evaluation_config) {
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
    //double likelihood =
    //       gsl_ran_gaussian_pdf(ToVentureType<VentureReal>(sampled_value)->GetReal() - // Should be ToVentureType<VentureReal>(sampled_value) in a good way.
    //                              average,
    //                            sigma);
    //return log(likelihood);
    real sampled_value_real = ToVentureType<VentureReal>(sampled_value)->GetReal();
    return NormalDistributionLogLikelihood(sampled_value_real, average, sigma);
  } else {
    throw std::runtime_error("Wrong number of arguments.");
  }
}
shared_ptr<VentureValue> ERP__Normal::Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller, EvaluationConfig& evaluation_config) {
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
    //double likelihood =
    //       gsl_ran_beta_pdf(ToVentureType<VentureReal>(sampled_value)->GetReal(),
    //                        alpha,
    //                        beta);
    //return log(likelihood);
    real sampled_value_real = ToVentureType<VentureReal>(sampled_value)->GetReal();
    return BetaDistributionLogLikelihood(sampled_value_real, alpha, beta);
  } else {
    throw std::runtime_error("Wrong number of arguments.");
  }
}
shared_ptr<VentureValue> ERP__Beta::Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller, EvaluationConfig& evaluation_config) {
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

real ERP__Poisson::GetSampledLoglikelihood(vector< shared_ptr<VentureValue> >& arguments,
                                shared_ptr<VentureValue> sampled_value) {
  real lambda;
  if (arguments.size() == 1) {
    VentureSmoothedCount::CheckMyData(arguments[0].get());
    lambda = arguments[0]->GetReal();
    //double likelihood =
    //       gsl_ran_poisson_pdf(ToVentureType<VentureCount>(sampled_value)->GetInteger(),
    //                         lambda);
    //return log(likelihood);
    int sampled_value_count = ToVentureType<VentureCount>(sampled_value)->GetInteger();
    return PoissonDistributionLogLikelihood(sampled_value_count, lambda);
  } else {
    throw std::runtime_error("Wrong number of arguments.");
  }
}
shared_ptr<VentureValue> ERP__Poisson::Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller, EvaluationConfig& evaluation_config) {
  real lambda;
  if (arguments.size() == 1) {
    VentureSmoothedCount::CheckMyData(arguments[0].get());
    lambda = arguments[0]->GetReal();
    int random_value =
           gsl_ran_poisson(random_generator,
                         lambda);
    return shared_ptr<VentureCount>(new VentureCount(random_value));
  } else {
    throw std::runtime_error("Wrong number of arguments.");
  }
}
string ERP__Poisson::GetName() { return "ERP__Poisson"; }

real ERP__Gamma::GetSampledLoglikelihood(vector< shared_ptr<VentureValue> >& arguments,
                                shared_ptr<VentureValue> sampled_value) {
  real alpha;
  real beta;
  if (arguments.size() == 2) {
    VentureSmoothedCount::CheckMyData(arguments[0].get());
    alpha = arguments[0]->GetReal();
    VentureSmoothedCount::CheckMyData(arguments[1].get());
    beta = arguments[1]->GetReal();
    //double likelihood =
    //       gsl_ran_gamma_pdf(ToVentureType<VentureReal>(sampled_value)->GetReal(),
    //                         alpha,
    //                         1.0 / beta);
    //return log(likelihood);
    real sampled_value_real = ToVentureType<VentureReal>(sampled_value)->GetReal();
    return GammaDistributionLogLikelihood(sampled_value_real, alpha, beta);
  } else {
    throw std::runtime_error("Wrong number of arguments.");
  }
}
shared_ptr<VentureValue> ERP__Gamma::Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller, EvaluationConfig& evaluation_config) {
  real alpha;
  real beta;
  if (arguments.size() == 2) {
    VentureSmoothedCount::CheckMyData(arguments[0].get());
    alpha = arguments[0]->GetReal();
    VentureSmoothedCount::CheckMyData(arguments[1].get());
    beta = arguments[1]->GetReal();
    double random_value =
           gsl_ran_gamma(random_generator,
                         alpha,
                         1.0 / beta);
    return shared_ptr<VentureReal>(new VentureReal(random_value));
  } else {
    throw std::runtime_error("Wrong number of arguments.");
  }
}
string ERP__Gamma::GetName() { return "ERP__Gamma"; }

real ERP__InverseGamma::GetSampledLoglikelihood(vector< shared_ptr<VentureValue> >& arguments,
                                shared_ptr<VentureValue> sampled_value) {
  real alpha;
  real beta;
  if (arguments.size() == 2) {
    VentureSmoothedCount::CheckMyData(arguments[0].get());
    alpha = arguments[0]->GetReal();
    VentureSmoothedCount::CheckMyData(arguments[1].get());
    beta = arguments[1]->GetReal();
    //double likelihood =
    //       gsl_ran_gamma_pdf(1.0 / ToVentureType<VentureReal>(sampled_value)->GetReal(),
    //                         alpha,
    //                         1.0 / beta);
    //return log(likelihood);
    real sampled_value_real = ToVentureType<VentureReal>(sampled_value)->GetReal();
    return InverseGammaDistributionLogLikelihood(sampled_value_real, alpha, beta);
  } else {
    throw std::runtime_error("Wrong number of arguments.");
  }
}
shared_ptr<VentureValue> ERP__InverseGamma::Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller, EvaluationConfig& evaluation_config) {
  real alpha;
  real beta;
  if (arguments.size() == 2) {
    VentureSmoothedCount::CheckMyData(arguments[0].get());
    alpha = arguments[0]->GetReal();
    VentureSmoothedCount::CheckMyData(arguments[1].get());
    beta = arguments[1]->GetReal();
    double random_value =
           gsl_ran_gamma(random_generator,
                         alpha,
                         1.0 / beta);
    return shared_ptr<VentureReal>(new VentureReal(1.0 / random_value));
  } else {
    throw std::runtime_error("Wrong number of arguments.");
  }
}
string ERP__InverseGamma::GetName() { return "ERP__InverseGamma"; }

real ERP__ChiSquared::GetSampledLoglikelihood(vector< shared_ptr<VentureValue> >& arguments,
                                shared_ptr<VentureValue> sampled_value) {
  real nu;
  if (arguments.size() == 1) {
    VentureSmoothedCount::CheckMyData(arguments[0].get());
    nu = arguments[0]->GetReal();
    //double likelihood =
    //       gsl_ran_chisq_pdf(ToVentureType<VentureReal>(sampled_value)->GetReal(),
    //                         nu);
    //return log(likelihood);
    real sampled_value_real = ToVentureType<VentureReal>(sampled_value)->GetReal();
    return ChiSquaredDistributionLogLikelihood(sampled_value_real, nu);
  } else {
    throw std::runtime_error("Wrong number of arguments.");
  }
}
shared_ptr<VentureValue> ERP__ChiSquared::Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller, EvaluationConfig& evaluation_config) {
  real nu;
  if (arguments.size() == 1) {
    VentureSmoothedCount::CheckMyData(arguments[0].get());
    nu = arguments[0]->GetReal();
    double random_value =
           gsl_ran_chisq(random_generator,
                         nu);
    return shared_ptr<VentureReal>(new VentureReal(random_value));
  } else {
    throw std::runtime_error("Wrong number of arguments.");
  }
}
string ERP__ChiSquared::GetName() { return "ERP__ChiSquared"; }

real ERP__InverseChiSquared::GetSampledLoglikelihood(vector< shared_ptr<VentureValue> >& arguments,
                                shared_ptr<VentureValue> sampled_value) {
  real nu;
  if (arguments.size() == 1) {
    VentureSmoothedCount::CheckMyData(arguments[0].get());
    nu = arguments[0]->GetReal();
    //double likelihood =
    //       gsl_ran_chisq_pdf(1.0 / ToVentureType<VentureReal>(sampled_value)->GetReal(),
    //                         nu);
    //return log(likelihood);
    real sampled_value_real = ToVentureType<VentureReal>(sampled_value)->GetReal();
    return InverseChiSquaredDistributionLogLikelihood(sampled_value_real, nu);
  } else {
    throw std::runtime_error("Wrong number of arguments.");
  }
}
shared_ptr<VentureValue> ERP__InverseChiSquared::Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller, EvaluationConfig& evaluation_config) {
  real nu;
  if (arguments.size() == 1) {
    VentureSmoothedCount::CheckMyData(arguments[0].get());
    nu = arguments[0]->GetReal();
    double random_value =
           gsl_ran_chisq(random_generator,
                         nu);
    return shared_ptr<VentureReal>(new VentureReal(1.0 / random_value));
  } else {
    throw std::runtime_error("Wrong number of arguments.");
  }
}
string ERP__InverseChiSquared::GetName() { return "ERP__InverseChiSquared"; }

real ERP__SymmetricDirichlet::GetSampledLoglikelihood(vector< shared_ptr<VentureValue> >& arguments,
                                shared_ptr<VentureValue> sampled_value) {
  if (arguments.size() == 2) {
    double* arguments_for_gsl = new double[arguments[1]->GetInteger()];
    double* returned_values = new double[arguments[1]->GetInteger()];
    double sum = 0.0;
    assert(arguments[1]->GetInteger() == ToVentureType<VentureSimplexPoint>(sampled_value)->data.size());
    VentureSmoothedCount::CheckMyData(arguments[0].get());
    for (size_t index = 0; index < arguments[1]->GetInteger(); index++) {
      arguments_for_gsl[index] = arguments[0]->GetReal();
      if (index + 1 != arguments[1]->GetInteger()) {
        returned_values[index] = ToVentureType<VentureSimplexPoint>(sampled_value)->data[index];
        sum += returned_values[index];
      } else {
        returned_values[index] = 1.0 - sum;
      }
    }

    real likelihood = gsl_ran_dirichlet_pdf(arguments[1]->GetInteger(), arguments_for_gsl, returned_values);
    
    delete [] arguments_for_gsl;
    delete [] returned_values;

    return log(likelihood);
  } else {
    throw std::runtime_error("Wrong number of arguments.");
  }
}
shared_ptr<VentureValue> ERP__SymmetricDirichlet::Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller, EvaluationConfig& evaluation_config) {
  if (arguments.size() == 2) {
    double* arguments_for_gsl = new double[arguments[1]->GetInteger()];
    double* returned_values = new double[arguments[1]->GetInteger()];
    VentureSmoothedCount::CheckMyData(arguments[0].get());
    for (size_t index = 0; index < arguments[1]->GetInteger(); index++) {
      arguments_for_gsl[index] = arguments[0]->GetReal();
    }

    gsl_ran_dirichlet(random_generator, arguments[1]->GetInteger(), arguments_for_gsl, returned_values);
    
    vector<real> returned_value_as_vector(arguments[1]->GetInteger()); // Not very efficient.
    for (size_t index = 0; index < arguments[1]->GetInteger(); index++) {
      returned_value_as_vector[index] = returned_values[index];
    }
    
    delete [] arguments_for_gsl;
    delete [] returned_values;

    return shared_ptr<VentureSimplexPoint>(new VentureSimplexPoint(returned_value_as_vector));
  } else {
    throw std::runtime_error("Wrong number of arguments.");
  }
}
string ERP__SymmetricDirichlet::GetName() { return "ERP__SymmetricDirichlet"; }

real ERP__Dirichlet::GetSampledLoglikelihood(vector< shared_ptr<VentureValue> >& arguments,
                                shared_ptr<VentureValue> sampled_value) {
  if (arguments.size() >= 2) {
    double* arguments_for_gsl = new double[arguments.size()];
    double* returned_values = new double[arguments.size()];
    double sum = 0.0;
    assert(arguments.size() == ToVentureType<VentureSimplexPoint>(sampled_value)->data.size());
    for (size_t index = 0; index < arguments.size(); index++) {
      VentureSmoothedCount::CheckMyData(arguments[index].get());
      arguments_for_gsl[index] = arguments[index]->GetReal();
      if (index + 1 != arguments.size()) {
        returned_values[index] = ToVentureType<VentureSimplexPoint>(sampled_value)->data[index];
        sum += returned_values[index];
      } else {
        returned_values[index] = 1.0 - sum;
      }
    }

    real likelihood = gsl_ran_dirichlet_pdf(arguments.size(), arguments_for_gsl, returned_values);
    
    delete [] arguments_for_gsl;
    delete [] returned_values;

    return log(likelihood);
  } else {
    throw std::runtime_error("Wrong number of arguments.");
  }
}
shared_ptr<VentureValue> ERP__Dirichlet::Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller, EvaluationConfig& evaluation_config) {
  if (arguments.size() >= 2) {
    double* arguments_for_gsl = new double[arguments.size()];
    double* returned_values = new double[arguments.size()];
    for (size_t index = 0; index < arguments.size(); index++) {
      VentureSmoothedCount::CheckMyData(arguments[index].get());
      arguments_for_gsl[index] = arguments[index]->GetReal();
    }

    gsl_ran_dirichlet(random_generator, arguments.size(), arguments_for_gsl, returned_values);
    
    vector<real> returned_value_as_vector(arguments.size()); // Not very efficient.
    for (size_t index = 0; index < arguments.size(); index++) {
      returned_value_as_vector[index] = returned_values[index];
    }
    
    delete [] arguments_for_gsl;
    delete [] returned_values;

    return shared_ptr<VentureSimplexPoint>(new VentureSimplexPoint(returned_value_as_vector));
  } else {
    throw std::runtime_error("Wrong number of arguments.");
  }
}
string ERP__Dirichlet::GetName() { return "ERP__Dirichlet"; }

real ERP__CategoricalSP::GetSampledLoglikelihood(vector< shared_ptr<VentureValue> >& arguments,
                                shared_ptr<VentureValue> sampled_value) { // inline?
  if (arguments.size() != 1) {
    throw std::runtime_error("Wrong number of arguments.");
  }
  shared_ptr<VentureSimplexPoint> simplex_point = ToVentureType<VentureSimplexPoint>(arguments[0]);
  if (sampled_value->GetInteger() < 0 || sampled_value->GetInteger() >= simplex_point->data.size() + 1) {
    return log(0.0);
  } else {
    if (sampled_value->GetInteger() == simplex_point->data.size()) {
      double sum = 0.0;
      for (size_t index = 0; index < simplex_point->data.size(); index++) {
        sum += simplex_point->data[index];
      }
      return log(1.0 - sum);
    } else {
      return log(simplex_point->data[sampled_value->GetInteger()]);
    }
  }
}
shared_ptr<VentureValue> ERP__CategoricalSP::Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller, EvaluationConfig& evaluation_config) {
  if (arguments.size() != 1) {
    throw std::runtime_error("Wrong number of arguments.");
  }

  double random_uniform_0_1 = gsl_ran_flat(random_generator, 0, 1);
  shared_ptr<VentureSimplexPoint> simplex_point = ToVentureType<VentureSimplexPoint>(arguments[0]);
  double accumulated_probability = 0.0;

  for (size_t index = 0; index < simplex_point->data.size(); index++) {
    accumulated_probability += simplex_point->data[index];
    if (random_uniform_0_1 <= accumulated_probability) {
      return shared_ptr<VentureAtom>(new VentureAtom(index));
    }
  }
  return shared_ptr<VentureAtom>(new VentureAtom(simplex_point->data.size()));
}
string ERP__CategoricalSP::GetName() { return "ERP__CategoricalSP"; }

real ERP__UniformDiscrete::GetSampledLoglikelihood(vector< shared_ptr<VentureValue> >& arguments,
                                 shared_ptr<VentureValue> sampled_value) {
  int left_bound;
  int right_bound;
  if (arguments.size() == 2) {
    assert(arguments[1]->GetInteger() >= arguments[0]->GetInteger());
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
shared_ptr<VentureValue> ERP__UniformDiscrete::Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller, EvaluationConfig& evaluation_config) {
  int left_bound;
  int right_bound;
  if (arguments.size() == 2) {
    assert(arguments[1]->GetInteger() >= arguments[0]->GetInteger());
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
    assert(arguments[1]->GetReal() >= arguments[0]->GetReal());
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
shared_ptr<VentureValue> ERP__UniformContinuous::Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller, EvaluationConfig& evaluation_config) {
  real left_bound;
  real right_bound;
  if (arguments.size() == 2) {
    assert(arguments[1]->GetReal() >= arguments[0]->GetReal());
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
    //cout << " " << sampled_value << endl;
    throw std::runtime_error("Wrong value.");
  }
}
shared_ptr<VentureValue> ERP__NoisyNegate::Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller, EvaluationConfig& evaluation_config) {
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

real ERP__Categorical::GetSampledLoglikelihood(vector< shared_ptr<VentureValue> >& arguments,
                                shared_ptr<VentureValue> sampled_value) { // inline?
  if (arguments.size() != 1) {
    throw std::runtime_error("Wrong number of arguments.");
  }
  shared_ptr<VentureList> list = ToVentureType<VentureList>(arguments[0]);
  if (GetSize(list) < 1) {
    throw std::runtime_error("The dimensionality should be >= 1.");
  }
  { // Reconsider.
    double accumulated_probability = 0.0;
    shared_ptr<VentureList> list_to_enumerate = list;
    while (list_to_enumerate != NIL_INSTANCE) {
      accumulated_probability += GetFirst(list_to_enumerate)->GetReal();
      list_to_enumerate = GetNext(list_to_enumerate);
    }
    if (fabs(accumulated_probability - 1.0) > comparison_epsilon) {
      throw std::runtime_error("The sum of probabilities should be equal to 1.0.");
    }
  }
  if (sampled_value->GetInteger() < 0 || sampled_value->GetInteger() >= GetSize(list)) {
    return log(0.0);
  } else {
    return log(GetNth(list, sampled_value->GetInteger() + 1)->GetReal());
  }
}
shared_ptr<VentureValue> ERP__Categorical::Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller, EvaluationConfig& evaluation_config) {
  if (arguments.size() != 1) {
    throw std::runtime_error("Wrong number of arguments.");
  }
  shared_ptr<VentureList> list = ToVentureType<VentureList>(arguments[0]);
  if (GetSize(list) < 1) {
    throw std::runtime_error("The dimensionality should be >= 1.");
  }
  {
    double accumulated_probability = 0.0;
    shared_ptr<VentureList> list_to_enumerate = list;
    while (list_to_enumerate != NIL_INSTANCE) {
      accumulated_probability += GetFirst(list_to_enumerate)->GetReal();
      list_to_enumerate = GetNext(list_to_enumerate);
    }
    if (fabs(accumulated_probability - 1.0) > comparison_epsilon) {
      throw std::runtime_error("The sum of probabilities should be equal to 1.0.");
    }
  }

  double random_uniform_0_1 = gsl_ran_flat(random_generator, 0, 1);
  double accumulated_probability = 0.0;
  size_t index = 0;
  while (list != NIL_INSTANCE) {
    accumulated_probability += GetFirst(list)->GetReal();
    if (random_uniform_0_1 <= accumulated_probability) {
      return shared_ptr<VentureAtom>(new VentureAtom(index));
    }
    list = GetNext(list);
    index++;
  }
  if (fabs(accumulated_probability - 1.0) < comparison_epsilon) {
    return shared_ptr<VentureAtom>(new VentureAtom(index - 1));
  } else {
    throw std::runtime_error("Strange error in ERP__Categorical.");
  }
}
string ERP__Categorical::GetName() { return "ERP__Categorical"; }

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
shared_ptr<VentureValue> ERP__ConditionERP::Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller, EvaluationConfig& evaluation_config) {
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
