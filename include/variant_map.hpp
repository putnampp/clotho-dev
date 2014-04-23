#ifndef VARIANT_MAP_HPP_
#define VARIANT_MAP_HPP_

#include <type_traits>
#include <memory>
#include <unordered_map>
#include <set>

#include <cmath>
#include <limits>

#include "variant_base.h"
#include "rng/rng.hpp"
#include "functors.h"

#include "gamete.hpp"

template < class VT, class E = void >
class variant_map;

template < class VT >
class variant_map< VT, typename std::enable_if< std::is_base_of< variant_base, VT >::value >::type > {
public:
    typedef VT value_t;
    typedef typename VT::key_t key_t;    // assuming for the moment that key_t is double
    typedef std::shared_ptr< value_t >   value_ptr_t;

    typedef std::unordered_map< key_t, value_ptr_t, std::hash< key_t >, equal_eps< key_t > > lookup_map_t;

    struct vt_comparator {
        bool operator()( const value_ptr_t l, const value_ptr_t r ) const {
            return (( l != r ) && ( *l < *r ));
        }
    };

    typedef std::set< value_ptr_t, vt_comparator > variant_set_t;
    typedef gamete< variant_set_t >     gamete_t;

    variant_map( std::shared_ptr< iRNG > r ) :
        m_rng(r)
    {}

/*
 *  By default variant_map assumes an infite site model for every
 *  variant type (VT).  Thus we randomly generate a key value from
 *  the uniform distribution that does not exist within the current
 *  pool of variants.
 *
 */
    value_ptr_t getNewVariant() {
        key_t   k = 0.0;
        do {
            k = m_rng->nextUniform();
        } while( is_known(k) );

        value_ptr_t v = createNewVariant( k );
        m_variants.insert(std::make_pair( k, v));

        return v;
    }

    bool is_known( key_t k ) const {
        return m_variants.find( k ) != m_variants.end();
    }

    virtual ~variant_map() {
        m_variants.clear();
    }

protected:

    value_ptr_t createNewVariant( key_t k ) {
        value_ptr_t nVar( new value_t( k, 0.0, 0.0, NEUTRAL ) );
        return nVar;
    }

    std::shared_ptr< iRNG > m_rng;
    lookup_map_t    m_variants;
};

#endif  // VARIANT_MAP_HPP_
