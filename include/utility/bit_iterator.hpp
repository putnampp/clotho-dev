#ifndef BIT_ITERATOR_HPP_
#define BIT_ITERATOR_HPP_

/*
 * May be worth defining a static lookup table
 * to "walk" the set bits of a byte, rather than shift to
 * them
 */

template < class B, class O = size_t >
class BitIterator {
public:
    typedef B block_t;
    typedef O offset_t;

    BitIterator( block_t * m = NULL, size_t w = 0 ) :
        m_block( m ),
        m_mask( ((m == NULL) ? 0 : 1) ),
        m_weight( w ),
        m_seq_index(0) {
        operator++();
    }

    BitIterator(const BitIterator & m) :
        m_block( m.m_block ),
        m_mask( m.m_mask ),
        m_weight( m.m_weight ),
        m_seq_index(m.m_seq_index) {
    }

    BitIterator<B,O> & operator++() {
        if( m_block != NULL && m_weight >= 1 ) {
            while( !(*m_block & m_mask ) ) {
                ++m_seq_index;
                m_mask <<= 1;
                if( m_mask == 0 ) {
                    m_mask = 1;
                    ++m_block;
                }
            }
            if( --m_weight == 0 ) {
                m_mask = 0;
                m_block = NULL;
            }
        }

        return *this;
    }

    BitIterator< B, O> operator++( int ) {
        BitIterator<B,O> bmi(*this);
        operator++();
        return bmi;
    }

    inline bool operator==( const BitIterator & rhs ) {
        return (m_block == rhs.m_block && m_mask == rhs.m_mask);
    }

    inline bool operator!=( const BitIterator & rhs ) {
        return (m_block != rhs.m_block || m_mask != rhs.m_mask);
    }

    offset_t operator*() {
        return m_seq_index;
    }

    bool hasMore() const {
        return m_weight > 0;
    }

    virtual ~BitIterator() {}
protected:
    block_t * m_block;
    block_t m_mask;

    size_t  m_weight, m_seq_index;
};

#endif  // BIT_ITERATOR_HPP_
