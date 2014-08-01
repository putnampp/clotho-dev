#ifndef VARIANT_DB_H_
#define VARIANT_DB_H_

#include <set>
#include <vector>
#include <unordered_map>

#include <boost/bimap.hpp>
#include <boost/bimap/set_of.hpp>
#include <boost/bimap/multiset_of.hpp>
#include <boost/bimap/list_of.hpp>

template < typename L, typename A,
           typename LH = std::hash< L >,
           typename LE = std::equal_to< L >
           >
class VariantDB {
public:
    typedef L   locus_t;
    typedef A   allele_t;

//    typedef std::set< allele_t * >      allele_set_t;
//    typedef std::unordered_map< typename locus_t::key_t, locus_t * > loci_t;

//    typedef std::pair< locus_t *, allele_t * >  alphabet_key_t;
//    typedef std::vector< alphabet_key_t >       alphabet_t;
//    typedef typename alphabet_t::size_type      symbol_t;
//
//
    typedef boost::bimaps::bimap <
    boost::bimaps::unordered_multiset_of< locus_t, LH, LE >,
          boost::bimaps::multiset_of< allele_t >,
          boost::bimaps::list_of_relation > locus_allele_map;

    typedef locus_allele_map::value_type     locus_allele_t;
    typedef locus_allele_map::const_iterator symbol_t;

    VariantDB();

    void addAlternateAllele( locus_t * l, allele_t * a );
    void addAlternateAllele( typename locus_t::key_t k, allele_t * a );

    locus_t *   getLocus( typename locus_t::key_t k );

    symbol_t getOrCreateSymbol( locus_t * l, allele_t * a );

    template < class R >
    R get( symbol_t s );

    template <>
    locus_t * get< locus_t * >( symbol_t s ) {
        return m_alphabet[s].first;
    }

    template <>
    allele_t * get< allele_t * >( symbol_t s ) {
        return m_alphabet[s].second;
    }

    template <>
    allele_key_t get< allele_key_t >( symbol_t s ) {
        return m_alphabet[s];
    }

    size_t  alphabet_size() const;

    virtual ~VariantDB();
protected:
    loci_t          m_loci;
    allele_set_t    m_alleles;

    alphabet_t      m_alphabet;
};

#endif  // VARIANT_DB_H_
