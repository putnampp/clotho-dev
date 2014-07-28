#ifndef ACTIVESPACESUBSET_H_
#define ACTIVESPACESUBSET_H_

#include "ActiveSpace.h"

class ActiveSpaceSubset {
public:
    typedef ActiveSpace                         space_type;
    typedef typename space_type::value_type     value_type;
    typedef typename space_type::bitset_type    bitset_type;

protected:
    class set_bit_iterator {
    public:
        typedef typename ActiveSpace::value_type value_type;

        set_bit_iterator( ActiveSpaceSubset< ValueType > * ss, size_t p ) : m_subset( ss ), m_pos(p) { }

        set_bit_iterator( const set_bit_iterator & other ): m_subset( other.m_subset ), m_pos( other.m_pos ) {}

        set_bit_iterator & operator++() {
            if( m_pos != ActiveSpaceSubset< ValueType >::space_type::npos ) {
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

        size_t position() {
            return m_pos;
        }

        value_type operator*() {
            assert( m_pos != ActiveSpaceSubset< ValueType >::space_type::npos );
            return ActiveSpaceSubset< ValueType >::m_space[ m_pos ];
        }

        virtual ~set_bit_iterator() {}
    protected:
        ActiveSpaceSubset< ValueType > * m_subset;
        size_t m_pos;
    };

public:
    typedef  set_bit_iterator    iterator;

    friend class set_bit_iterator;
    ActiveSpaceSubset() {}
    ActiveSpaceSubset( const ActiveSpaceSubset< ValueType > & other ) :
        m_subset( other.m_subset ) {
//        updateReference();
    }

    void add( value_type v ) {
        size_t p = m_space.getValue( v  );

        if( m_subset.size() <= p ) {
            m_subset.resize( p + 1 );
        }

        m_subset[p] = true;
    }

    void remove( value_type v ) {
        size_t p = m_space[ v ];
        if( p != space_type::npos ) {
            if( m_subset[ p ] ) {
//                m_space.releaseValue( p );
                m_subset[ p ] = false;
            }
        }
    }

    iterator begin() {
        return iterator( this, m_subset.find_first() );
    }
    iterator end()  {
        return iterator(this, -1);
    }

    ActiveSpaceSubset< ValueType > & operator=( const ActiveSpaceSubset< ValueType > & rhs ) {
//        std::cerr << "Dereferencing " << m_subset.count() << " [" << m_subset.size() << "]" << std::endl;
//        for( iterator it = begin(); it != end(); it++ ) {
//            m_space.releaseValue( it.position() );
//        }
        m_subset = rhs.m_subset;
//        updateReference();
        return *this;
    }

    ActiveSpaceSubset< ValueType > & operator&=( const ActiveSpaceSubset< ValueType > & rhs ) {
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

//        updateReference();
        return *this;
    }

    ActiveSpaceSubset< ValueType > & operator|=( const ActiveSpaceSubset< ValueType > & rhs ) {
        if( m_subset.size() > rhs.m_subset.size() ) {
            bitset_type b( rhs.m_subset);
            b.resize( m_subset.size() );
            m_subset |= b;
        } else {
            if( m_subset.size() < rhs.m_subset.size() ) {
                m_subset.resize( rhs.m_subset.size());
            }
            assert( m_subset.size() != 0 && m_subset.size() == rhs.m_subset.size() );
            m_subset |= rhs.m_subset;
        }

//        updateReference();
        return *this;
    }

    ActiveSpaceSubset< ValueType > & operator-=( const ActiveSpaceSubset< ValueType > & rhs ) {
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
//        updateReference();
        return *this;
    }
    virtual ~ActiveSpaceSubset() {
//        size_t p = m_subset.find_first();
//        while( p != bitset_type::npos ) {
//            m_space.releaseValue( p );
//            p = m_subset.find_next(p);
//        }
    }
protected:

//    inline void updateReference( ) {
//        for( iterator it = begin(); it != end(); it++ ) {
//            m_space.addReference( it.position() );
//        }
//    }
    bitset_type         m_subset;
    static space_type   m_space;
};

#endif  // ACTIVESPACESUBSET_H_
