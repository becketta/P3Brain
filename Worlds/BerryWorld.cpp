//
//  BerryWorld.cpp
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 6/15/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#include <stdlib.h>     // for atoi()

#include "BerryWorld.h"

#include "../Utilities/Random.h"
#include "../Utilities/Utilities.h"

/* *** implementation of the World *** */

double& BerryWorld::TSK = Parameters::register_parameter("BERRY_taskSwitchingCost", 1.4, "cost to change food sources", "WORLD - BERRY");
int& BerryWorld::worldUpdates = Parameters::register_parameter("BERRY_WorldUpdates", 400, "amount of time an brain is tested", "WORLD - BERRY");

int& BerryWorld::foodSourceTypes = Parameters::register_parameter("BERRY_foodSourceTypes", 2, "number of types of food", "WORLD - BERRY");
double& BerryWorld::rewardForFood1 = Parameters::register_parameter("BERRY_rewardForFood1", 1.0, "reward for eating a Food1", "WORLD - BERRY");
double& BerryWorld::rewardForFood2 = Parameters::register_parameter("BERRY_rewardForFood2", 1.0, "reward for eating a Food2", "WORLD - BERRY");
double& BerryWorld::rewardForFood3 = Parameters::register_parameter("BERRY_rewardForFood3", 1.0, "reward for eating a Food3", "WORLD - BERRY");
double& BerryWorld::rewardForFood4 = Parameters::register_parameter("BERRY_rewardForFood4", 1.0, "reward for eating a Food4", "WORLD - BERRY");
double& BerryWorld::rewardForFood5 = Parameters::register_parameter("BERRY_rewardForFood5", 1.0, "reward for eating a Food5", "WORLD - BERRY");
double& BerryWorld::rewardForFood6 = Parameters::register_parameter("BERRY_rewardForFood6", 1.0, "reward for eating a Food6", "WORLD - BERRY");
double& BerryWorld::rewardForFood7 = Parameters::register_parameter("BERRY_rewardForFood7", 1.0, "reward for eating a Food7", "WORLD - BERRY");
double& BerryWorld::rewardForFood8 = Parameters::register_parameter("BERRY_rewardForFood8", 1.0, "reward for eating a Food8", "WORLD - BERRY");

int& BerryWorld::worldColumns = Parameters::register_parameter("BERRY_WorldX", 8, "world X size", "WORLD - BERRY");
int& BerryWorld::worldRows = Parameters::register_parameter("BERRY_WorldY", 8, "world Y size", "WORLD - BERRY");
bool& BerryWorld::borderWalls = Parameters::register_parameter("BERRY_makeBorderWalls", true, "if true world will have a bounding wall", "WORLD - BERRY");
int& BerryWorld::randomWalls = Parameters::register_parameter("BERRY_makeRandomWalls", 0, "add this many walls to the world", "WORLD - BERRY");

bool& BerryWorld::clearOutputs = Parameters::register_parameter("BERRY_clearOutputs", false, "if true outputs will be cleared on each world update", "WORLD - BERRY");

bool& BerryWorld::allowMoveAndEat = Parameters::register_parameter("BERRY_allowMoveAndEat", false, "if true, the brain can move and eat in the same world update", "WORLD - BERRY");
bool& BerryWorld::senseDown = Parameters::register_parameter("BERRY_senseDown", true, "if true, Agent can sense what it's standing on", "WORLD - BERRY");
bool& BerryWorld::senseFront = Parameters::register_parameter("BERRY_senseFront", true, "if true, Agent can sense what's in front of it", "WORLD - BERRY");
bool& BerryWorld::senseFrontSides = Parameters::register_parameter("BERRY_senseFrontSides", true, "if true, Agent can sense what's in front to the left and right of it", "WORLD - BERRY");
bool& BerryWorld::senseWalls = Parameters::register_parameter("BERRY_senseWalls", true, "if true, Agent can sense Walls", "WORLD - BERRY");

BerryWorld::BerryWorld() {
    senseWalls = senseWalls & (borderWalls | (randomWalls > 0)); // if there are no walls, there is no need to sense them!

    outputStatesCount = 3; // number of brain states used for output, 2 for move, 1 for eat

    if (senseWalls) {
        inputStatesCount = (senseDown * foodSourceTypes) + ((senseFront * foodSourceTypes) + senseWalls) + (2 * ((senseFrontSides * foodSourceTypes) + senseWalls));
        // sense down does not include walls (can't stand on a wall (yet!) * types of food
        // senseFront * types of food + wall, same for senseFrontSides, but there are 2
    } else { // no border walls
        inputStatesCount = (senseDown * foodSourceTypes) + (senseFront * foodSourceTypes) + (2 * (senseFrontSides * foodSourceTypes));
        // sense down * types of food, same for senseFront, same for senseFrontSides, but there are 2
    }

    cout << "  World using following BrainSates:\n    Inputs: 0 to " << inputStatesCount - 1 << "\n    Outputs: " << inputStatesCount << " to " << inputStatesCount + outputStatesCount - 1 << "\n";
}

double BerryWorld::testIndividual(shared_ptr<Organism> org, bool analyse) {
    vector<vector<int>> grid;
    grid.resize(worldRows);
    for (int rowCount = 0; rowCount < worldRows; rowCount++) {
        grid[rowCount].resize(worldColumns);
    }

    // agent starts in the center of the world, facing in a random direction.
    int currentColumn = worldColumns / 2; // column being occupied by agent
    int currentRow = worldRows / 2; // row being occupied by agent
    int facing = Random::getInt(7); // direction the agent is facing

    int switches = 0;
    int statesAssignmentCounter;

    vector<int> stateCollector;
    double score = 0.0;
    int lastFood = -1; //nothing has been eaten yet!

    vector<double> foodRewards;
    foodRewards.resize(8);
    foodRewards[0] = rewardForFood1;
    foodRewards[1] = rewardForFood2;
    foodRewards[2] = rewardForFood3;
    foodRewards[3] = rewardForFood4;
    foodRewards[4] = rewardForFood5;
    foodRewards[5] = rewardForFood6;
    foodRewards[6] = rewardForFood7;
    foodRewards[7] = rewardForFood8;

    vector<int> eaten; //the array eaten has 2 elements and they are both 0
    eaten.resize(foodSourceTypes);

    if (borderWalls) {
        for (int row = 1; row < worldRows - 1; row++)
            for (int column = 1; column < worldColumns - 1; column++) { //as you go across the x and y directions in grid...
                grid[row][column] = Random::getInt(1, foodSourceTypes); // plant red or blue food
            }
        for (int column = 0; column < worldColumns; column++) { //makes horizontal walls
            grid[0][column] = WALL;
            grid[worldRows - 1][column] = WALL;
        }
        for (int row = 0; row < worldRows; row++) { //makes vertical walls
            grid[row][0] = WALL;
            grid[row][worldColumns - 1] = WALL;
        }

    } else { // no border walls
        for (int row = 0; row < worldRows; row++) {
            for (int column = 0; column < worldColumns; column++) { //as you go across the x and y directions in grid...
                grid[row][column] = Random::getInt(1, foodSourceTypes); // plant red or blue food
            }
        }
    }
    for (int i = 0; i < randomWalls; i++) { // add walls
        grid[Random::getIndex(worldRows)][Random::getIndex(worldColumns)] = WALL;
    }

    org->brain->resetBrain();

    int output1 = 0;
    int output2 = 0;

    int here, leftFront, front, rightFront;

    for (int t = 0; t < worldUpdates; t++) { //run agent for "worldUpdates" brain updates

        here = grid[currentColumn][currentRow]; // where the agent is standing
        front = grid[loopMod((currentColumn + xm[facing]), (worldColumns))][loopMod((currentRow + ym[facing]), (worldRows))];
        leftFront = grid[loopMod((currentColumn + xm[(facing - 1) & 7]), (worldColumns))][loopMod((currentRow + ym[(facing - 1) & 7]), (worldRows))];
        rightFront = grid[loopMod((currentColumn + xm[(facing + 1) & 7]), (worldColumns))][loopMod((currentRow + ym[(facing + 1) & 7]), (worldRows))];

        statesAssignmentCounter = 0;

        if (senseWalls) {
            if (senseDown) {
                for (int i = 0; i < foodSourceTypes; i++) { // fill first states with food values at here location
                    org->brain->setState(statesAssignmentCounter++, (here == i + 1));
                }
            }
            if (senseFront) {
                for (int i = 0; i < foodSourceTypes; i++) { // fill first states with food values at front location
                    org->brain->setState(statesAssignmentCounter++, (front == i + 1));
                }
                org->brain->setState(statesAssignmentCounter++, (front == WALL));
            }
            if (senseFrontSides) {
                for (int i = 0; i < foodSourceTypes; i++) { // fill first states with food values at front location
                    org->brain->setState(statesAssignmentCounter++, (leftFront == i + 1));
                    org->brain->setState(statesAssignmentCounter++, (rightFront == i + 1));
                }
                org->brain->setState(statesAssignmentCounter++, (leftFront == WALL));
                org->brain->setState(statesAssignmentCounter++, (rightFront == WALL));
            }
        } else { // don't sense walls
            if (senseDown) {
                for (int i = 0; i < foodSourceTypes; i++) { // fill first states with food values at here location
                    org->brain->setState(statesAssignmentCounter++, (here == i + 1));
                }
            }
            if (senseFront) {
                for (int i = 0; i < foodSourceTypes; i++) { // fill first states with food values at front location
                    org->brain->setState(statesAssignmentCounter++, (front == i + 1));
                }
            }
            if (senseFrontSides) {
                for (int i = 0; i < foodSourceTypes; i++) { // fill first states with food values at front location
                    org->brain->setState(statesAssignmentCounter++, (leftFront == i + 1));
                    org->brain->setState(statesAssignmentCounter++, (rightFront == i + 1));
                }
            }
        }

        if (clearOutputs) {
            org->brain->setState(inputStatesCount, 0.0);
            org->brain->setState(inputStatesCount + 1, 0.0);
            org->brain->setState(inputStatesCount + 2, 0.0);
        }

        if (analyse) { // gather some data before and after running update
            int S = 0;
            for (int i = 0; i < inputStatesCount; i++)
                S = (S << 1) + Bit(org->brain->states[i]);
            org->brain->update();
            for (int i = inputStatesCount + outputStatesCount; i < org->brain->nrOfBrainStates; i++)
                S = (S << 1) + Bit(org->brain->states[i]);
            stateCollector.push_back(S);
        } else {
            org->brain->update(); // just run the update!
        }

        // set output values
        output1 = Bit(org->brain->getState(inputStatesCount)) + (Bit(org->brain->getState(inputStatesCount + 1)) << 1);
        output2 = Bit(org->brain->getState(inputStatesCount + 2));

        if ((output2 == 1) && (grid[currentColumn][currentRow] != EMPTY)) { // eat food here (if there is food here)
            if (lastFood != -1) { // if some food has already been eaten
                if (lastFood != grid[currentColumn][currentRow]) { // if this food is different then the last food eaten
                    score -= BerryWorld::TSK; // pay the task switch cost
                    switches++;
                }
            }
            score += foodRewards[grid[currentColumn][currentRow] - 1]; // you ate a food... good for you!
            lastFood = grid[currentColumn][currentRow]; // remember the last food eaten
            eaten[grid[currentColumn][currentRow] - 1]++; // track the number of each berry eaten
            org->dataMap.Append("foodList", grid[currentColumn][currentRow]);
            grid[currentColumn][currentRow] = 0; // clear this location
        }
        if ((output2 == 0) || (allowMoveAndEat == 1)) { // if we did not eat or we allow moving and eating in the same world update
            switch (output1) {
            case 0: //nothing
                break;
            case 1: //turn left
                facing = (facing - 1) & 7;
                break;
            case 2: //turn right
                facing = (facing + 1) & 7;
                break;
            case 3: //move forward
                if (grid[loopMod((currentColumn + xm[facing]), (worldColumns))][loopMod((currentRow + ym[facing]), (worldRows))] != WALL) { // if the proposed move is not a wall
                    if (grid[currentColumn][currentRow] == EMPTY) { // if the current location is empty
                        grid[currentColumn][currentRow] = Random::getInt(1, foodSourceTypes); // plant a red or blue food
                    }
                    currentColumn = loopMod((currentColumn + xm[facing]), worldColumns); // move to the new X
                    currentRow = loopMod((currentRow + ym[facing]), worldRows); // move to the new Y
                }
                break;
            }
        }
//		Data::Add(xp, "x"+mkString(t), org->brain->genome);
//		Data::Add(yp, "y"+mkString(t), org->brain->genome);

        /* uncommnet to print test output
         for(int x=0;x<xDim;x++){
         for(int y=0;y<yDim;y++){
         if((x==xp)&&(y==yp))
         printf("X");
         else
         printf("%i",grid[x][y]);
         }
         printf("\n");
         }
         printf("%f\n",score);
         for (int blah = 0; blah < 1000000; blah++){}
         */
    }
    if (score < 0.0) {
        score = 0.0;
    }
    if (!org->dataMap.fieldExists("foodList")) {
        org->dataMap.Append("foodList", 0);
    }
    org->dataMap.Set("switches", switches);
    int total_eaten = 0;
    for (int i = 0; i < foodSourceTypes; i++) {
        total_eaten += eaten[i];
        string temp_name = "food" + to_string(i + 1);
        org->dataMap.Set(temp_name, eaten[i]);
    }
    org->dataMap.Set("total", total_eaten);
    org->dataMap.Set("score", score);
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	if (analyse) {
//		org->dataMap[Global::update].Set("phi", Analyse::computeAtomicPhi(stateCollector, org->brain->nrOfBrainStates));
//	}
    return score;
}
