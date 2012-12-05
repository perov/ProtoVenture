
#ifndef VENTURE___XRP_H
#define VENTURE___XRP_H

#include "Header.h"
#include "VentureValues.h"
#include "VentureParser.h"

struct NodeXRPApplication;
extern set< shared_ptr<NodeXRPApplication> > random_choices;

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
  ReevaluationEntry(shared_ptr<NodeEvaluation>,
                    shared_ptr<NodeEvaluation>,
                    shared_ptr<VentureValue>);
  shared_ptr<NodeEvaluation> reevaluation_node;
  shared_ptr<NodeEvaluation> caller;
  shared_ptr<VentureValue> passing_value;
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

class XRP {
  virtual shared_ptr<VentureValue> XRP::Sampler(vector< shared_ptr<VentureValue> >&);
  virtual real XRP::GetSampledLoglikelihood(vector< shared_ptr<VentureValue> >&,
                                       shared_ptr<VentureValue>);
  virtual void XRP::Incorporate(vector< shared_ptr<VentureValue> >&,
                                shared_ptr<VentureValue>);
  virtual void XRP::Remove(vector< shared_ptr<VentureValue> >&,
                           shared_ptr<VentureValue>);

public:
  //XRP(shared_ptr<XRP> maker) : maker(maker) {}
  XRP::XRP();
  shared_ptr<VentureValue> XRP::Sample(vector< shared_ptr<VentureValue> >&,
                                       shared_ptr<NodeXRPApplication>);
  shared_ptr<RescorerResamplerResult>
  XRP::RescorerResampler(vector< shared_ptr<VentureValue> >&,
                         vector< shared_ptr<VentureValue> >&,
                         shared_ptr<NodeXRPApplication>,
                         bool);
  virtual bool XRP::IsRandomChoice();
  virtual bool XRP::CouldBeRescored();
  virtual string XRP::GetName();

private:
  shared_ptr<XRP> maker;
};

int UniformDiscrete(int, int);

bool VerifyOrderPattern(vector<size_t>&,
                        vector<size_t>&);

struct Node;

void DeleteNode(shared_ptr<Node>);

void DeleteBranch(shared_ptr<Node>);

void MakeMHProposal();

enum MHDecision { MH_DECLINED, MH_APPROVED };

#endif
