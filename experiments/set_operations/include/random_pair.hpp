#ifndef RANDOM_PAIR_HPP_
#define RANDOM_PAIR_HPP_

#include <utility>

template < class ForwardIterator, class Generator >
std::pair< ForwardIterator, ForwardIterator> random_pair( ForwardIterator first, ForwardIterator last, Generator gen ) {
    unsigned int range = (last - first);
    unsigned int a = 0, b = 0;
    if( range != 0) {
        a = gen(0, range);
        b = gen(0, range);
    }

    return std::make_pair( first + a, first + b);
}

template < class ForwardIterator, class Size, class Result, class Generator >
void random_pair_n( ForwardIterator first, ForwardIterator last, Size n, Result res, Generator gen ) {
    if( n == 0 ) return;

    unsigned int max = (last - first);
    if( max == 0 ) return;

    while( --n ) {
        unsigned int a = gen(0, max);
        unsigned int b = gen(0, max);
        res( std::make_pair( first + a, first + b ) );
    }
}

template < class ForwardIterator, class Size, class Result, class Generator >
void random_pair_n( ForwardIterator first, ForwardIterator last, Size n, std::back_insert_iterator< Result > res, Generator gen ) {
    if( n == 0 ) return;

    unsigned int max = (last - first);
    if( max == 0 ) return;

    while( n-- ) {
        unsigned int a = gen(0, max);
        unsigned int b = gen(0, max);
        res = std::make_pair( first + a, first + b );
        ++res;
    }
}
//template < class SIterator, class Dist = boost::random::uniform_int_distribution< unsigned int > >
//class randomly_pair {
//public:
//    typedef SIterator                                       sample_iterator;
//    typedef std::pair< sample_iterator, sample_iterator >   pair_type;
//    typedef std::vector< pair_type >                        sample_pairing;
//    typedef typename sample_pairing::iterator               iterator;
//    typedef Dist                                            sampling_distribution;
//
//    randomly_pair( sample_iterator first, sample_iterator last ) : m_first(first), m_last(last) { }
//    randomly_pair( const randomly_pair< SIterator, Dist> & other ): m_first( other.m_first), m_last( other.m_last ), m_pairs( other.m_pairs ) {}
//
//    template < class URNG >
//    randomly_pair( sample_iterator first, sample_iterator last, URNG & rng, const unsigned int nPairs ) : m_first(first), m_last(last) {
//        generate( rng, nPairs );
//    }
//
//    template < class URNG >
//    void generate( URNG & rng, unsigned int nPairs = 0 ) {
//        m_pairs.clear();
//        if( nPairs == 0 ) return;
//
//        m_pairs.reserve( nPairs );
//
//        unsigned int max_offset = (m_last - m_first);
//        sampling_distribution dist(0, max_offset);
//        while( nPairs-- ) {
//            unsigned int offset1 = dist( rng );
//            unsigned int offset2 = dist( rng );
//
//            m_pairs.push_back( std::make_pair( m_first + offset1, m_first + offset2 ) );
//        }
//    }
//
//    size_t  index_of( sample_iterator it ) {
//        return it - m_first;
//    }
//
//    iterator begin()    {
//        return m_pairs.begin();
//    }
//    iterator end()      {
//        return m_pairs.end();
//    }
//
//    sample_iterator start() {
//        return m_first;
//    }
//    sample_iterator last() {
//        return m_last;
//    }
//
//    virtual ~randomly_pair() {}
//
//protected:
//    sample_iterator m_first, m_last;
//    sample_pairing  m_pairs;
//};

#endif  // RANDOM_PAIR_HPP_
