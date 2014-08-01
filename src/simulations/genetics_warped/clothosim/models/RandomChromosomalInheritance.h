#ifndef RANDOMCHROMOSOMALINHERITANCE_H_
#define RANDOMCHROMOSOMALINHERITANCE_H_

#include "ChromosomalInheritance.h"
#include "random.h"
#include <cstring>

template <  >
class ChromosomalInheritance<RAND, 2> : public inheritance_model {
public:
    ChromosomalInheritance( GeneticMap::Ptr gm, RAND * rng ) :
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
        assert( m_genetic_map );
        size_t prev_offset = 0;
        for(GeneticMap::chrom_offset_map_t::const_iterator it = m_genetic_map->begin_offsets(); it != m_genetic_map->end_offsets(); it++ ) {
            // copy from p0
            double rnd = m_rng->Uniform();

            memcpy( offspring[0] + prev_offset, p0[(rnd < 0.5)] + prev_offset, (it->second - prev_offset) * sizeof( allele_t ) );
            memcpy( offspring[1] + prev_offset, p1[((0.25<= rnd ) &&(rnd < 0.75))] + prev_offset, (it->second - prev_offset) * sizeof( allele_t ) );
            prev_offset = it->second;
        }
    }

    virtual ~ChromosomalInheritance() {}
protected:
    enum { ALLELES = 2 };

    GeneticMap::Ptr m_genetic_map;
    RAND *   m_rng;
};

#endif  // RANDOMCHROMOSOMALINHERITANCE_H_
