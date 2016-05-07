#ifndef __BasicMarkovBrainTemplate__Gate_Builder__
#define __BasicMarkovBrainTemplate__Gate_Builder__

#include "../../../Utilities/Parameters.h"
#include "DeterministicGate.h"
#include "FeedbackGate.h"
#include "FixedEpsilonGate.h"
#include "GPGate.h"
#include "NeuronGate.h"
#include "ProbabilisticGate.h"
#include "ThresholdGate.h"
#include "TritDeterministicGate.h"
#include "VoidGate.h"

class Gate_Builder {  // manages what kinds of gates can be built
public:

	static shared_ptr<bool> usingProbGate;
	static shared_ptr<int> probGateInitialCount;
	static shared_ptr<bool> usingDetGate;
	static shared_ptr<int> detGateInitialCount;
	static shared_ptr<bool> usingEpsiGate;
	static shared_ptr<int> epsiGateInitialCount;
	static shared_ptr<bool> usingVoidGate;
	static shared_ptr<int> voidGateInitialCount;

	static shared_ptr<bool> usingFBGate;
	static shared_ptr<int> fBGateInitialCount;
	static shared_ptr<bool> usingGPGate;
	static shared_ptr<int> gPGateInitialCount;
	static shared_ptr<bool> usingThGate;
	static shared_ptr<int> thGateInitialCount;

	static shared_ptr<bool> usingTritDeterministicGate;
	static shared_ptr<int> tritDeterministicGateInitialCount;

	static shared_ptr<bool> usingNeuronGate;
	static shared_ptr<int> neuronGateInitialCount;

	set<int> inUseGateTypes;
	set<string> inUseGateNames;
	vector<vector<int>> gateStartCodes;

	map<int, int> intialGateCounts;

	shared_ptr<ParametersTable> PT = nullptr;

	Gate_Builder() = default;
	Gate_Builder(shared_ptr<ParametersTable> _PT) {
		PT = _PT;
	}

	~Gate_Builder() = default;

	void AddGate(int gateType, function<shared_ptr<AbstractGate>(shared_ptr<AbstractGenome::Handler>, int gateID)> theFunction);
	void setupGates();
	vector<function<shared_ptr<AbstractGate>(shared_ptr<AbstractGenome::Handler>, int gateID)>> makeGate;

	//int getIOAddress(shared_ptr<AbstractGenome::Handler> genomeHandler, shared_ptr<AbstractGenome> genome, int gateID);  // extracts one brain state value address from a genome
	static void getSomeBrainAddresses(const int& howMany, const int& howManyMax, vector<int>& addresses, shared_ptr<AbstractGenome::Handler> genomeHandler, int code, int gateID);  // extracts many brain state value addresses from a genome
	static pair<vector<int>, vector<int>> getInputsAndOutputs(const pair<int, int> insRange, const pair<int, int>, shared_ptr<AbstractGenome::Handler> genomeHandle, int gateID);  // extracts the input and output brain state value addresses for this gate

	/* *** some c++ 11 magic to speed up translation from genome to gates *** */
	//function<shared_ptr<Gate>(shared_ptr<AbstractGenome::Handler> genomeHandler, int gateID)> Gate_Builder::makeGate[256];
};

#endif /* defined(__BasicMarkovBrainTemplate__Gate_Builder__) */

