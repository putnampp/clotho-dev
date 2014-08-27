#ifndef FINAL_INDIVIDUAL_PROPS_TCC_
#define FINAL_INDIVIDUAL_PROPS_TCC_

#include <utility>
#include "object/individual_properties.hpp"
#include "genetics/locus_bitset.h"

namespace finalizer {

template < unsigned char P, class S >
class FinalWorker< individual_props< locus_bitset, P, S > > {
public:
    static void finalize( individual_props< locus_bitset, P, S > * props ) {
        for( unsigned char i = 0; i < P; ++i ) {
            if( props->m_gametes[i].second != NULL ) {
                props->m_gametes[i].second->release();
                props->m_gametes[i].second = NULL;
            }
        }
        props->m_free_gametes = P;
    }
};

}

#endif  // FINAL_INDIVIDUAL_PROPS_TCC_
