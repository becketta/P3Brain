//
//  sites.cpp
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/29/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#include "Genome.h"
#include <random>

double Genome::insertionDeletionP=0.02;

Genome::Genome(){
	referenceCounter=1;
	ancestor=nullptr;
	ID=-1;
}

Genome::Genome(Genome *from){
	referenceCounter=1;
	ancestor=from;
	from->referenceCounter++;
	copyGenome(from);
	ID=Data::registerGenome(this);
}

Genome::~Genome(){
	Data::removeGenome(this);
	if(ancestor!=nullptr){
		ancestor->kill();
	}
}

void Genome::copyGenome(Genome *from){
	Genome *who=from;
	sites.resize(who->sites.size());
	for(int i=0;i<sites.size();i++)
		sites[i]=who->sites[i];
}

void Genome::applyMutations(double mutationRate){
	if(mutationRate>0.0){
		int nucleotides=(int)sites.size();
		int i,s,o,w;
		vector<unsigned char> buffer;
		default_random_engine generator;
		binomial_distribution<int> distribution(nucleotides,mutationRate);
		int localMutations= distribution(generator);
		for(i=0;i<localMutations;i++)
			sites[rand()%nucleotides]=rand()&255;
		if((((double)rand()/(double)RAND_MAX)<insertionDeletionP)&&(sites.size()<20000)){
			//duplication
			w=128+rand()%(512-128);
			s=rand()%((int)sites.size()-w);
			o=rand()%(int)sites.size();
			buffer.clear();
			buffer.insert(buffer.begin(),sites.begin()+s,sites.begin()+s+w);
			sites.insert(sites.begin()+o,buffer.begin(),buffer.end());
		}
		if((((double)rand()/(double)RAND_MAX)<insertionDeletionP)&&(sites.size()>1000)){
			//deletion
			w=128+rand()%(512-128);
			s=rand()%((int)sites.size()-w);
			sites.erase(sites.begin()+s,sites.begin()+s+w);
		}
	}
}

void Genome::kill(){
	referenceCounter--;
	if(referenceCounter<=0){
		delete this;
	}
}

void Genome::fillRandom(){
	sites.resize(5000);
	for(int i=0;i<sites.size();i++)
		sites[i]=(unsigned char)rand()&255;
	for(int codon=42;codon<50;codon++)
		for(int i=0;i<4;i++){
			int j=rand()%(int)(sites.size()-2);
			sites[j]=codon;
			sites[j+1]=256-codon;
		}
}

Genome* Genome::makeMutatedOffspring(double mutationRate){
	Genome* G=new Genome(this);
	G->applyMutations(mutationRate);
	return G;
}

void Genome::makePointMutation(){
	sites[rand()%sites.size()]=(unsigned char)(rand()&255);
}


void Genome::saveToFile(FILE *F){
	for(int i=0;i<sites.size();i++)
		fprintf(F,"	%i",sites[i]);
	fprintf(F,"\n");
}