#ifndef LOCUS_BITSET_H_
#define LOCUS_BITSET_H_

#include "population_alphabet.h"

#include <boost/pool/object_pool.hpp>

class locus_bitset {
public:
    typedef PopulationAlphabet      alphabet_t;
    typedef alphabet_t::locus_t     locus_type;
    typedef alphabet_t::allele_t    allele_type;
    typedef alphabet_t::bitset_type bitset_type;
    typedef alphabet_t::index_type  index_type;
    typedef alphabet_t::edge_type   edge_type;

    typedef locus_bitset *          pointer;

    typedef boost::object_pool< locus_bitset > pool_type;

//    typedef EdgeIterator< bitset_type, alphabet_t > iterator;

    locus_bitset( alphabet_t::pointer alpha = alphabet_t::getInstance() );
    locus_bitset( const locus_bitset & la );

    pointer copy();
    pointer clone();

    void addVariant( const locus_type & l, const allele_type & a );
    void removeVariant( const locus_type & l, const allele_type & a );

    size_t size();

//    iterator begin();
//    iterator end();

    static void * operator new( size_t );

    void    release();

    virtual ~locus_bitset();
protected:
    bitset_type             m_bits;
    size_t                  m_copies;
    alphabet_t::pointer     m_alphabet;

    static pool_type m_pool;
};

#endif  // LOCUS_BITSET_H_
