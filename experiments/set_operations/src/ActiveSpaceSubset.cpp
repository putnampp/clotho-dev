#include "ActiveSpaceSubset.h"

// static
typename ActiveSpaceSubset::space_type ActiveSpaceSubset::m_space;

ActiveSpaceSubset::ActiveSpaceSubset() {}
ActiveSpaceSubset::ActiveSpaceSubset( const ActiveSpaceSubset< ValueType > & other ) :
    m_subset( other.m_subset ) {
//        updateReference();
}

void ActiveSpaceSubset::add( value_type v ) {
    size_t p = m_space.getValue( v  );

    if( m_subset.size() <= p ) {
        m_subset.resize( p + 1 );
    }

    m_subset[p] = true;
}

void ActiveSpaceSubset::remove( value_type v ) {
    size_t p = m_space[ v ];
    if( p != space_type::npos ) {
        if( m_subset[ p ] ) {
//                m_space.releaseValue( p );
            m_subset[ p ] = false;
        }
    }
}

ActiveSpaceSubset::iterator ActiveSpaceSubset::begin() {
    return iterator( this, m_subset.find_first() );
}
ActiveSpaceSubset::iterator ActiveSpaceSubset::end()  {
    return iterator(this, -1);
}

ActiveSpaceSubset & ActiveSpaceSubset::operator=( const ActiveSpaceSubset & rhs ) {
    m_subset = rhs.m_subset;
    return *this;
}

ActiveSpaceSubset & ActiveSpaceSubset::operator&=( const ActiveSpaceSubset & rhs ) {
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

    return *this;
}

ActiveSpaceSubset & ActiveSpaceSubset::operator|=( const ActiveSpaceSubset & rhs ) {
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

    return *this;
}

ActiveSpaceSubset & ActiveSpaceSubset::operator-=( const ActiveSpaceSubset & rhs ) {
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
    return *this;
}

ActiveSpaceSubset::~ActiveSpaceSubset() { }
