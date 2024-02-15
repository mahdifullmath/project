/*
 * Chromosome.h
 *
 *  Created on: Oct 2, 2023
 *      Author: mkiani
 */

#ifndef Chromosome_H_
#define Chromosome_H_

#include <iostream>
#include <vector>
#include <cmath>
#include <stdio.h>  /* printf, scanf, puts, NULL */
#include <stdlib.h> /* srand, rand */
#include <time.h>   /* time */
#include <iterator>
#include <vector>
#include <algorithm>

#include "pm.h"
#include "vm.h"
#include "mapping.h"

struct vmGene
{
    unsigned int id;
    vm vmInfo; // to access its resource values
};

class Chromosome
{
private:
public:
    Chromosome();
    virtual ~Chromosome();
    Chromosome(unsigned int ID);
    unsigned int ChromosomeID;
    std::vector<vmGene> structure;
    float position;
    unsigned int P;  // number of Protons of the Gene
    unsigned int N;  // number of Neutrons of the Gene
    float fit;       // fitness of the Gene
    float StabLevel; // SL_i=(fi-fbest)/(fworst-fbest)
    void calculateFitChromosome(vector<pm> PMList, vector<vm> VMList);
    float calculatePowerChromosome(vector<pm> PMList, vector<vm> VMList);
    mapping fitChromosomeSequenceUsingFF(vector<pm> PMList, vector<vm> VMList);

    vector<vm> createVMListChromosome(vector<vm> VMList); // to re-create VM list from the Chromosome's structure
    vector<vmGene> findMissingVMIndicesGa(Chromosome theChromosomeWithAllVMs);
    void putNullValueOnRepeatingVMIndicesGa(unsigned int alphaIdx2);
    void insertVMListRandomIntoBubllesGa(vector<vmGene> missingVMs, unsigned int m); // into those positions containing #VM (m)
    void insertVMListRandomGa(vector<vmGene> missingVMs);                            // at random places
    void insertVMListRandomEndGa(vector<vmGene> missingVMs);
    void checkValidityChromosome(); // check the structure to see if all VMs are present and no repeated index exists. Print an error message if not
    void printChromosomeStructure();
    void putBubbleInIndicesWithDifferentCPUGa(Chromosome Chromosome1, Chromosome Chromosome2);       // check CPU values of each index and put void (m or #VMs) in those indexes that has more than 10% difference
    vector<vmGene> shiftRightChromosomeStructureGa(unsigned int numShift, Chromosome theChromosome); // shit out and remove exiting VMs
    void rotateRightChromosomeStructureGa(unsigned int numRotate, Chromosome theChromosome);         // rotate right and put exiting VMs at the beginning position
};

#endif /* Chromosome_H_ */
