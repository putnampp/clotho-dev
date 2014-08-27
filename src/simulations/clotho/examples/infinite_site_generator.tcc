#ifndef INFINITE_SITE_GENERATOR_TCC_
#define INFINITE_SITE_GENERATOR_TCC_

#include "genetics/locus_bitset.h"

struct infinite_site {};

#define SYM_SPEC symbol_generator< locus_bitset::alphabet_t::locus_t, locus_bitset::alphabet_t::allele_t, locus_bitset::alphabet_t::index_type, locus_bitset::alphabet_t >

template <> template <>
SYM_SPEC::symbol_type SYM_SPEC::operator()< infinite_site >( alphabet_pointer alpha, infinite_site * inf) {
    static locus_generator< alphabet_type::locus_t, RandomProcess::rng_pointer> lgen;
    static allele_generator< alphabet_type::allele_t, void > agen;

    SYM_SPEC::symbol_type res = alpha->getSymbol(lgen(), agen(), true );
    while( res == alphabet_type::npos ) {
        res = alpha->getSymbol(lgen(), agen(), true );
    }
    return res;
}
#undef SYM_SPEC

#endif  // INFINITE_SITE_GENERATOR_TCC_
