#ifndef ACTIVE_SPACE_SUBSET_HPP_
#define ACTIVE_SPACE_SUBSET_HPP_

#include "active_space.hpp"
#include <cassert>

template < class ValueType >
class active_space_subset {
public:
    typedef ValueType                                           value_type;
    typedef active_space< value_type, size_t, unsigned long >   space_type;
    typedef typename space_type::bitset_type                    bitset_type;

protected:
    class set_bit_iterator {
    public:
        typedef ValueType value_type;

        set_bit_iterator( active_space_subset< ValueType > * ss, size_t p ) : m_subset( ss ), m_pos(p) { }

        set_bit_iterator( const set_bit_iterator & other ): m_subset( other.m_subset ), m_pos( other.m_pos ) {}

        set_bit_iterator & operator++() {
            if( m_pos != active_space_subset< ValueType >::space_type::npos ) {
                m_pos = m_subset->m_subset.find_next(m_pos);
            }
            return *this;
        }

        set_bit_iterator operator++(int) {
            set_bit_iterator tmp(*this);
            operator++();
            return tmp;
        }

        bool operator==( const set_bit_iterator & rhs ) {
            assert( m_subset == rhs.m_subset );
            return m_pos == rhs.m_pos;
        }

        bool operator!=( const set_bit_iterator & rhs ) {
            assert( m_subset == rhs.m_subset );
            return m_pos != rhs.m_pos;
        }

        size_t position() { return m_pos; }

        value_type operator*() { 
            assert( m_pos != active_space_subset< ValueType >::space_type::npos );
            return active_space_subset< ValueType >::m_space[ m_pos ];
        }

        virtual ~set_bit_iterator() {}
    protected:
        active_space_subset< ValueType > * m_subset;
        size_t m_pos;
    };

public:
    typedef  set_bit_iterator    iterator;

    friend class set_bit_iterator;
    active_space_subset() {}
    active_space_subset( const active_space_subset< ValueType > & other ) :
        m_subset( other.m_subset ) {
        updateReference();
    }

    void add( value_type v ) {
        size_t p = m_space.getValue( v  );

        if( m_subset.size() <= p ) { m_subset.resize( p + 1 ); }

        m_subset[p] = true;
    }

    void remove( value_type v ) {
        size_t p = m_space[ v ];
        if( p != space_type::npos ) {
            if( m_subset[ p ] ) {
                m_space.releaseValue( p );
                m_subset[ p ] = false;
            }
        }
    }

    iterator begin() { return iterator( this, m_subset.find_first() ); }
    iterator end()  { return iterator(this, -1); }

    active_space_subset< ValueType > & operator=( const active_space_subset< ValueType > & rhs ) {
        std::cerr << "Dereferencing " << m_subset.count() << " [" << m_subset.size() << "]" << std::endl;
        for( iterator it = begin(); it != end(); it++ ) {
            m_space.releaseValue( it.position() );
        }
        m_subset = rhs.m_subset;
        updateReference();
        return *this;
    }

    active_space_subset< ValueType > & operator&=( const active_space_subset< ValueType > & rhs ) {
        if( m_subset.size() > rhs.m_subset.size() ) {
            bitset_type b( rhs.m_subset);
            b.resize( m_subset.size() );
            m_subset &= b;
        } else {
            if( m_subset.size() < rhs.m_subset.size() ) {
                m_subset.resize( rhs.m_subset.size());
            }
            m_subset &= rhs.m_subset;
        }

        updateReference();
        return *this;
    }
    
    active_space_subset< ValueType > & operator|=( const active_space_subset< ValueType > & rhs ) {
        if( m_subset.size() > rhs.m_subset.size() ) {
            bitset_type b( rhs.m_subset);
            b.resize( m_subset.size() );
            m_subset |= b;
        } else {
            if( m_subset.size() < rhs.m_subset.size() ) {
                m_subset.resize( rhs.m_subset.size());
            }
            m_subset |= rhs.m_subset;
        } 

        updateReference();
        return *this;
    }

    active_space_subset< ValueType > & operator-=( const active_space_subset< ValueType > & rhs ) {
        if( m_subset.size() > rhs.m_subset.size() ) {
            bitset_type b( rhs.m_subset);
            b.resize( m_subset.size() );
            m_subset -= b;
        } else {
            if( m_subset.size() < rhs.m_subset.size() ) {
                m_subset.resize( rhs.m_subset.size());
            }
            m_subset -= rhs.m_subset;
        } 
        updateReference();
        return *this;
    }
    virtual ~active_space_subset() {
        size_t p = m_subset.find_first();
        while( p != bitset_type::npos ) {
            m_space.releaseValue( p );
            p = m_subset.find_next(p);
        }
    }
protected:

    inline void updateReference( ) {
        for( iterator it = begin(); it != end(); it++ ) {
            m_space.addReference( it.position() );
        }
    }
    bitset_type         m_subset;
    static space_type   m_space;
};

template < class V >
typename active_space_subset< V >::space_type active_space_subset< V >::m_space;

#endif  // ACTIVE_SPACE_SUBSET_HPP_
