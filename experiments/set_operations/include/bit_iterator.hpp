#ifndef BIT_ITERATOR_HPP_
#define BIT_ITERATOR_HPP_

template < class B >
class bit_iterator;

#include <boost/dynamic_bitset.hpp>
template < class B, class BA >
class bit_iterator< boost::dynamic_bitset< B, BA > > {
public:
    typedef boost::dynamic_bitset< B, BA > bitset_type;

    bit_iterator( bitset_type * bits ) : m_pdata( bits ), m_pos( bits->find_first() ) { }
    bit_iterator( bitset_type * bits, size_t p ) : m_pdata( bits ), m_pos( p ) { }
    bit_iterator( const bit_iterator< bitset_type > & other ): m_pdata( other.m_pdata ), m_pos( other.m_pos ) {}

    bit_iterator< bitset_type >  & operator++() {
        if( m_pos != bitset_type::npos ) {
            m_pos = m_pdata->find_next(m_pos);
        }
        return *this;
    }

    bit_iterator< bitset_type > operator++(int) {
        bit_iterator< bitset_type >  tmp(*this);
        operator++();
        return tmp;
    }

    bool operator==( const bit_iterator< bitset_type > & rhs ) {
        assert( m_pdata == rhs.m_pdata );
        return m_pos == rhs.m_pos;
    }

    bool operator!=( const bit_iterator< bitset_type > & rhs ) {
        assert( m_pdata == rhs.m_pdata );
        return m_pos != rhs.m_pos;
    }

    size_t operator*() {
        assert( m_pos != bitset_type::npos );
        return m_pos;
    }

    virtual ~bit_iterator() {}
protected:
    bitset_type * m_pdata;
    size_t m_pos;
};

#endif  // BIT_ITERATOR_HPP_
