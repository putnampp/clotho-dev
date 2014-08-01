#ifndef CHROMOSOMALINHERITANCE_H_
#define CHROMOSOMALINHERITANCE_H_

#include "GeneticMap.h"
#include "InheritanceModel.h"

template < class RNG, unsigned char ALL  >
class ChromosomalInheritance : public inheritance_model {
public:
    ChromosomalInheritance( GeneticMap::Ptr gm, RNG * rng ) :
        m_genetic_map( gm ),
        m_rng( rng ) {
    }

    bool hasGeneticMap() const {
        return (bool) m_genetic_map;
    }

    void setGeneticMap( GeneticMap::Ptr gm ) {
        m_genetic_map = gm;
    }

    void inherit( AlleleGroupPtr p0, AlleleGroupPtr p1, AlleleGroupPtr offspring ) {
        assert(false);
    }

    virtual ~ChromosomalInheritance() {}
protected:
    enum { ALLELES = ALL };

    GeneticMap::Ptr m_genetic_map;
    RNG *   m_rng;
};

#endif  // CHROMOSOMALINHERITANCE_H_
