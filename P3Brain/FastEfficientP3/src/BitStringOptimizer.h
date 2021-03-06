// Brian Goldman

// Defines the base class all optimization methods should inherit from.
// Provides tools for setting up configurable selection of optimization method.

#ifndef BITSTRINGOPTIMIZER_H_
#define BITSTRINGOPTIMIZER_H_

#include <memory>

#include "Util.h"
#include "Evaluation.h"
#include "Configuration.h"

using std::shared_ptr;

// Macro used to create a function which returns new instances of the desired optimization method.
#define create_optimizer(name) static shared_ptr<BitStringOptimizer> create(Generator& rand, shared_ptr<Evaluator> evaluator, Configuration& config)\
{\
	return shared_ptr<BitStringOptimizer>(new name(rand, evaluator, config));\
}

// Base class for optimization methods
class BitStringOptimizer {
 public:
  BitStringOptimizer(Generator& _rand, shared_ptr<Evaluator> _evaluator,
            Configuration& _config)
      : rand(_rand),
        evaluator(_evaluator),
        config(_config),
        length(_config.get<int>("length")) {
  }
  virtual ~BitStringOptimizer() = default;
  // External tools interact with the optimizer by telling it to "iterate",
  // and the optimizer will return true as long as it can continue to improve by iterating.
  // As example, an iteration may perform a generation of evolution, with the optimizer returning
  // false when convergence is detected.
  virtual bool iterate() = 0;

  virtual string finalize() {
    return string();
  }

 protected:
  // Tools useful to the actual optimization methods.
  Generator& rand;
  shared_ptr<Evaluator> evaluator;
  Configuration& config;
  size_t length;

};

#endif /* BITSTRINGOPTIMIZER_H_ */
