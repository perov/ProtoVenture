
#ifndef VENTURE___XRP_CORE_H
#define VENTURE___XRP_CORE_H

#include "Header.h"
#include "VentureValues.h"
#include "VentureParser.h"

struct NodeXRPApplication;
extern set< shared_ptr<NodeXRPApplication> > random_choices;
extern int next_gensym_atom;

struct RescorerResamplerResult {
  RescorerResamplerResult(shared_ptr<VentureValue>,
                          real,
                          real);
  shared_ptr<VentureValue> new_value;
  real old_loglikelihood;
  real new_loglikelihood;
};

struct ReevaluationResult {
  ReevaluationResult(shared_ptr<VentureValue>,
                     bool);
  shared_ptr<VentureValue> passing_value;
  bool pass_further;
};

struct ReevaluationEntry {
  ReevaluationEntry(shared_ptr<NodeEvaluation> reevaluation_node,
                    shared_ptr<NodeEvaluation> caller,
                    shared_ptr<VentureValue> passing_value,
                    size_t priority);
  shared_ptr<NodeEvaluation> reevaluation_node;
  shared_ptr<NodeEvaluation> caller;
  shared_ptr<VentureValue> passing_value;
  size_t priority;
};

struct OmitPattern {
  OmitPattern(vector<size_t>&,
              vector<size_t>&);
  vector<size_t> omit_pattern;
  vector<size_t> stop_pattern; // stop_pattern is omit_pattern.pop().
                               // Use it in the future to increase efficiency,
                               // and make the code more accurate.
};

struct ReevaluationParameters {
  ReevaluationParameters();
  double loglikelihood_changes;
  int random_choices_delta;
};

class XRP : public boost::enable_shared_from_this<XRP> {
public: // Should be private.
  virtual shared_ptr<VentureValue> Sampler(vector< shared_ptr<VentureValue> >& arguments, shared_ptr<NodeXRPApplication> caller);
  virtual real GetSampledLoglikelihood(vector< shared_ptr<VentureValue> >&,
                                       shared_ptr<VentureValue>);
  virtual void Incorporate(vector< shared_ptr<VentureValue> >&,
                                shared_ptr<VentureValue>);
  virtual void Remove(vector< shared_ptr<VentureValue> >&,
                           shared_ptr<VentureValue>);

public:
  //XRP(shared_ptr<XRP> maker) : maker(maker) {}
  XRP();
  shared_ptr<VentureValue> Sample(vector< shared_ptr<VentureValue> >&, // FIXME: why not virtual?
                                       shared_ptr<NodeXRPApplication>);
  virtual void Unsampler(vector< shared_ptr<VentureValue> >& old_arguments, shared_ptr<NodeXRPApplication> caller); // Unsampler or sampler?
  shared_ptr<RescorerResamplerResult>
  RescorerResampler(vector< shared_ptr<VentureValue> >& old_arguments,
                         vector< shared_ptr<VentureValue> >& new_arguments,
                         shared_ptr<NodeXRPApplication> caller,
                         bool forced_resampling,
                         ReevaluationParameters& reevaluation_parameters);
  virtual bool IsRandomChoice();
  virtual bool CouldBeRescored();
  virtual string GetName();

public: // Should be private?
};

#endif
