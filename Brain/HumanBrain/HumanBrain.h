//
//  Agent.h
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/30/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#ifndef __BasicMarkovBrainTemplate__HumanBrain__
#define __BasicMarkovBrainTemplate__HumanBrain__

#include <math.h>
#include <memory>
#include <iostream>
#include <set>
#include <vector>

#include "../../Genome/AbstractGenome.h"

#include "../../Utilities/Random.h"

#include "../AbstractBrain.h"


using namespace std;

class HumanBrain: public AbstractBrain {
public:

	static shared_ptr<ParameterLink<bool>> useActionMapPL;
	static shared_ptr<ParameterLink<string>> actionMapFileNamePL;

	map<char, vector<double>> actionMap;
	map<char, string> actionNames;

	HumanBrain() = delete;

	HumanBrain(int _nrInNodes, int _nrOutNodes, int _nrHiddenNodes);

	virtual ~HumanBrain() = default;

	virtual void update() override;

	virtual shared_ptr<AbstractBrain> makeBrainFromGenome(shared_ptr<AbstractGenome> _genome) override;

	virtual string description() override;
	virtual vector<string> getStats() override;

	virtual void resetBrain() override;

	virtual void initalizeGenome(shared_ptr<AbstractGenome> _genome);

};

#endif /* defined(__BasicMarkovBrainTemplate__HumanBrain__) */
