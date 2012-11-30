
#ifndef VENTURE___HEADER_H
#define VENTURE___HEADER_H

#include <algorithm>
#include <iostream>
#include <vector>
#include <list>
#include <string>
#include <map>
#include <set>
#include <limits>
#include <queue>
#include <stack>
#include "boost/weak_ptr.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/lexical_cast.hpp"
#include "boost/algorithm/string.hpp"
#include "boost/enable_shared_from_this.hpp"

using std::vector;
using boost::weak_ptr;
using boost::shared_ptr;
using boost::dynamic_pointer_cast;
using std::string;
using std::list;
using std::set;
using std::map;
using std::queue;
using std::stack;
using std::priority_queue;

// For basic output.
using std::cout;
using std::cin;
using std::endl;

typedef double real;
const real REAL_MINUS_INFINITY = std::numeric_limits<double>::min();

const real comparison_epsilon = 0.00000001;

#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
extern gsl_rng* random_generator;

#include <ctime>

#endif
