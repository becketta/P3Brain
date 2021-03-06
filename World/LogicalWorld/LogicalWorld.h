#pragma once
/******************************************************************************
* file: LogicalWorld.h
*
* author: Aaron Beckett
* date: 10/15/2015
******************************************************************************/

#ifndef LOGICALWORLD_H_
#define LOGICALWORLD_H_

#include "../AbstractWorld.h"


class LogicalWorld : public AbstractWorld
{
public:
    enum Logic {
        FALSE,  // 0000
        AND,    // 0001
        ANB,    // 0010
        A,      // 0011
        BNA,    // 0100
        B,      // 0101
        XOR,    // 0110
        OR,     // 0111
        NOR,    // 1000
        NXOR,   // 1001
        NB,     // 1010
        NBNA,   // 1011
        NA,     // 1100
        NANB,   // 1101
        NAND,   // 1110
        TRUE    // 1111
    };

    LogicalWorld();
    virtual ~LogicalWorld();

    virtual void runWorldSolo(shared_ptr<Organism> org, bool analyse, bool visualize, bool debug) override;

    virtual int requiredInputs() override
    {
        return 2;
    }

    virtual int requiredOutputs() override
    {
        return logic.size();
    }

    virtual int maxOrgsAllowed() override
    {
        return 1;
    }

    virtual int minOrgsAllowed() override
    {
        return 1;
    }

  private:
    vector<Logic> logic;
    double fitnessIncrement;
};

#endif /* LOGICALWORLD_H_ */
