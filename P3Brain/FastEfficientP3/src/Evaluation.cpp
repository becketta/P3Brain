// Brian Goldman

// Each evaluation object defines the "evaluate" function for how
// to convert the vector of bool solution into a floating point fitness.

#include "Evaluation.h"
#include "Tools.h"
#include "BitBrain.h"

using namespace std;

// Template specialization to convert a string to a function pointer
// Used to allow configurable problem choice
template<>
evaluation::pointer Configuration::get(const string key) {
  return evaluation::lookup[get<string>(key)];
}

/*
* For this integration of P3 with Markov Brains, a MarkovWorld is the only
* evaluator that is possible and the specific world to test in is a 
* configurable option.
*/
MarkovWorld::MarkovWorld( Configuration& config, int run_number )
{
    trainingGround = config.get<evaluation::pointer>("world")();

    string type = config.get<string>( "decoder" );
    if( type == "Unstructured" )
    {
        decoderType = BitBrain::Unstructured;
    }
    else if( type == "FixedInput" )
    {
        decoderType = BitBrain::FixedInput;
    }
    else if( type == "FixedLogic" )
    {
        decoderType = BitBrain::FixedLogic;
    }
    else if( type == "Hypercube" )
    {
        decoderType = BitBrain::Hypercube;
    }
    else
    {
        ASSERT( false, "Invalid decoder type given in P3 config file." );
    }
}

/*
* The evaluate function is called by P3 every time it needs to evaluate the
* fitness of a bit string. If the evaluator is set to "MarkovWorld" by a
* P3 config file or a command line parameter WHEN RUNNING P3, then this
* function will be called by P3, allowing us to translate the bit string to
* a Markov Brain and evaluate its fitness in the 'trainingGround'.
*/
float MarkovWorld::evaluate(const vector<bool>& solution)
{
    auto org = make_shared<Organism>();
    if( decoderType != BitBrain::Unstructured )
    {
      org->brain = make_shared<BitBrain>(solution, 11, 2, decoderType);
    }
    else
    {
      org->brain = make_shared<BitBrain>(solution, 11, 10, 3, 2);
    }
    return (float)trainingGround->testIndividual(org, false);
}
