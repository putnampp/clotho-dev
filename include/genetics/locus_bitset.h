#ifndef LOCUS_BITSET_H_
#define LOCUS_BITSET_H_

#include "population_alphabet.h"
#include "edge_iterator.hpp"

#include <type_traits>
#include <boost/pool/object_pool.hpp>

#include <set>

class locus_bitset {
public:
    typedef PopulationAlphabet      alphabet_t;
    typedef alphabet_t::locus_t     locus_type;
    typedef alphabet_t::allele_t    allele_type;
    typedef alphabet_t::bitset_type bitset_type;
    typedef alphabet_t::index_type  index_type;
    typedef alphabet_t::edge_type   edge_type;

    typedef locus_bitset *  pointer;

    typedef boost::object_pool< locus_bitset >      pool_type;

    typedef alphabet_t::edge_iterator edge_iterator;

    typedef std::set< locus_bitset * > active_bitsets;
    typedef typename active_bitsets::iterator active_iterator;

    static locus_bitset EMPTY;

    locus_bitset( alphabet_t::pointer alpha = alphabet_t::getInstance() );
    locus_bitset( const locus_bitset & la );

    pointer copy();
    pointer clone() const;

    void addVariant( const locus_type & l, const allele_type & a );
    void removeVariant( const locus_type & l, const allele_type & a );

    void addVariant( index_type idx );
    void removeVariant( index_type idx );

    bool operator[]( index_type idx );

    size_t size();

    edge_iterator begin();
    edge_iterator end();

    void updateSymbols();

    static void * operator new( size_t );
    void    release();

    static active_iterator active_begin();
    static active_iterator active_end();

    virtual ~locus_bitset();
protected:
    bitset_type             m_bits;
    mutable size_t          m_copies;
    alphabet_t::pointer     m_alphabet;

    static pool_type m_pool;

    static active_bitsets m_active;
};

#endif  // LOCUS_BITSET_H_
