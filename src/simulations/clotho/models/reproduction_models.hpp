#ifndef REPRODUCTION_MODELS_HPP_
#define REPRODUCTION_MODELS_HPP_

#include "reproduction.hpp"

#include <iostream>

#include "../clotho.h"
#include "gamete.h"

#include "rng/random_process.hpp"

#include <iostream>
#include <algorithm>
#include <deque>
#include <vector>

namespace reproduction {
namespace models {
namespace mutation {

template < class V, class VM, class GM >
class mutate_site : public RandomProcess {
public:
    typedef V   variant_type;
    typedef VM  variant_map_t;
    typedef GM  gamete_t;
    typedef typename GM::pointer gamete_ptr;

    static void initialize( double mu = 0.0001, bool bFixed = false ) {
        m_variants.reset( new variant_map_t() );
        m_mu = mu;
        m_bFixed = bFixed;
    }

    static std::shared_ptr< variant_map_t > getVariantMap() {
        return m_variants;
    }

    static gamete_ptr mutate( gamete_ptr gm ) {
        return mutate( gm, m_mu, m_bFixed );
    }

    static gamete_ptr mutate( gamete_ptr gm, double mu, bool bFixed ) {
        if( !bFixed ) {
            return mutate_infinite(gm, mu);
        } else {
            return mutate_fixed(gm, mu);
        }
    }

protected:
    static gamete_ptr mutate_infinite( gamete_ptr gm, double mu ) {
        unsigned int nMut = m_rng->nextPoisson( mu );
        if( nMut > 0) {
            gamete_ptr res = gm->clone();
            
            do {
                typename variant_map_t::value_ptr_t var = getNewVariant();
                res->addVariant( var );
            } while( --nMut );

            return res;
        } 
        return gm->copy();
    }

    static gamete_ptr mutate_fixed( gamete_ptr g, double mu ) {
        if( g->size() >= m_variants->size() ) return g; // no sites available for novel mutation

        unsigned int nMut = m_rng->nextPoisson( mu );   // how many mutations should occur
        if( nMut ) {
            gamete_ptr res = (( g != NULL ) ? g->clone() : gamete_t::EMPTY_GAMETE.clone());
            while( nMut-- ) {
                unsigned int idx = m_rng->nextInt( m_variants->size() );
                typename variant_map_t::value_ptr_t vptr = (*m_variants)[idx];

                while( (*res)[vptr] ) {
                    // random site already mutated. check another
                    idx = m_rng->nextInt( m_variants->size() );
                    vptr = (*m_variants)[idx];
                }

                res->addVariant( vptr );   // site is unmutated. mutate it
            }
            return res;
        }
        return g->copy();
    }

    static typename variant_map_t::value_ptr_t getNewVariant( ) {
//        typename variant_type::key_t k;
//
//        do {
//            k = m_rng->nextUniform();
//        } while( m_variants->is_known( k ) );
//
//        typename variant_map_t::value_ptr_t vptr = m_variants->createVariant( k );

        typename variant_map_t::value_ptr_t vptr = m_variants->createNewVariant();
        return vptr;
    }

    static std::shared_ptr< variant_map_t > m_variants;
    static double m_mu;
    static bool m_bFixed;
};

template < class V, class VM, class GM >
std::shared_ptr< VM > mutate_site< V, VM, GM >::m_variants;

template < class V, class VM, class GM >
double mutate_site< V, VM, GM >::m_mu = 0.0;

template < class V, class VM, class GM >
bool mutate_site< V, VM, GM>::m_bFixed = false;

}   // namespace mutation

namespace recombination {

template < unsigned char P >
class no_recomb;

template < class IP, class GM >
class basic_recomb;

}   // namespace recombination
}   // namespace models
}   // namespace reproduction

#endif  // REPRODUCTION_MODELS_HPP_

#include "no_recombination.tcc"
#include "basic_recombination.tcc"
