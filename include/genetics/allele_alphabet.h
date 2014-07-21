#ifndef ALLELE_ALPHABET_H_
#define ALLELE_ALPHABET_H_

#define BOOST_DYNAMIC_BITSET_DONT_USE_FRIENDS

#include <vector>
#include <map>
#include <set>

#include "locus.h"
#include "allele.h"

#include <boost/dynamic_bitset.hpp>

#include <memory>

#include "edge_iterator.hpp"
#include "symbol_generator.hpp"

class AlleleAlphabet {
public:
    typedef Locus           locus_t;
    typedef allele          allele_t;

//    typedef std::pair< locus_t, allele_t > symbol_type;
    
    // using set and multimap (log(size) lookup) b/c unordered version
    // will invalid iterators upon rehash (resize)
    typedef std::size_t                         index_type;
    typedef std::set< allele_t >                alleles_type;
    typedef alleles_type::iterator              allele_iterator;

    typedef std::pair< locus_t, std::pair< allele_iterator, index_type> >       symbol_type;
    typedef std::pair< locus_t, std::pair< allele_iterator, index_type> >       edge_type;
    typedef std::multimap< locus_t, std::pair< allele_iterator, index_type> >   variant_db_t;
    typedef variant_db_t::iterator                                              locus_iterator;
    typedef variant_db_t::const_iterator                                        clocus_iterator;
    typedef std::vector< locus_iterator >                                            active_database_type;

    typedef unsigned long                       block_type;
    typedef boost::dynamic_bitset< block_type > bitset_type;
    typedef bitset_type                         edge_set_type;

    typedef EdgeIterator< variant_db_t, edge_set_type > edge_iterator;

    typedef std::shared_ptr< AlleleAlphabet >   pointer;

    friend class symbol_generator< locus_t, allele_t, index_type, AlleleAlphabet >;

    AlleleAlphabet();

    static const index_type  npos = bitset_type::npos;

    static pointer getInstance() {
        static pointer instance(new AlleleAlphabet());
        return instance;
    } 

    index_type  getSymbol( const locus_t & l, const allele_t & a, bool createNew = true );
    void     updateFreeSymbols( const bitset_type & fs );
    void     resetFreeSymbols( );
    size_t   block_count();

    size_t  size();
    size_t  active_size();

    bool isLocus( locus_t & l ) const;

    locus_iterator   operator[]( index_type idx );
    inline index_type operator[]( const locus_iterator & sym ) {
        return sym->second.second;
    }

    edge_iterator begin( edge_set_type * es );
    edge_iterator end( edge_set_type * es );

    virtual ~AlleleAlphabet();
protected:
    alleles_type    m_alleles;
    variant_db_t    m_db;
 
    active_database_type    m_active;
  
    bitset_type     m_free_list, m_free_intersect, m_free_union;
};

#undef BOOST_DYNAMIC_BITSET_DONT_USE_FRIENDS

#endif  // ALLELE_ALPHABET_H_
