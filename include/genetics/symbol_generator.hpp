#ifndef SYMBOL_GENERATOR_HPP_
#define SYMBOL_GENERATOR_HPP_

template < typename L, typename A, typename S, typename ALPHA >
class symbol_generator {
public:
    typedef L locus_type;
    typedef A allele_type;
    typedef S symbol_type;
    typedef ALPHA alphabet_type;
    typedef typename ALPHA::pointer alphabet_pointer;

    template < typename M >
    symbol_type operator()( alphabet_pointer , M * ) {
        return alphabet_type::npos;
    }
};

#endif  // SYMBOL_GENERATOR_HPP_
