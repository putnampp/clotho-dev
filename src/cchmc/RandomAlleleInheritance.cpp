#include "RandomAlleleInheritance.h"

RandomAlleleInheritance::RandomAlleleInheritance( GeneticMap::Ptr gm,  ) :
    m_nLoci( gm->getLociCount() ) {
}

void RandomAlleleInheritance::inherit( AlleleGroupPtr p0, AlleleGroupPtr p1, AlleleGroupPtr offspring ) {
    pword_t * f = reinterpret_cast< pword_t * >(p0[0]),
              * f1 = reinterpret_cast< pword_t * >(p0[1]),
                * m = reinterpret_cast< pword_t * >(p1[0]),
                  * m1 = reinterpret_cast< pword_t * >(p1[1]),
                    * g = reinterpret_cast< pword_t * >(offspring[0]),
                      * g1 = reinterpret_cast< pword_t * >(offspring[1]);

    size_t blocks = (p0[1]-p1[0]) / ALLELES_PER_PWORD;
    while ( blocks-- ) {
        unsigned int rnd = (unsigned int) (m_rng->Uniform() * 65536 );
        register pword_t mask = m_pword_masks[ rnd % 256 ];
        (*g++) = (((*m++) & (mask)) | ((*m1++) & ~(mask)));
        mask = m_pword_masks[ rnd / 256 ];
        (*g1++) = (((*f++) & (mask)) | ((*f1++) & ~(mask)));
    }
}

RandomAlleleInheritance::~RandomAlleleInheritance() {}
