#ifndef MASK_ITERATOR_H_
#define MASK_ITERATOR_H_

#include "collection_accessor.hpp"

template < class I, class A, class K, class V, class OP = collection_accessor< A, K, V >  >
class MaskIterator {
public:
    typedef I bit_iter_t;
    typedef A alphabet_t;
    typedef V value_t;

    MaskIterator( const bit_iter_t & it , alphabet_t * s = NULL ) :
        m_iter( it ),
        m_set( s )
    { }

    MaskIterator(const MaskIterator<I,A,V> & m) :
        m_iter( m.m_iter ),
        m_set( m.m_set )
    {}

    MaskIterator<I,A,V> & operator++() {
        if( hasMore() ) {
            ++m_iter;
        } 
        return *this;
    }

    MaskIterator<I,A,V> operator++( int ) {
        MaskIterator<I, A, V> bmi(*this);
        operator++();
        return bmi;
    }

    bool operator==( const MaskIterator<I,A,V> & rhs ) {
        return (m_set == rhs.m_set && m_iter == rhs.m_iter );
    }

    bool operator!=( const MaskIterator<I,A,V> & rhs ) {
        return (m_set != rhs.m_set || m_iter != rhs.m_iter );
    }
    
    value_t operator*() {
        return oper(*m_set, *m_iter);
    }

    bool hasMore() const {
        return m_iter.hasMore();
    }

    virtual ~MaskIterator() {}
protected:
    bit_iter_t  m_iter;
    alphabet_t* m_set;

    static OP oper;
};

#endif  // MASK_ITERATOR_H_
