/*
 * Placement.h
 *
 *  Created on: Jan 15, 2024
 *      Author: mahdi
 */

#ifndef Placement_H_
#define Placement_H_


#include <iostream>
#include <vector>
#include <cmath>
#include <stdio.h>  
#include <stdlib.h>
#include <time.h>   
#include <iterator>
#include <vector>
#include <algorithm>

#include "pm.h"
#include "vm.h"
#include "mapping.h"

struct vmPlacement
{
    unsigned int id;
    vm vmInfo; // to access its resource values
};

class Placement
{
private:
public:
    Placement();
    virtual ~Placement();
    Placement(unsigned int ID);
    unsigned int PlacementID;
    std::vector <vmPlacement> structure;
    float position;
    unsigned int P;  // number of Protons of the Gene
    unsigned int N;  // number of Neutrons of the Gene
    float fit;       // fitness of the Gene
    float StabLevel; // SL_i=(fi-fbest)/(fworst-fbest)
    void calculateFitPlacement(vector<pm> PMList, vector<vm> VMList);
    float calculatePowerPlacement(vector<pm> PMList, vector<vm> VMList);
    mapping fitPlacementSequenceUsingFF(vector<pm> PMList, vector<vm> VMList);

    vector<vm> createVMListPlacement(vector<vm> VMList); // to re-create VM list from the Chromosome's structure
    vector<vmPlacement> findMissingVMIndicesHho(Placement theChromosomeWithAllVMs);
    void putNullValueOnRepeatingVMIndicesHho(unsigned int alphaIdx2);
    void insertVMListRandomIntoBubllesHho(vector<vmPlacement> missingVMs, unsigned int m); // into those positions containing #VM (m)
    void insertVMListRandomHho(vector<vmPlacement> missingVMs);                            // at random places
    void insertVMListRandomEndHho(vector<vmPlacement> missingVMs);
    void checkValidityPlacement(); // check the structure to see if all VMs are present and no repeated index exists. Print an error message if not
    void printPlacementStructure();
    void putBubbleInIndicesWithDifferentCPUHho(Placement Chromosome1, Placement Chromosome2);       // check CPU values of each index and put void (m or #VMs) in those indexes that has more than 10% difference
    vector<vmPlacement> shiftRightPlacementStructureHho(unsigned int numShift, Placement theChromosome); // shit out and remove exiting VMs
    void rotateRightPlacementStructureHho(unsigned int numRotate, Placement theChromosome);         // rotate right and put exiting VMs at the beginning position
};





#endif /* Placement_H_ */
