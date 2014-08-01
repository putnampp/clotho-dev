#ifndef LOCUS_ADJACENCY_H_
#define LOCUS_ADJACENCY_H_

#include "population_alphabet.h"
#include "utility/bit_vector.h"

#include "mask_iterator.h"

class locus_adjacency : public bit_vector {
public:
    typedef PopulationAlphabet  alphabet_t;

    typedef MaskIterator< bit_vector::iterator, alphabet_t,
            size_t, alphabet_t::allele_t * > allele_iterator;
    typedef MaskIterator< bit_vector::iterator, alphabet_t,
            size_t, alphabet_t::locus_t * > locus_iterator;
    typedef MaskIterator< bit_vector::iterator, alphabet_t,
            size_t, alphabet_t::alphabet_key_t > locus_allele_iterator;

    typedef locus_adjacency * pointer;

    locus_adjacency( alphabet_t * alpha );
    locus_adjacency( const locus_adjacency & la );

    pointer copy();
    pointer clone();

    allele_iterator allele_begin();
    allele_iterator allele_end();

    locus_iterator locus_begin();
    locus_iterator locus_end();

    locus_allele_iterator locus_allele_begin();
    locus_allele_iterator locus_allele_end();

    void    release();

    virtual ~locus_adjacency();
protected:
    size_t      m_copies;
    alphabet_t * m_alphabet;
};

#endif  // LOCUS_ADJACENCY_H_
