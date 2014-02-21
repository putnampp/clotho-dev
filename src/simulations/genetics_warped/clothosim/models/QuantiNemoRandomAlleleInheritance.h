#ifndef QUANTINEMORANDOMALLELEINHERITANCE_H_
#define QUANTINEMORANDOMALLELEINHERITANCE_H_

#include "RandomAlleleInheritance.h"
#include "random.h"

#include "pword_allele_masks.hpp"

template <>
class RandomAlleleInheritance< RAND, 2 > : public inheritance_model {
public:
    RandomAlleleInheritance( RAND * rng ) :
        m_rng( rng ),
        m_pword_masks()
    {}

    void inherit( AlleleGroupPtr p0, AlleleGroupPtr p1, AlleleGroupPtr offspring ) {
        pword_t * f = reinterpret_cast< pword_t * >(p0[0]),
            * f1 = reinterpret_cast< pword_t * >(p0[1]),
            * m = reinterpret_cast< pword_t * >(p1[0]),
            * m1 = reinterpret_cast< pword_t * >(p1[1]),
            * g = reinterpret_cast< pword_t * >(offspring[0]),
            * g1 = reinterpret_cast< pword_t * >(offspring[1]);

        size_t blocks = (p0[1]-p0[0]) / ALLELES_PER_PWORD;
        while ( blocks-- ) {
            unsigned int rnd = (unsigned int) (m_rng->Uniform() * 65536 );
            register pword_t mask = m_pword_masks[ rnd % 256 ];
            (*g++) = (((*m++) & (mask)) | ((*m1++) & ~(mask)));
            mask = m_pword_masks[ rnd / 256 ];
            (*g1++) = (((*f++) & (mask)) | ((*f1++) & ~(mask)));
        }
    }

    virtual ~RandomAlleleInheritance() {}
protected:
    enum { ALLELES = 2 };

    RAND * m_rng;

    pword_allele_mask< pword_t, allele_t, 2 > m_pword_masks;
};

#endif  // QUANTINEMORANDOMALLELEINHERITANCE_H_
