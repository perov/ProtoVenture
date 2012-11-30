
#ifndef VENTURE___XRP_H
#define VENTURE___XRP_H

#include "Header.h"
#include "VentureValues.h"
#include "VentureParser.h"

struct NodeXRPApplication;
extern set< shared_ptr<NodeXRPApplication> > random_choices;

struct RescorerResamplerResult {
  RescorerResamplerResult(shared_ptr<VentureValue> new_value,
                          real old_loglikelihood,
                          real new_loglikelihood)
    : new_value(new_value),
      old_loglikelihood(old_loglikelihood),
      new_loglikelihood(new_loglikelihood)
  {}
  shared_ptr<VentureValue> new_value;
  real old_loglikelihood;
  real new_loglikelihood;
};

struct ReevaluationResult {
  ReevaluationResult(shared_ptr<VentureValue> passing_value,
                     bool pass_further)
    : passing_value(passing_value),
      pass_further(pass_further)
  {}
  shared_ptr<VentureValue> passing_value;
  bool pass_further;
};

struct ReevaluationEntry {
  ReevaluationEntry(shared_ptr<NodeEvaluation> reevaluation_node,
                    shared_ptr<NodeEvaluation> caller,
                    shared_ptr<VentureValue> passing_value)
    : reevaluation_node(reevaluation_node),
      caller(caller),
      passing_value(passing_value)
  {}
  shared_ptr<NodeEvaluation> reevaluation_node;
  shared_ptr<NodeEvaluation> caller;
  shared_ptr<VentureValue> passing_value;
};

struct OmitPattern {
  OmitPattern(vector<size_t>& omit_pattern,
              vector<size_t>& stop_pattern)
    : omit_pattern(omit_pattern),
      stop_pattern(stop_pattern)
  {}
  vector<size_t> omit_pattern;
  vector<size_t> stop_pattern; // stop_pattern is omit_pattern.pop().
                               // Use it in the future to increase efficiency,
                               // and make the code more accurate.
};

struct ReevaluationParameters {
  ReevaluationParameters()
    : loglikelihood_changes(0.0),
      random_choices_delta(0)
  {}
  double loglikelihood_changes;
  int random_choices_delta;
};

class XRP {
  virtual shared_ptr<VentureValue>
  Sampler(vector< shared_ptr<VentureValue> >& arguments) {
    throw std::exception("It should not happen.");
  } // Should be just ";"?
  virtual real GetSampledLoglikelihood(vector< shared_ptr<VentureValue> >& arguments,
                                       shared_ptr<VentureValue> sampled_value) {
    throw std::exception("It should not happen.");
  } // Should be just ";"?
  virtual void Incorporate(vector< shared_ptr<VentureValue> >& arguments,
                           shared_ptr<VentureValue> sampled_value) {
    throw std::exception("It should not happen.");
  } // Should be just ";"?
  virtual void Remove(vector< shared_ptr<VentureValue> >& arguments,
                      shared_ptr<VentureValue> sampled_value) {
    throw std::exception("It should not happen.");
  } // Should be just ";"?

public:
  //XRP(shared_ptr<XRP> maker) : maker(maker) {}
  XRP() {}
  shared_ptr<VentureValue> Sample(vector< shared_ptr<VentureValue> >&,
                                  shared_ptr<NodeXRPApplication>);
  shared_ptr<RescorerResamplerResult>
  RescorerResampler(vector< shared_ptr<VentureValue> >&,
                    vector< shared_ptr<VentureValue> >&,
                    shared_ptr<NodeXRPApplication>,
                    bool);
  virtual bool IsRandomChoice() { return false; }
  virtual bool CouldBeRescored() { return false; }
  virtual string GetName() { return "XRPClass"; }

private:
  shared_ptr<XRP> maker;
};

class ERP : public XRP {
  void Incorporate(vector< shared_ptr<VentureValue> >& arguments,
                   shared_ptr<VentureValue> sampled_value) { // inline?

  }
  void Remove(vector< shared_ptr<VentureValue> >& arguments,
              shared_ptr<VentureValue> sampled_value) { // inline?

  }
  
public:
  virtual bool IsRandomChoice() { return true; }
  virtual bool CouldBeRescored() { return true; }
  virtual string GetName() { return "ERPClass"; }
};

// IMPORTANT FIXME:
// weight SHOULD BE IMPLEMENTED NOT VIA REAL!
class ERP__Flip : public ERP {
  real GetSampledLoglikelihood(vector< shared_ptr<VentureValue> >& arguments,
                                 shared_ptr<VentureValue> sampled_value) { // inline?
    real weight;
    if (arguments.size() == 0) {
      weight = 0.5;
    } else if (arguments.size() == 1) {
      weight = ToVentureType<VentureReal>(arguments[0])->data;
    } else {
      throw std::exception("Wrong number of arguments.");
    }
    if (VerifyVentureType<VentureBoolean>(sampled_value) == true) {
      if (CompareValue(sampled_value, shared_ptr<VentureValue>(new VentureBoolean(true)))) {
        return log(weight);
      } else {
        return log(1 - weight);
      }
    } else {
      cout << " " << sampled_value << endl;
      throw std::exception("Wrong value.");
    }
  }
  shared_ptr<VentureValue> Sampler(vector< shared_ptr<VentureValue> >& arguments) {
    real weight;
    if (arguments.size() == 0) {
      weight = 0.5;
    } else if (arguments.size() == 1) {
      weight = ToVentureType<VentureReal>(arguments[0])->data;
    } else {
      throw std::exception("Wrong number of arguments.");
    }
    unsigned int result_int = gsl_ran_bernoulli(random_generator, weight);
    bool result_bool = (result_int == 1);
    return shared_ptr<VentureBoolean>(new VentureBoolean(result_bool));
  }

public:
  virtual string GetName() { return "ERP__Flip"; }
};

class ERP__ConditionERP : public ERP {
  real GetSampledLoglikelihood(vector< shared_ptr<VentureValue> >& arguments,
                                 shared_ptr<VentureValue> sampled_value) { // inline?
    if (arguments.size() != 3) {
      throw std::exception("Wrong number of arguments.");
    }
    if (StandardPredicate(arguments[0]) &&
        arguments[1] == sampled_value) { // Comparing by reference.
      return log(1.0);
    } else if (!StandardPredicate(arguments[0])
               && arguments[2] == sampled_value) { // Comparing by reference.
      return log(1.0);
    } else {
      return log(0.0);
    }
  }
  shared_ptr<VentureValue> Sampler(vector< shared_ptr<VentureValue> >& arguments) {
    if (arguments.size() != 3) {
      throw std::exception("Wrong number of arguments.");
    }
    if (StandardPredicate(arguments[0])) {
      return arguments[1];
    } else {
      return arguments[2];
    }
  }

public:
  virtual bool IsRandomChoice() { return false; }
  virtual bool CouldBeRescored() { return false; }
  virtual string GetName() { return "ERP__ConditionERP"; }
};

class Primitive : public ERP {
  real GetSampledLoglikelihood(vector< shared_ptr<VentureValue> >& arguments,
                                 shared_ptr<VentureValue> sampled_value) { // inline?
    return log(1.0);
  }
  
public:
  virtual bool IsRandomChoice() { return false; }
  virtual bool CouldBeRescored() { return false; }
  virtual string GetName() { return "PrimitiveClass"; }
};

class Primitive__RealPlus : public Primitive {
  shared_ptr<VentureValue> Sampler(vector< shared_ptr<VentureValue> >& arguments) {
    shared_ptr<VentureReal> result = shared_ptr<VentureReal>(new VentureReal(0.0));
    //for (vector< shared_ptr<VentureValue> >::iterator iterator = arguments.begin();
    //     iterator != arguments.end();
    //     iterator++) {
    for (size_t index = 0; index < arguments.size(); index++) {
      //result->data += dynamic_pointer_cast<VentureInteger>(iterator->get())->data; // Too ambiguous!
      result->data += ToVentureType<VentureReal>(arguments[index])->data; // Too ambiguous!
      // Consider *real*! Consider errors!
    }
    return result;
  }

public:
  virtual string GetName() { return "Primitive__RealPlus"; }
};

int UniformDiscrete(int, int);

void MakeMHProposal();

enum MHDecision { MH_DECLINED, MH_APPROVED };

#endif
