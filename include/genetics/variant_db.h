#ifndef VARIANT_DB_H_
#define VARIANT_DB_H_

#include <set>
#include <vector>
#include <unordered_map>

template < class L, class A, class E = void >
class VariantDB;

template < class L, class A >
class VariantDB< L, A, void> {
public:
    typedef L   locus_t;
    typedef A   allele_t;

    typedef std::set< allele_t * >      allele_set_t;
    typedef std::unordered_map< typename locus_t::key_t, locus_t * > loci_t;

    typedef std::pair< locus_t *, allele_t * >  alphabet_key_t;
    typedef std::vector< alphabet_key_t >       alphabet_t;
    typedef typename alphabet_t::size_type      symbol_t;

    VariantDB();

    void addAlternateAllele( locus_t * l, allele_t * a );
    void addAlternateAllele( typename locus_t::key_t k, allele_t * a );

    locus_t *   getLocus( typename locus_t::key_t k );

    alphabet_key_t operator[]( symbol_t s );

    alphabet_key_t  getAlphabetKey( symbol_t s );

    locus_t *   getLocusBySymbol( symbol_t s );
    allele_t *  getAlleleBySymbol( symbol_t s );

    size_t  alphabet_size() const;

    virtual ~VariantDB();
protected:
    loci_t          m_loci;
    allele_set_t    m_alleles;

    alphabet_t      m_alphabet;
};

#endif  // VARIANT_DB_H_
