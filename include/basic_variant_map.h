#ifndef BASIC_VARIANT_MAP_H_
#define BASIC_VARIANT_MAP_H_

#include <type_traits>
#include <memory>
#include <unordered_map>
#include <set>

#include <cmath>
#include <limits>

#include "variant_base.h"
#include "rng/rng.hpp"
//#include "functors.h"

class basic_variant_map {
public:
    typedef variant_base value_t;
    typedef value_t::key_t key_t;    // assuming, for the moment, that key_t is double
    typedef value_t *   value_ptr_t;

 //   typedef std::unordered_map< key_t, value_ptr_t, std::hash< key_t >, equal_eps< key_t > > lookup_map_t;
    typedef std::unordered_map< key_t, value_ptr_t > lookup_map_t;

    typedef typename lookup_map_t::iterator iterator;

    struct vt_comparator {
        bool operator()( const value_ptr_t l, const value_ptr_t r ) const {
            return (( l != r ) && ( *l < *r ));
        }
    };

    typedef std::set< value_ptr_t, vt_comparator > variant_set_t;

    basic_variant_map( ) {}

/*
 *  By default variant_map assumes an infite site model for every
 *  variant type (VT).  Thus we randomly generate a key value from
 *  the uniform distribution that does not exist within the current
 *  pool of variants.
 */
    value_ptr_t createNewVariant() {
        key_t   k = RandomProcess::getRNG()->nextUniform();
        while( is_known(k) ) {
            k = RandomProcess::getRNG()->nextUniform();
        }

        value_ptr_t nVar = new value_t( k, 0.0, 0.0, NEUTRAL );

        m_variants.insert(std::make_pair( k, nVar));

        return nVar;
    }

    bool is_known( key_t k ) const {
        return m_variants.find( k ) != m_variants.end();
    }

    value_ptr_t operator[]( size_t s ) const {
        if( s >= m_variants.size() ) return NULL;

        return m_variants.at(s);
    }

    size_t size() const {
        return m_variants.size();
    }

    iterator begin() {
        return m_variants.begin();
    }

    iterator end() {
        return m_variants.end();
    }

    virtual ~basic_variant_map() {
        m_variants.clear();
    }
protected:
    lookup_map_t    m_variants;
};

#endif  // BASIC_VARIANT_MAP_H_
