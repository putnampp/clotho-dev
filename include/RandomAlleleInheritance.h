#ifndef RANDOMALLELEINHERITANCE_H_
#define RANDOMALLELEINHERITANCE_H_

#include "AlleleTuple.h"
#include "InheritanceModel.h"
#include <cassert>

template < class RNG, unsigned char ALL >
class RandomAlleleInheritance : public inheritance_model {
public:
    RandomAlleleInheritance( GeneticMap::Ptr gm, RNG * rng ) : 
        m_nLoci( gm->getLociCount() ),
        m_rng( rng )
    {}

    void inherit( AlleleGroupPtr p0, AlleleGroupPtr p1, AlleleGroupPtr offspring ) {
        assert( false );
    }

    virtual ~RandomAlleleInheritance() { }
protected:
    enum { ALLELES = ALL };

    unsigned int m_nLoci;
    RNG * m_rng;
};

#endif  // RANDOMALLELEINHERITANCE_H_
