#ifndef POPULATION_ALPHABET_H_
#define POPULATION_ALPHABET_H_

#include <vector>

#include "locus.h"
#include "allele.h"
#include "utility/bit_vector.h"

#include <boost/bimap.hpp>
#include <boost/dynamic_bitset.hpp>

class PopulationAlphabet {
public:
    typedef Locus           locus_t;
    typedef allele          allele_t;
//    typedef VariantDB< locus_t, allele_t > variant_db_t;
    
    typedef boost::bimaps::bimap<
        boost::bimaps::unordered_multiset_of< locus_t >,
        boost::bimaps::set_of< allele_t >,
        boost::bimaps::vector_of_relation > variant_db_t;
    typedef variant_db_t::value_type edge_type;
    typedef std::pair< variant_db_t::left_map::iterator, variant_db_t::left_map::iterator > locus_alleles_t;

    typedef std::size_t                         index_t;

    typedef unsigned long                       block_type;
    typedef boost::dynamic_bitset< block_type > bitset_type;

    PopulationAlphabet();

    static index_t addSymbol( const locus_t & l, const allele_t & a );
    static void    updateFreeSymbols( const bitset_type & bs );

    symbol_t operator[]( size_t idx );

    allele_t * getAllele( size_t idx );
    locus_t *  getLocus( size_t idx );

    size_t  indexOf( symbol_t );

    virtual ~PopulationAlphabet();
protected:
    static variant_db_t     m_db;
    static bitset_type      m_free_list;
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
