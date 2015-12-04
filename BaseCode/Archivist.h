//
//  Archivist.h
//  BasicMarkovBrainTemplate
//
//  Created by Cliff Bohm on 5/30/15.
//

#ifndef __BasicMarkovBrainTemplate__Archivist__
#define __BasicMarkovBrainTemplate__Archivist__

#include <algorithm>
#include <memory>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "Brain.h"
#include "Genome.h"
#include "Global.h"
#include "Organism.h"

//#include "Utilities/Parameters.h"
#include "../Utilities/Data.h"
#include "../Utilities/Utilities.h"

using namespace std;

class Default_Archivist {
 public:

  static string& Arch_outputMethodStr;  // string parameter for outputMethod;
  ////static int outputMethod;  // 0 = LODwAP (Line of Decent with Aggressive Pruning), 1 = SSwD (SnapShot with Delay)

  static int& Arch_realtimeFilesInterval;  // how often to write out data
  static bool& Arch_writeAveFile;  // if true, ave file will be created
  static bool& Arch_writeDominantFile;  // if true, dominant file will be created
  static string& Arch_AveFileName;  // name of the Averages file (ave for all brains when file is written to)
  static string& Arch_DominantFileName;  // name of the Dominant file (all stats for best brain when file is writtne to)
  static string& Arch_DefaultAveFileColumnNames;  // data to be saved into average file (must be values that can generate an average)

  int realtimeFilesInterval;  // how often to write out data
  bool writeAveFile;  // if true, ave file will be created
  bool writeDominantFile;  // if true, dominant file will be created
  string AveFileName;  // name of the Averages file (ave for all brains when file is written to)
  string DominantFileName;  // name of the Dominant file (all stats for best brain when file is writtne to)

  map<string, vector<string>> files;  // list of files (NAME,LIST OF COLUMNS)
  vector<string> DefaultAveFileColumns;  // what columns will be written into the AveFile

  bool finished;  // if finished, then as far as the archivist is concerned, we can stop the run.

  Default_Archivist() {
    realtimeFilesInterval = Arch_realtimeFilesInterval;
    writeAveFile = Arch_writeAveFile;
    writeDominantFile = Arch_writeDominantFile;
    AveFileName = Arch_AveFileName;
    DominantFileName = Arch_DominantFileName;
    convertCSVListToVector(Arch_DefaultAveFileColumnNames, DefaultAveFileColumns);
    finished = false;
  }

  virtual ~Default_Archivist() = default;

  void writeRealTimeFiles(vector<shared_ptr<Organism>> &population) {  // write Ave and Dominant files NOW!
    // write out Ave
    if (writeAveFile) {
      double aveValue, temp;
      DataMap AveMap;
      for (auto key : DefaultAveFileColumns) {
        aveValue = 0;
        for (auto org : population) {
          stringstream ss(org->dataMap.Get(key));
          ss >> temp;
          aveValue += temp;
        }
        aveValue /= population.size();
        AveMap.Set(key, aveValue);
      }
      AveMap.writeToFile(AveFileName, DefaultAveFileColumns);
    }
    // write out Dominant
    if (writeDominantFile) {
      vector<double> Scores;
      for (auto org : population) {
        Scores.push_back(org->score);
      }

      int best = findGreatestInVector(Scores);
      population[best]->dataMap.writeToFile(DominantFileName);
    }
  }

  // save data and manage in memory data
  // return true if next save will be > updates + terminate after
  virtual bool archive(vector<shared_ptr<Organism>> population, int flush = 0) {
    if (flush != 1) {
      if ((Global::update % realtimeFilesInterval == 0) && (flush == 0)) {  // do not write files on flush - these organisms have not been evaluated!
        writeRealTimeFiles(population);  // write to dominant and average files
      }
      for (auto org : population) {  // we don't need to worry about tracking parents or lineage, so we clear out this data every generation.
        org->clearHistory();
      }
    }
    // if we are at the end of the run
    return (Global::update >= Global::updates);
  }

};

class LODwAP_Archivist : Default_Archivist {  // Line of Decent with Active Pruning
 public:

  static int& LODwAP_Arch_dataInterval;  // how often to write out data
  static int& LODwAP_Arch_genomeInterval;  // how often to write out genomes
  static int& LODwAP_Arch_pruneInterval;  // how often to attempt to prune the LOD
  static int& LODwAP_Arch_terminateAfter;  // how long to run after updates (to get better coalescence)
  static string& LODwAP_Arch_DataFileName;  // name of the Data file
  static string& LODwAP_Arch_GenomeFileName;  // name of the Genome file (genomes on LOD)

  int dataInterval;  // how often to write out data
  int genomeInterval;  // how often to write out genomes
  int pruneInterval;  // how often to attempt to prune the LOD
  int terminateAfter;  // how long to run after updates (to get better coalescence)
  string DataFileName;  // name of the Data file
  string GenomeFileName;  // name of the Genome file (genomes on LOD)

  int lastPrune;  // last time Genome was Pruned

  //// info about files under management
  int nextDataWrite;  // next time data files will be written to disk
  int nextGenomeWrite;  // next time a genome file will be written to genome.csv
  int nextDataCheckPoint;  // next time data files contents need to be checkpointed (this is needed particularly to handle delay > interval)
  int nextGenomeCheckPoint;  // next time a genome files contents need to be checkpointed (this is needed particularly to handle delay > interval)

  //unordered_map<int, vector<weak_ptr<Organism>>> checkpoints;  // used by SSwD only - this keeps lists of orgs that may be written (if they have living decendents)
  //// key is Global::nextGenomeWrite or Global::nextDataWrite

  LODwAP_Archivist()
      : Default_Archivist() {

    dataInterval = LODwAP_Arch_dataInterval;
    genomeInterval = LODwAP_Arch_genomeInterval;
    pruneInterval = LODwAP_Arch_pruneInterval;
    terminateAfter = LODwAP_Arch_terminateAfter;
    DataFileName = LODwAP_Arch_DataFileName;
    GenomeFileName = LODwAP_Arch_GenomeFileName;

    nextDataWrite = 0;
    nextGenomeWrite = 0;
    nextDataCheckPoint = 0;
    nextGenomeCheckPoint = 0;

    lastPrune = 0;
  }

  virtual ~LODwAP_Archivist() = default;

  virtual bool archive(vector<shared_ptr<Organism>> population, int flush = 0) {

    if ((Global::update % dataInterval == 0) && (flush == 0)) {  // do not write files on flush - these organisms have not been evaluated!
      writeRealTimeFiles(population);  // write to dominant and average files
    }
    if ((Global::update % pruneInterval == 0) || (flush == 1)) {

      if (files.find("data.csv") == files.end()) {  // if file has not be initialized yet
        files[DataFileName] = population[0]->dataMap.getKeys();
      }

      vector<shared_ptr<Organism>> LOD = population[0]->getLOD(population[0]);  // get line of decent
      shared_ptr<Organism> effective_MRCA;
      if (flush) {  // if flush then we don't care about convergance
        cout << "flushing LODwAP: using population[0] as MRCA\n";
        effective_MRCA = population[0]->parents[0];  // this assumes that a population was created, but not tested at the end of the evolution loop!
      } else {
        effective_MRCA = population[0]->getMostRecentCommonAncestor(LOD);  // find the convergance point in the LOD.
      }
      while ((effective_MRCA->timeOfBirth >= nextDataWrite) && (nextDataWrite <= Global::updates)) {  // if there is convergence before the next data interval
        shared_ptr<Organism> current = LOD[nextDataWrite - lastPrune];
        for (auto file : files) {  // for each file in files
          current->dataMap.writeToFile(file.first, file.second);  // append new data to the file
        }
        nextDataWrite += dataInterval;
      }

      while ((effective_MRCA->timeOfBirth >= nextGenomeWrite) && (nextGenomeWrite <= Global::updates)) {  // if there is convergence before the next data interval
        shared_ptr<Organism> current = LOD[nextGenomeWrite - lastPrune];
        string dataString = to_string(nextGenomeWrite) + FileManager::separator + "\"[" + current->genome->convert_to_string() + "]\"";  // add write update and padding to genome string
        FileManager::writeToFile(GenomeFileName, dataString, "update,genome");  // write data to file
        nextGenomeWrite += genomeInterval;
      }
      // data and genomes have now been written out up till the MRCA
      // so all data and genomes from before the MRCA can be deleted
      effective_MRCA->parents.clear();
      lastPrune = effective_MRCA->timeOfBirth;            // this will hold the time of the oldest genome in RAM
    }

    // if we have reached the end of time OR we have pruned past updates (i.e. written out all data up to updates), then we ae done!
    return (Global::update >= Global::updates + terminateAfter || lastPrune >= Global::updates);
  }

};

class SnapShot_Archivist : Default_Archivist {  // SnapShot
 public:
  static int& SS_Arch_snapshotInterval;  // how often to output snapshots

  int snapshotInterval;

  SnapShot_Archivist()
      : Default_Archivist() {
    snapshotInterval = SS_Arch_snapshotInterval;
  }

  ~SnapShot_Archivist() = default;

  void saveSnapshot(vector<shared_ptr<Organism>> population, int update) {

    // write out genomes
    string genomeFileName = "genomes_" + to_string(update) + ".csv";

    string dataString;
    for (auto org : population) {
      dataString = to_string(org->ID) + FileManager::separator + "\"[" + org->genome->convert_to_string() + "]\"";  // add interval update, genome ancestors, and genome with padding to string
      FileManager::writeToFile(genomeFileName, dataString, "ID,genome");  // write data to file
    }

    // write out data
    string dataFileName = "data_" + to_string(update) + ".csv";

    if (files.find("data") == files.end()) {  // first make sure that the dataFile has been set up.
      files["data"] = population[0]->dataMap.getKeys();  // get all keys from the valid orgs dataMap (all orgs should have the same keys in their dataMaps)
    }
    for (auto org : population) {
      org->dataMap.writeToFile(dataFileName, files["data"]);  // append new data to the file
    }
  }

  virtual bool archive(vector<shared_ptr<Organism>> population, int flush = 0) {
    if (flush != 1) {

      if ((Global::update % realtimeFilesInterval == 0) && (flush == 0)) {  // do not write files on flush - these organisms have not been evaluated!
        writeRealTimeFiles(population);  // write to dominant and average files
      }
      if ((Global::update % snapshotInterval == 0) && (flush == 0)) {  // do not write files on flush - these organisms have not been evaluated!
        saveSnapshot(population, Global::update);
      }
      for (auto org : population) {  // we don't need to worry about tracking parents or lineage, so we clear out this data every generation.
        org->clearHistory();
      }
    }
    // if we are at the end of the run
    return (Global::update >= Global::updates);
  }

};

class SSwD_Archivist : Default_Archivist {  // SnapShot with Delay
 public:
  static int& SSwD_Arch_dataInterval;  // how often to write out data
  static int& SSwD_Arch_genomeInterval;  // how often to write out genomes
  static int& SSwD_Arch_intervalDelay;  // when using SSwD, how long is the delay
  static int& SSwD_Arch_cleanupInterval;  // how often to attempt to prune the LOD
  static string& SSwD_Arch_DataFileName;  // name of the Data file
  static string& SSwD_Arch_GenomeFileName;  // name of the Genome file (genomes on LOD)

  int dataInterval;  // how often to write out data
  int genomeInterval;  // how often to write out genomes
  int intervalDelay;  // when using SSwD, how long is the delay
  int cleanupInterval;  // how often to attempt to prune the LOD
  string DataFileName;  // name of the Data file
  string GenomeFileName;  // name of the Genome file (genomes on LOD)

  //// info about files under management
  int nextDataWrite;  // next time data files will be written to disk
  int nextGenomeWrite;  // next time a genome file will be written to genome.csv
  int nextDataCheckPoint;  // next time data files contents need to be checkpointed (this is needed particularly to handle delay > interval)
  int nextGenomeCheckPoint;  // next time a genome files contents need to be checkpointed (this is needed particularly to handle delay > interval)

  unordered_map<int, vector<weak_ptr<Organism>>> checkpoints;  // used by SSwD only - this keeps lists of orgs that may be written (if they have living decendents)
  // key is Global::nextGenomeWrite or Global::nextDataWrite

  SSwD_Archivist()
  : Default_Archivist() {

    dataInterval = SSwD_Arch_dataInterval;
    genomeInterval = SSwD_Arch_genomeInterval;
    intervalDelay = SSwD_Arch_intervalDelay;
    cleanupInterval = SSwD_Arch_cleanupInterval;
    DataFileName = SSwD_Arch_DataFileName;
    GenomeFileName = SSwD_Arch_GenomeFileName;

    nextDataWrite = 0;
    nextGenomeWrite = 0;
    nextDataCheckPoint = 0;
    nextGenomeCheckPoint = 0;
  }

  virtual ~SSwD_Archivist() = default;

  ///// CLEANUP / DELETE STALE CHECKPOINTS
  // if a checkpoint is from before Global::update - archivist::intervalDelay than delete the checkpoint
  // and all of it's org parents (with clear) assuming org was dead at the time
  // this will have the effect of a delayed pruning, but should do a good enough job keeping memory down.
  void cleanup() {

    {
      vector<int> expiredCheckPoints;
      bool checkpointEmpty;
      for (auto checkpoint : checkpoints) {  // for every checkpoint
        if (checkpoint.first < (Global::update - intervalDelay)) {  // if that checkpoint is older then the intervalDelay
          checkpointEmpty = true;
          for (auto weakPtrToOrg : checkpoints[checkpoint.first]) {  // than for each element in that checkpoint
            if (auto org = weakPtrToOrg.lock()) {  // if this ptr is still good
              if ((!org->alive) && (org->timeOfDeath < (Global::update - intervalDelay))) {  // and if the organism was dead at the time of this checkpoint
                org->parents.clear();// clear this organisms parents
              } else {
                checkpointEmpty = false;  // there is an organism in this checkpoint that was alive later then (Global::update - intervalDelay)
                                          // this could organism could still be active
              }
            }
          }
          if (checkpointEmpty) {  // if all of the organisims in this checkpoint have been dead for a good long time...
                                  // if this checkpoint is not empty, we want to keep it so we can check it again later.
            expiredCheckPoints.push_back(checkpoint.first);// add this checkpoint to a list of checkpoints to be deleted
          }
        }
      }
      for (auto checkPointTime : expiredCheckPoints) {  // for each checkpoint in the too be deleted list
        checkpoints.erase(checkPointTime);// delete this checkpoint
      }
    }
  }

  virtual bool archive(vector<shared_ptr<Organism>> population, int flush = 0) {

    if (flush != 1) {

      ///// Clean up the checkpoints
      if (Global::update % cleanupInterval == 0) {
        cleanup();
      }

      ///// ADDING TO THE ARCHIVE

      if (Global::update == nextGenomeCheckPoint || Global::update == nextDataCheckPoint) {  // if we are at a data or genome interval...
                                                                                             // we need to make a checkpoint of the current population
        for (auto org : population) {  // add the current population to checkPointTracker
          checkpoints[Global::update].push_back(org);
          org->snapShotDataMaps[Global::update] = org->dataMap;// back up state of dataMap

          if (Global::update == nextGenomeCheckPoint) {  // if this is a genome interval, add genomeAncestors to snapshot dataMap
            for (auto ancestor : org->genomeAncestors) {
              org->snapShotDataMaps[Global::update].Append("genomeAncestors", ancestor);
            }
            org->genomeAncestors.clear();  // clear genomeAncestors (this data is safe in the checkPoint)
            org->genomeAncestors.insert(org->ID);// now that we have saved the ancestor data, set ancestors to self (so that others will inherit correctly)
                                                 // also, if this survives over intervals, it'll be pointing to self as ancestor in files (which is good)
          }

          if (Global::update == nextDataCheckPoint) {  // if this is a data interval, add dataAncestors to snapshot dataMap
            for (auto ancestor : org->dataAncestors) {
              org->snapShotDataMaps[Global::update].Append("dataAncestors", ancestor);
            }
            org->dataAncestors.clear();  // clear dataAncestors (this data is safe in the checkPoint)
            org->dataAncestors.insert(org->ID);// now that we have saved the ancestor data, set ancestors to self (so that others will inherit correctly)
                                               // also, if this survives over intervals, it'll be pointing to self as ancestor in files (which is good)
          }
        }
        if (Global::update == nextGenomeCheckPoint) {  // we have now made a genome checkpoint, advance nextGenomeCheckPoint to get ready for the next interval
          nextGenomeCheckPoint += genomeInterval;
        }
        if (Global::update == nextDataCheckPoint) {  // we have now made a data checkpoint, advance nextDataCheckPoint to get ready for the next interval
          nextDataCheckPoint += dataInterval;
        }
      }

      ////// WRITING FROM THE ARCHIVE

      ////// WRITING GENOMES

      if (Global::update == nextGenomeWrite + intervalDelay) {  // now it's time to write genomes in the checkpoint at time nextGenomeWrite
        string genomeFileName = "genomes_" + to_string(nextGenomeWrite) + ".csv";

        string dataString;
        size_t index = 0;
        while (index < checkpoints[nextGenomeWrite].size()) {
          if (auto org = checkpoints[nextGenomeWrite][index].lock()) {  // this ptr is still good
            dataString = to_string(org->ID) + FileManager::separator + org->snapShotDataMaps[nextGenomeWrite].Get("genomeAncestors") + FileManager::separator + "\"[" + org->genome->convert_to_string() + "]\"";// add interval update, genome ancestors, and genome with padding to string
            FileManager::writeToFile(genomeFileName, dataString, "ID,genomeAncestors,genome");// write data to file
            index++;// advance to nex element
          } else {  // this ptr is expired - cut it out of the vector
            swap(checkpoints[nextGenomeWrite][index], checkpoints[nextGenomeWrite].back());// swap expired ptr to back of vector
            checkpoints[nextGenomeWrite].pop_back();// pop expired ptr from back of vector
          }
        }
        nextGenomeWrite += genomeInterval;  // we are done with this interval, get ready for the next one.
      }

      ////// WRITING DATA

      if (Global::update == nextDataWrite + intervalDelay) {
        // now it's time to write data in the checkpoint at time nextDataWrite
        string dataFileName = "data_" + to_string(nextDataWrite) + ".csv";

        if (files.find("data") == files.end()) {  // if file info has not been initialized yet, find a valid org and extract it's keys
          bool found = false;
          shared_ptr<Organism> org;

          while (!found) {  // check each org in checkPointTraker[nextDataWrite] until we find a valid org
            if (auto temp_org = checkpoints[nextDataWrite][0].lock()) {  // this ptr is still good
              org = temp_org;
              found = true;
            } else {  // it' empty, swap to back and remove.
              swap(checkpoints[nextDataWrite][0], checkpoints[nextDataWrite].back());// swap expired ptr to back of vector
              checkpoints[nextDataWrite].pop_back();// pop expired ptr from back of vector
            }
          }

          vector<string> tempKeysList = org->snapShotDataMaps[nextDataWrite].getKeys();  // get all keys from the valid orgs dataMap (all orgs should have the same keys in their dataMaps)
          for (auto key : tempKeysList) {  // for every key in dataMap...
            if (key != "genomeAncestors") {  // as long as it's not genomeAncestors... (genomeAncestors may be in the dataMap if a genome and data interval overlap)
              files["data"].push_back(key);// add it to the list of keys associated with the genome file.
            }
          }
        }

        // write out data for all orgs in checkPointTracker[Global::nextGenomeWrite] to "genome_" + to_string(Global::nextGenomeWrite) + ".csv"

        size_t index = 0;
        while (index < checkpoints[nextDataWrite].size()) {
          if (auto org = checkpoints[nextDataWrite][index].lock()) {  // this ptr is still good
            org->snapShotDataMaps[nextDataWrite].writeToFile(dataFileName, files["data"]);// append new data to the file
            index++;// advance to nex element
          } else {  // this ptr is expired - cut it out of the vector
            swap(checkpoints[nextDataWrite][index], checkpoints[nextDataWrite].back());// swap expired ptr to back of vector
            checkpoints[nextDataWrite].pop_back();// pop expired ptr from back of vector
          }
        }
        nextDataWrite += dataInterval;  // we are done with this interval, get ready for the next one.
      }
    }
    // if enough time has passed to save all data and genomes, then we are done!
    return (Global::update >= Global::updates + dataInterval && Global::update >= Global::updates + genomeInterval);
  }

};

//
//    if (outputMethod == -1) {  // this is the first time archive is called. get the output method
//      if (outputMethodStr == "LODwAP") {
//        outputMethod = 0;
//      } else if (outputMethodStr == "SSwD") {
//        outputMethod = 1;
//      } else {
//        cout << "unrecognized archive method \"" << outputMethodStr << "\". Should be either \"LODwAP\" or \"SSwD\"\nExiting.\n";
//      }
//    }
//
//    if (outputMethod == 0) {
//      if ((Global::update % pruneInterval == 0) || (flush == 1)) {
//
//        if (files.find("data.csv") == files.end()) {  // if file has not be initialized yet
//          files[DataFileName] = population[0]->dataMap.getKeys();
//        }
//
//        vector<shared_ptr<Organism>> LOD = population[0]->getLOD(population[0]);  // get line of decent
//        shared_ptr<Organism> effective_MRCA;
//        if (flush) {  // if flush then we don't care about convergance
//          cout << "flushing LODwAP: using population[0] as MRCA\n";
//          effective_MRCA = population[0]->parents[0];// this assumes that a population was created, but not tested at the end of the evolution loop!
//        } else {
//          effective_MRCA = population[0]->getMostRecentCommonAncestor(LOD);  // find the convergance point in the LOD.
//        }
//        while ((effective_MRCA->timeOfBirth >= nextDataWrite) && (nextDataWrite <= Global::updates)) {  // if there is convergence before the next data interval
//          shared_ptr<Organism> current = LOD[nextDataWrite - lastPrune];
//          for (auto file : files) {  // for each file in files
//            current->dataMap.writeToFile(file.first, file.second);// append new data to the file
//          }
//          nextDataWrite += dataInterval;
//        }
//
//        while ((effective_MRCA->timeOfBirth >= nextGenomeWrite) && (nextGenomeWrite <= Global::updates)) {  // if there is convergence before the next data interval
//          shared_ptr<Organism> current = LOD[nextGenomeWrite - lastPrune];
//          string dataString = to_string(nextGenomeWrite) + FileManager::separator + "\"[" + current->genome->convert_to_string() + "]\"";// add write update and padding to genome string
//          FileManager::writeToFile(GenomeFileName, dataString, "update,genome");// write data to file
//          nextGenomeWrite += genomeInterval;
//        }
//        // data and genomes have now been written out up till the MRCA
//        // so all data and genomes from before the MRCA can be deleted
//        effective_MRCA->parents.clear();
//        lastPrune = effective_MRCA->timeOfBirth;// this will hold the time of the oldest genome in RAM
//      }
//    }
//
//    if (outputMethod == 1) {  // SSwD (SnapShot with Delay)
//
//      if (flush == 1) {
//        cout << "flushing SSwD: nothing needs to be done\n";
//      } else {  // not a flush - perform normal achive
//
//        ///// CLEANUP DELETE STALE CHECKPOINTS
//        // if a checkpoint is from before Global::update - archivist::intervalDelay than delete the checkpoint
//        // and all of it's org parents (with clear) assuming org was dead at the time
//        // this will have the effect of a delayed pruning, but should do a good enough job keeping memory down.
//
//        {
//          vector<int> expiredCheckPoints;
//          bool checkpointEmpty;
//          for (auto checkpoint : checkpoints) {  // for every checkpoint
//            if (checkpoint.first < (Global::update - intervalDelay)) {  // if that checkpoint is older then the intervalDelay
//              checkpointEmpty = true;
//              for (auto weakPtrToOrg : checkpoints[checkpoint.first]) {  // than for each element in that checkpoint
//                if(auto org = weakPtrToOrg.lock()) {  // if this ptr is still good
//                  if((!org->alive) && (org->timeOfDeath < (Global::update - intervalDelay))) {  // and if the organism was dead at the time of this checkpoint
//                    org->parents.clear();// clear this organisms parents
//                  } else {
//                    checkpointEmpty = false;  // there is an organism in this checkpoint that was alive later then (Global::update - intervalDelay)
//                                              // this could organism could still be active
//                  }
//                }
//              }
//              if (checkpointEmpty) {  // if all of the organisims in this checkpoint have been dead for a good long time...
//                                      // if this checkpoint is not empty, we want to keep it so we can check it again later.
//                expiredCheckPoints.push_back(checkpoint.first);// add this checkpoint to a list of checkpoints to be deleted
//              }
//            }
//          }
//          for (auto checkPointTime : expiredCheckPoints) {  // for each checkpoint in the too be deleted list
//            checkpoints.erase(checkPointTime);// delete this checkpoint
//          }
//        }
//
//        ///// ADDING TO THE ARCHIVE
//
//        if (Global::update == nextGenomeCheckPoint || Global::update == nextDataCheckPoint) {  // if we are at a data or genome interval...
//                                                                                               // we need to make a checkpoint of the current population
//          for (auto org : population) {  // add the current population to checkPointTracker
//            checkpoints[Global::update].push_back(org);
//            org->snapShotDataMaps[Global::update] = org->dataMap;// back up state of dataMap
//
//            if (Global::update == nextGenomeCheckPoint) {  // if this is a genome interval, add genomeAncestors to snapshot dataMap
//              for (auto ancestor : org->genomeAncestors) {
//                org->snapShotDataMaps[Global::update].Append("genomeAncestors",ancestor);
//              }
//              org->genomeAncestors.clear();  // clear genomeAncestors (this data is safe in the checkPoint)
//              org->genomeAncestors.insert(org->ID);// now that we have saved the ancestor data, set ancestors to self (so that others will inherit correctly)
//                                                   // also, if this survives over intervals, it'll be pointing to self as ancestor in files (which is good)
//            }
//
//            if (Global::update == nextDataCheckPoint) {  // if this is a data interval, add dataAncestors to snapshot dataMap
//              for (auto ancestor : org->dataAncestors) {
//                org->snapShotDataMaps[Global::update].Append("dataAncestors",ancestor);
//              }
//              org->dataAncestors.clear();  // clear dataAncestors (this data is safe in the checkPoint)
//              org->dataAncestors.insert(org->ID);// now that we have saved the ancestor data, set ancestors to self (so that others will inherit correctly)
//                                                 // also, if this survives over intervals, it'll be pointing to self as ancestor in files (which is good)
//            }
//          }
//          if (Global::update == nextGenomeCheckPoint) {  // we have now made a genome checkpoint, advance nextGenomeCheckPoint to get ready for the next interval
//            nextGenomeCheckPoint += genomeInterval;
//          }
//          if (Global::update == nextDataCheckPoint) {  // we have now made a data checkpoint, advance nextDataCheckPoint to get ready for the next interval
//            nextDataCheckPoint += dataInterval;
//          }
//        }
//
//        ////// WRITING FROM THE ARCHIVE
//
//        ////// WRITING GENOMES
//
//        if (Global::update == nextGenomeWrite + intervalDelay) {  // now it's time to write genomes in the checkpoint at time nextGenomeWrite
//          string genomeFileName = "genomes_" + to_string(nextGenomeWrite) + ".csv";
//
//          string dataString;
//          size_t index = 0;
//          while (index<checkpoints[nextGenomeWrite].size()) {
//            if(auto org = checkpoints[nextGenomeWrite][index].lock()) {  // this ptr is still good
//              dataString = to_string(org->ID) + FileManager::separator + org->snapShotDataMaps[nextGenomeWrite].Get("genomeAncestors") + FileManager::separator + "\"[" + org->genome->convert_to_string() + "]\"";// add interval update, genome ancestors, and genome with padding to string
//              FileManager::writeToFile(genomeFileName, dataString, "ID,genomeAncestors,genome");// write data to file
//              index++;// advance to nex element
//            } else {  // this ptr is expired - cut it out of the vector
//              swap(checkpoints[nextGenomeWrite][index],checkpoints[nextGenomeWrite].back());// swap expired ptr to back of vector
//              checkpoints[nextGenomeWrite].pop_back();// pop expired ptr from back of vector
//            }
//          }
//          nextGenomeWrite += genomeInterval;  // we are done with this interval, get ready for the next one.
//        }
//
//        ////// WRITING DATA
//
//        if (Global::update == nextDataWrite + intervalDelay) {
//          // now it's time to write data in the checkpoint at time nextDataWrite
//          string dataFileName = "data_" + to_string(nextDataWrite) + ".csv";
//
//          if (files.find("data") == files.end()) {  // if file info has not been initialized yet, find a valid org and extract it's keys
//            bool found = false;
//            shared_ptr<Organism> org;
//
//            while (!found) {  // check each org in checkPointTraker[nextDataWrite] until we find a valid org
//              if (auto temp_org = checkpoints[nextDataWrite][0].lock()) {  // this ptr is still good
//                org = temp_org;
//                found = true;
//              } else {  // it' empty, swap to back and remove.
//                swap(checkpoints[nextDataWrite][0],checkpoints[nextDataWrite].back());// swap expired ptr to back of vector
//                checkpoints[nextDataWrite].pop_back();// pop expired ptr from back of vector
//              }
//            }
//
//            vector<string> tempKeysList = org->snapShotDataMaps[nextDataWrite].getKeys();  // get all keys from the valid orgs dataMap (all orgs should have the same keys in their dataMaps)
//            for (auto key : tempKeysList) {  // for every key in dataMap...
//              if (key != "genomeAncestors") {  // as long as it's not genomeAncestors... (genomeAncestors may be in the dataMap if a genome and data interval overlap)
//                files["data"].push_back(key);// add it to the list of keys associated with the genome file.
//              }
//            }
//          }
//
//          // write out data for all orgs in checkPointTracker[Global::nextGenomeWrite] to "genome_" + to_string(Global::nextGenomeWrite) + ".csv"
//
//          size_t index = 0;
//          while (index<checkpoints[nextDataWrite].size()) {
//            if(auto org = checkpoints[nextDataWrite][index].lock()) {  // this ptr is still good
//              org->snapShotDataMaps[nextDataWrite].writeToFile(dataFileName, files["data"]);// append new data to the file
//              index++;// advance to nex element
//            } else {  // this ptr is expired - cut it out of the vector
//              swap(checkpoints[nextDataWrite][index],checkpoints[nextDataWrite].back());// swap expired ptr to back of vector
//              checkpoints[nextDataWrite].pop_back();// pop expired ptr from back of vector
//            }
//          }
//          nextDataWrite += dataInterval;  // we are done with this interval, get ready for the next one.
//        }
//      }
//    }
//  }

#endif /* defined(__BasicMarkovBrainTemplate__Archivist__) */
