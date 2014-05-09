#ifndef REPRODUCTION_MODELS_HPP_
#define REPRODUCTION_MODELS_HPP_

#include "reproduction.hpp"

#include <iostream>

#include "../clotho.h"
#include "variant_map.hpp"

#include "rng/random_process.hpp"

#include <iostream>
#include <algorithm>
#include <deque>

namespace reproduction {
namespace models {
namespace mutation {

template < class V, class VM = variant_map< V >, class GM = typename VM::gamete_t >
class mutate_site : public RandomProcess {
public:
    typedef V   variant_type;
    typedef VM  variant_map_t;
    typedef GM  gamete_t;

    static void initialize( double mu = 0.0001, bool bFixed = false ) {
        m_variants.reset( new variant_map_t() );
        m_mu = mu;
        m_bFixed = bFixed;
    }

    static std::shared_ptr< variant_map_t > getVariantMap() {
        return m_variants;
    }

    static gamete_t * mutate( gamete_t * gm ) {
        return mutate( gm, m_mu, m_bFixed );
    }

    static gamete_t * mutate( gamete_t * gm, double mu, bool bFixed ) {
        if( !bFixed ) {
            return mutate_infinite(gm, mu);
        } else {
            return mutate_fixed(gm, mu);
        }
    }

protected:
    static gamete_t * mutate_infinite( gamete_t * gm, double mu ) {
        unsigned int nMut = m_rng->nextPoisson( mu );
        gamete_t * res = gm;
        if( nMut > 0) {
            res = gm->clone();
            gm->release();
            do {
                typename variant_map_t::value_ptr_t var = getNewVariant();
                res->addVariant( var );
            } while( --nMut );
        }
        return res;
    }

    static gamete_t * mutate_fixed( gamete_t * g, double mu ) {
        if( g->size() >= m_variants->size() ) return g; // no sites available for novel mutation

        unsigned int nMut = m_rng->nextPoisson( mu );   // how many mutations should occur
        gamete_t * res = g;
        if( nMut ) {

//            if( 2 * g->size() <= m_variants->size() ) {
                // if less than half of the fixed sites are mutated
                res = g->clone();
                g->release();
                while( nMut-- ) {
                    unsigned int idx = m_rng->nextInt( m_variants->size() );
                    while( (*res)[(*m_variants)[idx] ] ) {
                        // random site already mutated. check another
                        idx = m_rng->nextInt( m_variants->size() );
                    }

                    res->addVariant( (*m_variants)[idx] );   // site is unmutated. mutate it
                }
/*            } else {
                // attempt to improve performance by reducing
                // the number of random numbers necessary to generate
                // mutations
                //
                // more than half the sites have been mutated.
                //
                // randomly guessing a site index to mutate will
                // fail half the time.
                //
                // determine the unmutated sites. Linear scan 
                // is potentially costly
                std::vector< variant_map_t::value_ptr_t > unmutated;
                for( typename variant_map_t::value_list_t::const_iterator it = m_variants->begin(); it != m_variants->end(); it++ ) {
                    if(! (*g)[ *it ] ) {
                        unmutated.push_back( *it );
                    }
                }

                while( nMut-- && !unmutated.empty() ) {
                    unsigned int idx = m_rng->nextInt( unmutated.size() );
                    if( idx != unmutated.size() - 1 ) {
                        std::swap( unmutated.back(), unmutated.at(idx) );
                    }

                    g->addVariant(unmutated.back());
                    unmutated.pop_back();
                    --nMut;
                }
            }*/
        }
        return res;
    }

    static typename variant_map_t::value_ptr_t getNewVariant( ) {
        typename variant_type::key_t k;

        do {
            k = m_rng->nextUniform();
        } while( m_variants->is_known( k ) );

        typename variant_map_t::value_ptr_t vptr = m_variants->createVariant( k );

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
class no_recomb : public RandomProcess {
public:
    template < class IP, class GM >
    static GM * recombine( IP * ind, GM * ) {
        GM * g = ind->cloneGamete( m_rng->nextInt( P ) );
        return g;
    }
};

template <>
template < class IP, class GM >
GM * no_recomb< 1 >::recombine( IP * ind, GM * ) {
    return ind->cloneGamete( 0 );
}

template <>
template < class IP, class GM >
GM * no_recomb< 2 >::recombine( IP * ind, GM * ) {
    return ind->cloneGamete( (m_rng->nextBool() ? 1 : 0) );
}

class basic_recomb : public RandomProcess {
public:
    template < class IP, class GM >
    static GM * recombine( IP * ind, double r, GM * ) {
        //GM * g = ind->cloneGamete( m_rng->nextInt( P ) );
        //return g;
    }
};

}   // namespace recombination
}   // namespace models
}   // namespace reproduction

#endif  // REPRODUCTION_MODELS_HPP_
