//  MABE is a product of The Hintza Lab @ MSU
//     for general research information:
//         http://hintzelab.msu.edu/
//     for MABE documentation:
//         https://github.com/ahnt/BasicMarkovBrainTemplate/wiki - for
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//          https://github.com/ahnt/BasicMarkovBrainTemplate/wiki/license


#ifndef __BasicMarkovBrainTemplate__SSwD_Archivist__
#define __BasicMarkovBrainTemplate__SSwD_Archivist__

#include "../DefaultArchivist.h"

using namespace std;

class SSwDArchivist : public DefaultArchivist {  // SnapShot with Delay
 public:
	static shared_ptr<ParameterLink<string>> SSwD_Arch_dataSequenceStrPL;  // how often to write out data
	static shared_ptr<ParameterLink<string>> SSwD_Arch_genomeSequenceStrPL;  // how often to write out genomes
	static shared_ptr<ParameterLink<int>> SSwD_Arch_dataDelayPL;  // when using SSwD, how long is the delay
	static shared_ptr<ParameterLink<int>> SSwD_Arch_genomeDelayPL;  // when using SSwD, how long is the delay
	static shared_ptr<ParameterLink<int>> SSwD_Arch_cleanupIntervalPL;  // how often to attempt to prune the LOD
	static shared_ptr<ParameterLink<string>> SSwD_Arch_DataFilePrefixPL;  // name of the Data file
	static shared_ptr<ParameterLink<string>> SSwD_Arch_GenomeFilePrefixPL;  // name of the Genome file (genomes on LOD)
	static shared_ptr<ParameterLink<bool>> SSwD_Arch_writeDataFilesPL;  // if true, write data file
	static shared_ptr<ParameterLink<bool>> SSwD_Arch_writeGenomeFilesPL;  // if true, write genome file

	vector<int> dataSequence;  // how often to write out data
	vector<int> genomeSequence;  // how often to write out data
	int dataDelay;  // when using SSwD, how long is the delay
	int genomeDelay;  // when using SSwD, how long is the delay
	int cleanupInterval;  // how often to attempt to prune the LOD
	string DataFilePrefix;  // name of the Data file
	string GenomeFilePrefix;  // name of the Genome file (genomes on LOD)
	bool writeDataFiles;  // if true, write data file
	bool writeGenomeFiles;  // if true, write genome file

	//// info about files under management

	int writeDataSeqIndex;
	int checkPointDataSeqIndex;
	int writeGenomeSeqIndex;
	int checkPointGenomeSeqIndex;

	int nextDataWrite;  // next time data files will be written to disk
	int nextGenomeWrite;  // next time a genome file will be written to genome.csv
	int nextDataCheckPoint;  // next time data files contents need to be checkpointed (this is needed particularly to handle delay > interval)
	int nextGenomeCheckPoint;  // next time a genome files contents need to be checkpointed (this is needed particularly to handle delay > interval)

	map<int, vector<weak_ptr<Organism>>> checkpoints;  // used by SSwD only - this keeps lists of orgs that may be written (if they have living decendents)
	// key is Global::nextGenomeWrite or Global::nextDataWrite

	SSwDArchivist() = delete;
	SSwDArchivist(vector<string> aveFileColumns = {}, shared_ptr<ParametersTable> _PT = nullptr);

	virtual ~SSwDArchivist() = default;

	///// CLEANUP / DELETE STALE CHECKPOINTS
	// if a checkpoint is from before Global::update - archivist::intervalDelay than delete the checkpoint
	// and all of it's org parents (with clear) assuming org was dead at the time
	// this will have the effect of a delayed pruning, but should do a good enough job keeping memory down.
	void cleanup();

	virtual bool archive(vector<shared_ptr<Organism>> population, int flush = 0) override;
};

#endif /* defined(__BasicMarkovBrainTemplate__SSwD_Archivist__) */
