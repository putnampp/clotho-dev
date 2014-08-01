#ifndef BITSET_OPS_HPP_
#define BITSET_OPS_HPP_

#include <boost/dynamic_bitset.hpp>
#include <type_traits>

template < typename B, typename A, typename OP >
inline void combine( boost::dynamic_bitset< B, A > & res,
                     const boost::dynamic_bitset< B, A > & rhs, OP oper ) {

    size_t i = 0;
    while( i < res.m_bits.size() && i < rhs.m_bits.size() ) {
        res.m_bits[i] = oper( res.m_bits[i], rhs.m_bits[i] );
        ++i;
    }

    while( i < rhs.m_bits.size() ) {
        res.append( oper(0, rhs.m_bits[i++]) );
    }

    while( i < res.m_bits.size() ) {
        res.m_bits[i] = oper( res.m_bits[i], 0 );
        ++i;
    }
}

template < typename B, class E = void >
struct block_operator;

template < typename B >
struct block_operator< B, typename std::enable_if< std::is_integral<B>::value >::type > {
    typedef B block_type;
    typedef B result_type;
};

template < typename B >
struct block_intersect : public block_operator< B > {
    typedef typename block_operator<B>::block_type block_type;
    typedef typename block_operator<B>::result_type result_type;

    inline result_type operator()( block_type a, block_type b ) {
        return a & b;
    }
};

template < typename B >
struct block_union : public block_operator< B > {
    typedef typename block_operator<B>::block_type block_type;
    typedef typename block_operator<B>::result_type result_type;

    inline result_type operator()( block_type a, block_type b ) {
        return a | b;
    }
};

template < typename B >
struct block_difference : public block_operator< B > {
    typedef typename block_operator<B>::block_type block_type;
    typedef typename block_operator<B>::result_type result_type;

    inline result_type operator()( block_type a, block_type b ) {
        return a ^ b;
    }
};

#endif  // BITSET_OPS_HPP_
