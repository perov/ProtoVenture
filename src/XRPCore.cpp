
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
  : loglikelihood_changes(0.0),
    random_choices_delta(0)
{}

shared_ptr<VentureValue>
XRP::Sample(vector< shared_ptr<VentureValue> >& arguments,
            shared_ptr<NodeXRPApplication> caller) {
  shared_ptr<VentureValue> new_sample = Sampler(arguments, caller);
  real loglikelihood = GetSampledLoglikelihood(arguments, new_sample);
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
  }
  
  if (this->GetName() == "XRP__memoized_procedure") {
    FreezeBranch(caller, reevaluation_parameters);
  }
  if (forced_resampling || !CouldBeRescored()) {
    Remove(old_arguments, caller->sampled_value);
    
    shared_ptr<VentureValue> new_sample = Sampler(new_arguments, caller);
    Incorporate(new_arguments, new_sample);

    return shared_ptr<RescorerResamplerResult>(new RescorerResamplerResult(new_sample, 0, 0));
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
shared_ptr<VentureValue> XRP::Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller) {
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
