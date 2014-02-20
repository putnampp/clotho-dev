#ifndef GENETICREPRODUCTION_H_
#define GENETICREPRODUCTION_H_

#include "GeneticMap.h"
#include "InheritanceModel.h"
#include "RecombinationModel.h"
#include "MutationModel.h"

class GeneticReproduction : public chromosome_builder {
public:
    GeneticReproduction( GeneticMap::Ptr gm, recombination_model * recomb, inheritance_model * inherit, mutation_model * mutate );

    virtual void reproduce( AlleleGroupPtr p0, AlleleGroupPtr p1, AlleleGroupPtr offspring );

    virtual ~GeneticReproduction();
protected:

    GeneticMap::Ptr              m_genetic_map;

    shared_ptr< recombination_model > m_recomb;
    shared_ptr< inheritance_model > m_inherit;
    shared_ptr< mutation_model > m_mutate;

    AlleleGroupPtr          m_p0_tmp;
    AlleleGroupPtr          m_p1_tmp;
};

#endif  // GENETICREPRODUCTION_H_
