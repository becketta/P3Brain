//
//  main.cpp
//
//
//  Created by Arend Hintze on 5/29/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#include <algorithm>
#include <memory>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "Global.h"

#include "Archivist/Archivist.h"
#include "Archivist/LODwAP_Archivist.h"
#include "Archivist/snapshot_Archivist.h"
#include "Archivist/SSwD_Archivist.h"
#include "Brain/MarkovBrain.h"
#include "Brain/WireBrain.h"

#include "Genome/Genome.h"

#include "GateListBuilder/GateListBuilder.h"

#include "Group/Group.h"
#include "Optimizer/GA_Optimizer.h"
#include "Optimizer/Optimizer.h"
#include "Optimizer/Tournament2Optimizer.h"
#include "Optimizer/TournamentOptimizer.h"
#include "Organism/Organism.h"
#include "World/World.h"
#include "World/BerryWorld.h"

#include "Utilities/Parameters.h"
#include "Utilities/Random.h"
#include "Utilities/Data.h"
#include "Utilities/Utilities.h"

using namespace std;

int main(int argc, const char * argv[]) {

	cout << "\n\n" << "\tMM   MM      A       BBBBBB    EEEEEE\n" << "\tMMM MMM     AAA      BB   BB   EE\n" << "\tMMMMMMM    AA AA     BBBBBB    EEEEEE\n" << "\tMM M MM   AAAAAAA    BB   BB   EE\n" << "\tMM   MM  AA     AA   BBBBBB    EEEEEE\n" << "\n" << "\tModular    Agent      Based    Evolver\n\n\n\thttp://hintzelab.msu.edu/MABE\n\n\n";
	Parameters::initialize_parameters(argc, argv);  // loads command line and configFile values into registered parameters
	                                                // also writes out a config file if requested
	//make a node map to handle genome value to brain state address look up.
	MarkovBrain::makeNodeMap(MarkovBrain::defaultNodeMap, Global::bitsPerBrainAddress, MarkovBrain::defaultNrOfBrainStates);

	Gate_Builder::setupGates();  // determines which gate types will be in use.

	// outputDirectory must exist. If outputDirectory does not exist, no error will occur, but no data will be writen.
	FileManager::outputDirectory = Global::outputDirectory;

	if (Global::randomSeed == -1) {
		random_device rd;
		Random::getCommonGenerator().seed(rd());
	} else {
		Random::getCommonGenerator().seed(Global::randomSeed);
	}
	World *world = (World*) new BerryWorld();  //new World();
	//World *world = (World*) new World();  //new World();

// test chromosome crossover speed
//	auto C1 = make_shared<Chromosome<bool>>(10000, 2);
//	C1->fillRandom();
//	auto C2 = make_shared<Chromosome<bool>>(10000, 2);
//	C2->fillRandom();
//	auto C3 = make_shared<Chromosome<bool>>(0, 2);
//
//	for (int k = 0; k < 100; k++) {
//		for (int j = 0; j < 500; j++) {
//			for (int i = 0; i < 6; i++) {
//				//cout << "." << flush;
//				C3->crossover( { C1, C2 }, 3);
//				//cout << "*" << flush;
//			}
//			cout << "--" << flush;
//		}
//		cout << k << endl;
//	}
//
//	exit(0);

	// test other speed

//	auto testChromosome = make_shared<Chromosome<bool>>(5000, 2);
//	auto testGenome = make_shared<Genome>(testChromosome, 3, 2);
//	auto GLB = make_shared<Classic_GateListBuilder>();  // make a organism with a genome and brain (if you need to change the types here is where you do it)
//	shared_ptr<Organism> testOrg = make_shared<Organism>(make_shared<Genome>(testChromosome, 3, 2), make_shared<MarkovBrain>(make_shared<Classic_GateListBuilder>()));  // make a organism with a genome and brain (if you need to change the types here is where you do it)
//
//	for (int k = 0; k < 100; k++) {
//		for (int j = 0; j < 500; j++) {
//			//GLB->buildGateList(testGenome, 16);
//			//auto testMB = make_shared<MarkovBrain>(make_shared<Classic_GateListBuilder>());
//			auto testGenomeMany = make_shared<Genome>(testChromosome, 3, 2);
//			//auto testOrgMany = make_shared<Organism>(testOrg, testGenome);
//			cout << "*." << flush;
//		}
//		cout << k << endl;
//	}
//
//	exit(0);

////  ///// to show org in world
//  shared_ptr<Genome> testGenome = make_shared<Genome>();
//  testGenome->loadSites("genome.csv",1000);
//  shared_ptr<Organism> testOrg = make_shared<Organism>(testGenome, make_shared<Brain>());
//  world->testIndividual(testOrg,0,1);
//  exit(0);
////  ///// end to show org in world

	//////////////////
	// define population
	//////////////////

	shared_ptr<Group> group;

	{
		// a progenitor must exist - that is, one ancestor genome
		Global::update = -1;  // before there was time, there was a progenitor
		//shared_ptr<MarkovBrain> tesBrain = make_shared<MarkovBrain>(make_shared<Classic_GateListBuilder>());
		auto initalChromosome = make_shared<Chromosome<bool>>(Genome::initialChromosomeSize, 2);
		auto initalGenome = make_shared<Genome>(initalChromosome, Genome::initialChromosomes, Genome::initialPloidy);
//		vector<shared_ptr<AbstractGenome>> genomes;
//		for (int i = 0; i < 500; i++) {
//			cout << i << " " << flush;
//			initalGenome->loadGenomes("genome_10.csv", genomes);
////--//		for (auto g : genomes){
////--//			cout << g->genomeToStr() << "\n";
////--//		}
//		}
//		exit(17);

		//auto initalBrain = make_shared<MarkovBrain>(make_shared<Classic_GateListBuilder>());
		auto initalBrain = make_shared<WireBrain>();
		shared_ptr<Organism> progenitor = make_shared<Organism>(initalGenome, initalBrain);  // make a organism with a genome and brain (if you need to change the types here is where you do it)

		Global::update = 0;  // the beginning of time - now we construct the first population
		vector<shared_ptr<Organism>> population;

		for (int i = 0; i < Global::popSize; i++) {
			shared_ptr<Genome> genome = make_shared<Genome>(initalChromosome, Genome::initialChromosomes, Genome::initialPloidy);
			genome->fillRandom();
//			auto genomeHandler = genome->newHandler(genome);
//
//			for (int i = 0; i < 5; i++) {
//				genomeHandler->randomize();
//				genomeHandler->writeInt(43, 0, 255);
//				genomeHandler->writeInt(255 - 43, 0, 255);
//				genomeHandler->writeInt(1, 0, 255);
//				genomeHandler->writeInt(2, 0, 255);
//				genomeHandler->writeInt(3, 0, 255);
//			}

			shared_ptr<Organism> org = make_shared<Organism>(progenitor, genome);
			population.push_back(org);  // add a new org to population using progenitors template and a new random genome
			population[population.size() - 1]->gender = Random::getInt(0, 1);  // assign a random gender to the new org
		}
		progenitor->kill();  // the progenitor has served it's purpose.

/////// to test genome to brain conversion and coding regions, set popsize = 1 and uncomment the block below this comment
//		shared_ptr<Organism> test_org = dynamic_pointer_cast<Organism>(population[0]);
//		shared_ptr<Genome> test_genome = dynamic_pointer_cast<Genome>(test_org->genome);
//		shared_ptr<MarkovBrain> test_brain = dynamic_pointer_cast<MarkovBrain>(test_org->brain);
//
//		cout << test_genome->genomeToStr();
//
//		for (int i = 0; i < 6; i++) {
//			shared_ptr<Chromosome<int>> ch = dynamic_pointer_cast<Chromosome<int>>(test_genome->chromosomes[i]);
//			cout << "chromosome " << i << endl;
//			cout << ch->chromosomeToStr() << endl;
//			cout << ch->codingRegionsToString() << endl;
//		}
//
//		cout << endl << endl;
//
//		cout << test_brain->description();
//		exit(1);
/////// to test genome to brain conversion and coding regions, set popsize = 1 and uncomment the block above this comment

		shared_ptr<Archivist> archivist;

		if (Archivist::Arch_outputMethodStr == "default") {
			archivist = make_shared<Archivist>();
		}
		if (Archivist::Arch_outputMethodStr == "LODwAP") {
			archivist = make_shared<LODwAP_Archivist>();
		}
		if (Archivist::Arch_outputMethodStr == "snapshot") {
			archivist = make_shared<Snapshot_Archivist>();
		}
		if (Archivist::Arch_outputMethodStr == "SSwD") {
			archivist = make_shared<SSwD_Archivist>();
		}

		shared_ptr<BaseOptimizer> optimizer;

		if (BaseOptimizer::Optimizer_MethodStr == "GA") {
			optimizer = make_shared<GA_Optimizer>();
		}
		if (BaseOptimizer::Optimizer_MethodStr == "Tournament") {
			optimizer = make_shared<TournamentOptimizer>();
		}
		if (BaseOptimizer::Optimizer_MethodStr == "Tournament2") {
			optimizer = make_shared<Tournament2Optimizer>();
		}

		//optimizer = make_shared<BaseOptimizer>();
		group = make_shared<Group>(population, optimizer, archivist);
	}

//////////////////
// evolution loop
//////////////////

	bool finished = false;  // when the archivist says we are done, we can stop!

	while (!finished) {

		world->evaluateFitness(group->population, false);  // evaluate each organism in the population using a World
		cout << "  evaluation done\n";
		finished = group->archive();  // save data, update memory and delete any unneeded data;
		cout << "  archive done\n";
		Global::update++;
		group->optimize();  // update the population (reproduction and death)
		cout << "  optimize done\n";
		cout << "update: " << Global::update - 1 << "   maxFitness: " << group->optimizer->maxFitness << "\n";
	}

	group->archive(1);  // flush any data that has not been output yet

	if (Archivist::Arch_outputMethodStr == "LODwAP") {  // if using LODwAP, write out some info about MRCA
		shared_ptr<Organism> FinalMRCA = group->population[0]->getMostRecentCommonAncestor(group->population[0]);
		cout << "MRCA - ID: " << FinalMRCA->ID << " born on: " << FinalMRCA->timeOfBirth << "\n" << FinalMRCA->brain->description();
		//cout << "\n\n" << FinalMRCA->genome->showCodingRegions();
	}
	return 0;
}
