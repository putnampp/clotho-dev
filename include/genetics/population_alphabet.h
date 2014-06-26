#ifndef POPULATION_ALPHABET_H_
#define POPULATION_ALPHABET_H_

#define BOOST_DYNAMIC_BITSET_DONT_USE_FRIENDS

#include <vector>

#include "locus.h"
#include "allele.h"

#include <boost/bimap.hpp>
#include <boost/bimap/vector_of.hpp>
#include <boost/bimap/set_of.hpp>
#include <boost/bimap/unordered_multiset_of.hpp>

#include <boost/dynamic_bitset.hpp>

#include <memory>

class PopulationAlphabet {
public:
    typedef Locus           locus_t;
    typedef allele          allele_t;
    
    typedef boost::bimaps::bimap<
        boost::bimaps::unordered_multiset_of< locus_t >,
        boost::bimaps::set_of< allele_t >,
        boost::bimaps::vector_of_relation > variant_db_t;
    typedef variant_db_t::iterator      edge_type;
    typedef std::pair< variant_db_t::left_map::iterator, variant_db_t::left_map::iterator > locus_alleles_t;

    typedef std::size_t                         index_type;

    typedef unsigned long                       block_type;
    typedef boost::dynamic_bitset< block_type > bitset_type;

    typedef std::shared_ptr< PopulationAlphabet > pointer;

    PopulationAlphabet();

    static const index_type  npos = (index_type) -1;

    static pointer getInstance() {
        static pointer instance(new PopulationAlphabet());
        return instance;
    } 

    index_type  getSymbol( const locus_t & l, const allele_t & a, bool createNew = true );
    void     updateFreeSymbols( const bitset_type & fs );
    void     resetFreeSymbols( );
    size_t   block_count();

    edge_type   operator[]( index_type idx );
    index_type  operator[]( edge_type sym );

    virtual ~PopulationAlphabet();
protected:
    variant_db_t     m_db;
    bitset_type      m_free_list, m_free_intersect, m_free_union;
};

#undef BOOST_DYNAMIC_BITSET_DONT_USE_FRIENDS

#endif  // POPULATION_ALPHABET_H_
