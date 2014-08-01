#ifndef FIXED_LENGTH_GAMETE_HPP_
#define FIXED_LENGTH_GAMETE_HPP_

#include "gamete_base.h"

#include "variant_map.hpp"
#include "variant_base.h"

//#include <vector>
#include <memory>
#include <cstring>

template < class A = unsigned char, class VM = variant_map< variant_base > >
class fixed_gamete : public gamete_base {
public:
    typedef A                           allele_t;
    typedef VM  variant_map_t;

    fixed_gamete() :
        gamete_base(),
        m_alleles( new allele_t[ m_variants->size() ] ),
        m_nMutated( m_variants->size() ) {
        memset( m_alleles, 0, sizeof( allele_t ) * m_variants->size() );
    }

    fixed_gamete(gamete_source_t s, gamete_type_t t) :
        gamete_base(s,t),
        m_alleles( new allele_t[ m_variants->size() ] ),
        m_nMutated( m_variants->size() ) {
        memset( m_alleles, 0, sizeof( allele_t ) * m_variants->size() );
    }

    fixed_gamete( const fixed_gamete< allele_t, variant_map_t > & c ) :
        gamete_base( c.m_id ),
        m_alleles( new allele_t[ m_variants->size() ] ),
        m_nMutated( c.m_nMutated ) {
        memcpy( m_alleles, c.m_alleles, sizeof( allele_t ) * m_variants->size());
    }

    static void setVariantMap( std::shared_ptr< variant_map_t > vm ) {
        m_variants = vm;
    }

    static std::shared_ptr< variant_map_t > getVariantMap() {
        return m_variants;
    }

    static void addVariant( typename variant_map_t::value_ptr_t v ) {
        m_variants->addVariant( v );
    }

    static typename variant_map_t::value_ptr_t getVariant( unsigned int relative_index ) {
        return (*m_variants)[ relative_index ];
    }

    static typename variant_map_t::value_ptr_t getVariantByKey( typename variant_map_t::key_t k  ) {
        return m_variants->getVariantByKey( k );
    }

    fixed_gamete< allele_t, variant_map_t > * clone() {
        fixed_gamete< allele_t, variant_map_t > * t = new fixed_gamete<allele_t, variant_map_t>( *this );
        return t;
    }

    allele_t   operator[]( unsigned int idx ) const {
        assert( idx < m_variants->size() );
        return m_alleles[ idx ];
    }

    void increasePenetrance() {
        allele_t * tmp = m_alleles;
        const unsigned int size = m_variants->size();
        for( unsigned int i = 0; i < size; ++i ) {
            if( *tmp++ != 0 ) {
                (*m_variants)[ i ]->incrementPenetrance();
            }
        }
    }

    void decreasePenetrance() {
        allele_t * tmp = m_alleles;
        const unsigned int size = m_variants->size();
        for( unsigned int i = 0; i < size; ++i ) {
            if( *tmp++ != 0 ) {
                (*m_variants)[ i ]->decrementPenetrance();
            }
        }
    }

//    void addVariant( unsigned int allele_idx, allele_t val = (allele_t)1 ) {
//        assert( allele_idx < m_variants->size() );
//        m_alleles[allele_idx] = val;
//        ++m_nMutated;
//    }

    size_t size() const {
        return m_nMutated;
    }

    virtual ~fixed_gamete() {
        delete [] m_alleles;
    }
protected:
    allele_t *              m_alleles;
    size_t                  m_nMutated;
    static std::shared_ptr< variant_map_t >    m_variants;
};

template < class A, class VM >
std::shared_ptr< typename fixed_gamete< A, VM >::variant_map_t > fixed_gamete< A, VM >::m_variants;

#endif  // FIXED_LENGTH_GAMETE_HPP_
