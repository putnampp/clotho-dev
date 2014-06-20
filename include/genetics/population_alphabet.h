#ifndef POPULATION_ALPHABET_H_
#define POPULATION_ALPHABET_H_

#include <vector>

#include "variant_db.h"
#include "locus.h"
#include "utility/bit_vector.h"

class PopulationAlphabet {
public:
    typedef locus i         locus_t;
    typedef locus::allele_t allele_t;
    typedef VariantDB< locus_t, allele_t > variant_db_t;

    typedef typename variant_db_t::symbol_t symbol_t;

    typedef std::vector< symbol_t > population_alphabet_t;

    PopulationAlphabet();

    symbol_t operator[]( size_t idx );

    allele_t * getAllele( size_t idx );
    locus_t *  getLocus( size_t idx );

    size_t  indexOf( symbol_t );

    virtual ~PopulationAlphabet();
protected:
    population_alphabet_t   m_alphabet;

    bit_vector  m_free_list;

    static variant_db_t     m_db;
};

#include "utility/collection_accessor.hpp"

template<>
struct collection_accessor< PopulationAlphabet, size_t, PopulationAlphabet::allele_t * > {
    typedef PopulationAlphabet collection_t;
    typedef PopulationAlphabet::symbol_t key_t;
    typedef PopulationAlphabet::allele_t * result_t;

    inline result_t operator()( const collection_t & c, const key_t k ) {
        return c.getAllele(k);
    }
};

template<>
struct collection_accessor< PopulationAlphabet, size_t, PopulationAlphabet::locus_t * > {
    typedef PopulationAlphabet collection_t;
    typedef PopulationAlphabet::symbol_t key_t;
    typedef PopulationAlphabet::locus_t * result_t;

    inline result_t operator()( const collection_t & c, const key_t k ) {
        return c.getLocus(k);
    }
};

#endif  // POPULATION_ALPHABET_H_
