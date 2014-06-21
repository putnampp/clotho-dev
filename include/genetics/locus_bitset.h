#ifndef LOCUS_BITSET_H_
#define LOCUS_BITSET_H_

#include "population_alphabet.h"
#include "utility/bit_vector.h"

#include "mask_iterator.h"

#include <boost/dynamic_bitset.hpp>

class locus_bitset {
public:
    typedef PopulationAlphabet  alphabet_t;

    typedef MaskIterator< bit_vector::iterator, alphabet_t, 
                            size_t, alphabet_t::allele_t * > allele_iterator;
    typedef MaskIterator< bit_vector::iterator, alphabet_t, 
                            size_t, alphabet_t::locus_t * > locus_iterator;
    typedef MaskIterator< bit_vector::iterator, alphabet_t, 
                            size_t, alphabet_t::alphabet_key_t > locus_allele_iterator;

    typedef unsigned long block_type;
    typedef boost::dynamic_bitset< block_type > bitset_type;

    typedef locus_bitset * pointer;

    locus_bitset( alphabet_t * alpha );
    locus_bitset( const locus_bitset & la );

    pointer copy();
    pointer clone();

    allele_iterator allele_begin();
    allele_iterator allele_end();

    locus_iterator locus_begin();
    locus_iterator locus_end();

    locus_allele_iterator locus_allele_begin();
    locus_allele_iterator locus_allele_end();

    void    release();

    virtual ~locus_bitset();
protected:
    bitset_type     m_bits;
    size_t          m_copies;
    alphabet_t *    m_alphabet;
};

#endif  // LOCUS_BITSET_H_
