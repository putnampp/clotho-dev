#ifndef VARIANT_MAP_HPP_
#define VARIANT_MAP_HPP_

#include <type_traits>
#include <memory>
#include <unordered_map>
#include <set>
#include <vector>

#include <cmath>
#include <limits>

#include "variant_base.h"
#include "rng/rng.hpp"
#include "functors.h"

#include "gamete.hpp"

#include "engine/object_manager.2.hpp"

template < class VT, class E = void >
class variant_map;

template < class VT >
class variant_map< VT, typename std::enable_if< std::is_base_of< variant_base, VT >::value >::type > {
public:
    typedef VT value_t;
    typedef typename VT::key_t key_t;    // assuming for the moment that key_t is double
    typedef std::shared_ptr< value_t >   value_ptr_t;

    typedef std::unordered_map< key_t, size_t, std::hash< key_t >, equal_eps< key_t > > lookup_map_t;
    typedef std::vector< value_ptr_t >   value_list_t;

    typedef typename value_list_t::const_iterator const_iterator;

    struct vt_comparator {
        bool operator()( const value_ptr_t l, const value_ptr_t r ) const {
            return (( l != r ) && ( *l < *r ));
        }
    };

    typedef std::set< value_ptr_t, vt_comparator > variant_set_t;
    typedef gamete< variant_set_t >     gamete_t;

    variant_map( /*std::shared_ptr< iRNG > r*/ ) /*:
        m_rng(r)*/
    {}

    value_ptr_t createVariant( key_t k, bool bReplace = false ) {
        value_ptr_t nVar( new value_t( k, 0.0, 0.0, NEUTRAL ) );
        return addVariant( nVar, bReplace );
    }
    /*
     *  By default variant_map assumes an infite site model for every
     *  variant type (VT).  Thus we randomly generate a key value from
     *  the uniform distribution that does not exist within the current
     *  pool of variants.
     *
     */
    /*    value_ptr_t getNewVariant() {
            key_t   k = 0.0;
            do {
                k = m_rng->nextUniform();
            } while( is_known(k) );

            value_ptr_t v = createNewVariant( k );
            m_variants_lookup.insert(std::make_pair( k, v));

            return v;
        }*/

    value_ptr_t addVariant( value_ptr_t v, bool bReplace ) {
        typename lookup_map_t::iterator it = m_variants_lookup.find( v->getKey() );
        if( it == m_variants_lookup.end() ) {
            m_variants_lookup.insert( std::make_pair( v->getKey(), m_variants.size() ) );
            m_variants.push_back( v );
        } else if( bReplace ) {
            m_variants[it->second].reset();
            m_variants[it->second] = v;
        } else {
            v.reset();  // delete duplicate
            v = m_variants[it->second];
        }
        return v;
    }

    bool is_known( key_t k ) const {
        return m_variants_lookup.find( k ) != m_variants_lookup.end();
    }

    value_ptr_t operator[]( unsigned int relative_index ) {
        assert( relative_index < m_variants.size() );
        return m_variants[ relative_index ];
    }

    value_ptr_t getVariantByKey( key_t k ) {
        typename lookup_map_t::iterator it = m_variants_lookup.find( k );
        if( it == m_variants_lookup.end() ) {
            return value_ptr_t();
        }

        return m_variants[ it->second ];
    }

    size_t size() const {
        return m_variants.size();
    }

    const_iterator begin() const {
        return m_variants.begin();
    }

    const_iterator end() const {
        return m_variants.end();
    }

    virtual ~variant_map() {
        m_variants_lookup.clear();
        m_variants.clear();
    }

protected:
    lookup_map_t    m_variants_lookup;
    value_list_t    m_variants;
};

#endif  // VARIANT_MAP_HPP_
