#ifndef RANDOMCHROMOSOMEBUILDER_H_
#define RANDOMCHROMOSOMEBUILDER_H_

#include "ChromosomeBuilder.h"

class RandomChromosomeBuilder : virtual public ChromosomeBuilder {
public:
    RandomChromosomeBuilder( GeneticMap::Ptr gm );

    template < class RNG >
    void build( AlleleGroupPtr src, AlleleGroupPtr dest, RNG * rng );

    virtual ~RandomChromosomeBuilder() {}
protected:
};

#endif  // RANDOMCHROMOSOMEBUILDER_H_
