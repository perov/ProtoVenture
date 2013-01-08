
#include "Header.h"
#include "VentureValues.h"
#include "VentureParser.h"
#include "Analyzer.h"
#include "XRPCore.h"
#include "RIPL.h"

RescorerResamplerResult::RescorerResamplerResult(shared_ptr<VentureValue> new_value,
                        real old_loglikelihood,
                        real new_loglikelihood)
  : new_value(new_value),
    old_loglikelihood(old_loglikelihood),
    new_loglikelihood(new_loglikelihood)
{}
  
ReevaluationResult::ReevaluationResult(shared_ptr<VentureValue> passing_value,
                    bool pass_further)
  : passing_value(passing_value),
    pass_further(pass_further)
{}
  
ReevaluationEntry::ReevaluationEntry(shared_ptr<NodeEvaluation> reevaluation_node,
                  shared_ptr<NodeEvaluation> caller,
                  shared_ptr<VentureValue> passing_value,
                  size_t priority)
  : reevaluation_node(reevaluation_node),
    caller(caller),
    passing_value(passing_value),
    priority(priority)
{}
  
OmitPattern::OmitPattern(vector<size_t>& omit_pattern,
            vector<size_t>& stop_pattern)
  : omit_pattern(omit_pattern),
    stop_pattern(stop_pattern)
{}
  
ReevaluationParameters::ReevaluationParameters()
  : loglikelihood_changes_from_new_to_old(0.0),
    loglikelihood_changes_from_old_to_new(0.0),
    loglikelihood_p_new(0.0),
    loglikelihood_p_old(0.0),
    random_choices_delta(0)
{}

shared_ptr<VentureValue>
XRP::Sample(vector< shared_ptr<VentureValue> >& arguments,
            shared_ptr<NodeXRPApplication> caller,
            EvaluationConfig& evaluation_config) {
  shared_ptr<VentureValue> new_sample;
  real loglikelihood;

  if (evaluation_config.should_be_forced_for_OBSERVE != shared_ptr<NodeDirectiveObserve>() &&
        this->GetName() != "XRP__memoized_procedure") {
    if (this->CouldBeRescored() == false) {
      throw std::runtime_error("You cannot force the XRP in this OBSERVE directive, because it is deterministic.");
    }
    new_sample = evaluation_config.should_be_forced_for_OBSERVE->observed_value;
    if (caller->was_forced_for != shared_ptr<NodeDirectiveObserve>()) {
      throw std::runtime_error("You cannot 'OBSERVE' the same XRP twice or more times.");
    }
    caller->was_forced_for = evaluation_config.should_be_forced_for_OBSERVE;
    evaluation_config.should_be_forced_for_OBSERVE = shared_ptr<NodeDirectiveObserve>();
    loglikelihood = GetSampledLoglikelihood(arguments, new_sample);
    evaluation_config.loglikelihood_p_constraint_score = loglikelihood; // Goes only to P(X_new)
  } else {
    new_sample = Sampler(arguments, caller, evaluation_config);
    loglikelihood = GetSampledLoglikelihood(arguments, new_sample);
    evaluation_config.loglikelihood_p_unconstraint_score = loglikelihood; // Goes both to P(X_new) and Q(X_old -> X_new)

    if (this->IsRandomChoice() == true) {
      random_choices.insert(dynamic_pointer_cast<NodeXRPApplication>(caller->shared_from_this()));
      evaluation_config.number_of_created_random_choices++;
    }
  }
  
  if (evaluation_config.we_are_in_proposal == false) {
    if (loglikelihood == log(0.0)) {
      throw std::runtime_error("You are trying to execute the code, which has the joint score = 0.0 (at least in one of its state!).");
      // FIXME: should be improved in the future.
      //        (By resampling, if it happens.)
    }
  }
  Incorporate(arguments, new_sample);
  caller->sampled_value = new_sample;
  return new_sample;
}

shared_ptr<RescorerResamplerResult>
XRP::RescorerResampler(vector< shared_ptr<VentureValue> >& old_arguments,
                       vector< shared_ptr<VentureValue> >& new_arguments,
                       shared_ptr<NodeXRPApplication> caller,
                       bool forced_resampling,
                       ReevaluationParameters& reevaluation_parameters) {
  assert(old_arguments.size() == new_arguments.size());
  // This check slows the inference, though only by constant time.
  // Warning! If somebody wants to remove this *check*,
  // it is necessary to know that without it in some situtations there would be a problem:
  // Let's consider memoized procedure MP = (mem (lambda (id) (CRP))).
  // Let's also assume that CRP works in the way that it returns the least
  // non-negative free number in the case of a new table.
  // If we then have in the code (MP (flip)), without this *check*,
  // and flip reflips from *false* to *false*,
  // memoization node will lose output reference to call to (MP (flip)).
  // FIXME: (self-notice) Yura, do not forget to remove
  //        that the (if (flip) (lambda ...) (lambda ...))
  //        has been already disabled for reevaluation
  //        in another code part!
  bool arguments_are_different = false;
  for (size_t index = 0; index < old_arguments.size(); index++) {
    if (old_arguments[index]->GetType() != new_arguments[index]->GetType() || // FIXME: Will not work now with the current NIL-LIST implementation?
          old_arguments[index]->CompareByValue(new_arguments[index]) == false) {
      arguments_are_different = true;
      break;
    }
  }
  if (arguments_are_different == false && !forced_resampling) {
    return shared_ptr<RescorerResamplerResult>(
      new RescorerResamplerResult(shared_ptr<VentureValue>(),
                                  0.0, // Is it okay that we do not send the actual loglikelihood?
                                  0.0)); // Is it okay that we do not send the actual loglikelihood?
                                  // Added Jan/5/2013: Especially when we now do not cancel by default?
  }
  
  shared_ptr<NodeDirectiveObserve> was_forced_for__if_any;
  if (this->GetName() == "XRP__memoized_procedure") {
    FreezeBranch(caller, reevaluation_parameters, was_forced_for__if_any);
  }
  EvaluationConfig local_evaluation_config(true);
  local_evaluation_config.should_be_forced_for_OBSERVE = was_forced_for__if_any;

  if (forced_resampling || !CouldBeRescored()) {
    Remove(old_arguments, caller->sampled_value);
    real old_loglikelihood = GetSampledLoglikelihood(old_arguments, caller->sampled_value);
    shared_ptr<VentureValue> new_sample = Sampler(new_arguments, caller, local_evaluation_config);
    real new_loglikelihood = GetSampledLoglikelihood(new_arguments, new_sample);
    Incorporate(new_arguments, new_sample);

    if (was_forced_for__if_any != shared_ptr<NodeDirectiveObserve>()) {
      reevaluation_parameters.random_choices_delta += local_evaluation_config.number_of_created_random_choices;
      reevaluation_parameters.loglikelihood_p_new += local_evaluation_config.loglikelihood_p_constraint_score + local_evaluation_config.loglikelihood_p_unconstraint_score;
      reevaluation_parameters.loglikelihood_changes_from_old_to_new += local_evaluation_config.loglikelihood_p_unconstraint_score;
    }

    if (local_evaluation_config.should_be_forced_for_OBSERVE != shared_ptr<NodeDirectiveObserve>()) {
      throw std::runtime_error("Evaluator has not found proper XRP sampler application to force its value (2).");
    }
    
    return shared_ptr<RescorerResamplerResult>(new RescorerResamplerResult(new_sample, old_loglikelihood, new_loglikelihood));
  } else {
    Remove(old_arguments, caller->sampled_value);
    real old_loglikelihood = GetSampledLoglikelihood(old_arguments, caller->sampled_value);
    real new_loglikelihood = GetSampledLoglikelihood(new_arguments, caller->sampled_value);
    Incorporate(new_arguments, caller->sampled_value);
    
    return shared_ptr<RescorerResamplerResult>(
      new RescorerResamplerResult(shared_ptr<VentureValue>(),
                                  old_loglikelihood,
                                  new_loglikelihood));
  }
}

XRP::XRP() {}
shared_ptr<VentureValue> XRP::Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller, EvaluationConfig& evaluation_config) {
  throw std::runtime_error("It should not happen.");
} // Should be just ";"?
void XRP::Unsampler(vector< shared_ptr<VentureValue> >& old_arguments, shared_ptr<NodeXRPApplication> caller) {
  
}
real XRP::GetSampledLoglikelihood(vector< shared_ptr<VentureValue> >& arguments,
                                      shared_ptr<VentureValue> sampled_value) {
  throw std::runtime_error("It should not happen.");
} // Should be just ";"?
void XRP::Incorporate(vector< shared_ptr<VentureValue> >& arguments,
                              shared_ptr<VentureValue> sampled_value) {
  throw std::runtime_error("It should not happen.");
} // Should be just ";"?
void XRP::Remove(vector< shared_ptr<VentureValue> >& arguments,
                          shared_ptr<VentureValue> sampled_value) {
  throw std::runtime_error("It should not happen.");
} // Should be just ";"?
  
bool XRP::IsRandomChoice() { return false; }
bool XRP::CouldBeRescored() { return false; }
string XRP::GetName() { return "XRPClass"; }
