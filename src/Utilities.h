
#ifndef VENTURE___UTILITIES_H
#define VENTURE___UTILITIES_H

#include "Header.h"
#include "Analyzer.h"

// http://stackoverflow.com/questions/4523178/how-to-print-out-all-elements-in-a-stdstack-or-stdqueue-conveniently
template < class Type, class Container >
const Container& GetQueueContainer
    (const std::queue<Type, Container>& queue);
template < class Type, class Container >
const Container& GetStackContainer
    (const std::stack<Type, Container>& stack);

int UniformDiscrete(int a, int b);

real NormalDistributionLogLikelihood(real sampled_value_real, real average, real sigma);
real BetaDistributionLogLikelihood(real sampled_value_real, real alpha, real beta);


#endif
