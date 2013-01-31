
#include "Utilities.h"

// http://stackoverflow.com/questions/4523178/how-to-print-out-all-elements-in-a-stdstack-or-stdqueue-conveniently
template < class Type, class Container >
const Container& GetQueueContainer
    (const std::queue<Type, Container>& queue)
{
    struct HackedQueue : private std::queue<Type, Container>
    {
        static const Container& GetQueueContainer
            (const std::queue<Type, Container>& queue)
        {
            return queue.*&HackedQueue::c;
        }
    };

    return HackedQueue::GetQueueContainer(queue);
}
template < class Type, class Container >
const Container& GetStackContainer // FIXME: Should be called GetStackContainer!
    (const std::stack<Type, Container>& stack)
{
    struct HackedStack : private std::stack<Type, Container>
    {
        static const Container& container
            (const std::stack<Type, Container>& stack)
        {
            return stack.*&HackedStack::c;
        }
    };

    return HackedStack::container(stack);
}

void BlankFunction___ForGetQueueContainer() { // For some reason without this function
                                              // "GetQueueContainer" is not generated.
  queue< shared_ptr<Node> > touched_nodes;
  GetQueueContainer(touched_nodes);
}

// FIXME: Should be called ...Stack...!
void BlankFunction___ForGetStackContainer() { // For some reason without this function
                                              // "GetQueueContainer" is not generated.
  stack< shared_ptr<Node> > touched_nodes;
  GetStackContainer(touched_nodes);
}

int UniformDiscrete(int a, int b) {
  return static_cast<int>(gsl_ran_flat(random_generator, a, b + 1));
}

real NormalDistributionLoglikelihood(real sampled_value_real, real average, real sigma) {
  double loglikelihood = 0.0;
  loglikelihood -= log(sigma);
  loglikelihood -= 0.5 * log(2 * 3.14159265358979323846264338327950);
  loglikelihood -= pow(sampled_value_real - average, 2.0) / (2 * pow(sigma, 2.0));
  return loglikelihood;
}
