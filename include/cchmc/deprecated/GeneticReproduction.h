#ifndef GENETICREPRODUCTION_H_
#define GENETICREPRODUCTION_H_
#include "reproduction.h"
#include "GeneticMap.h"
#include "RecombinationModel.h"
#include "MutationModel.h"

class GeneticReproduction : public reproduction {
public:
    GeneticReproduction( GeneticMap::Ptr gm, recombination_model * recomb, mutation_model * mutate );

    virtual void reproduce( AlleleGroupPtr p0, AlleleGroupPtr offspring );

    virtual ~GeneticReproduction();
protected:

    GeneticMap::Ptr              m_genetic_map;

    shared_ptr< recombination_model > m_recomb;
    shared_ptr< mutation_model > m_mutate;

    AlleleGroupPtr          m_p0_tmp;
};

#endif  // GENETICREPRODUCTION_H_
