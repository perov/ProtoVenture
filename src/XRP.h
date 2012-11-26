
#ifndef VENTURE___XRP_H
#define VENTURE___XRP_H

#include "VentureValues.h"

class XRP {
  virtual shared_ptr<VentureValue> Sampler(vector< shared_ptr<VentureValue> >& arguments);
  virtual double GetSampledLoglikelihood(vector< shared_ptr<VentureValue> >& arguments,
                                         shared_ptr<VentureValue> sampled_value);
  virtual void Incorporate(vector< shared_ptr<VentureValue> >& arguments,
                           shared_ptr<VentureValue> sampled_value);

public:
  
  XRP(shared_ptr<XRP> maker) : maker(maker) {}
  shared_ptr<VentureValue> Sample(vector< shared_ptr<VentureValue> >& arguments) {
    shared_ptr<VentureValue> new_sample = Sampler(arguments);
    double loglikelihood = GetSampledLoglikelihood(arguments, new_sample);
    Incorporate(arguments, new_sample);
  }

private:
  shared_ptr<XRP> maker;
};

class ERP : public XRP {
  void Incorporate(vector< shared_ptr<VentureValue> >& arguments,
                   shared_ptr<VentureValue> sampled_value) { // inline?

  }
};

class Primitive : public ERP {
  double GetSampledLoglikelihood(vector< shared_ptr<VentureValue> >& arguments,
                                 shared_ptr<VentureValue> sampled_value) { // inline?
    return 0.0;
  }
};

class Primitive__Sum : public Primitive {
  shared_ptr<VentureValue> Sampler(vector< shared_ptr<VentureValue> >& arguments) {
    shared_ptr<VentureInteger> result = shared_ptr<VentureInteger>(new VentureInteger(0));
    //for (vector< shared_ptr<VentureValue> >::iterator iterator = arguments.begin();
    //     iterator != arguments.end();
    //     iterator++) {
    for (size_t index = 0; index < arguments.size(); index++) {
      //result->data += dynamic_pointer_cast<VentureInteger>(iterator->get())->data; // Too ambiguous!
      result->data += dynamic_pointer_cast<VentureInteger>(arguments[index])->data; // Too ambiguous!
      // Consider *real*! Consider errors!
    }
  }
};

#endif
