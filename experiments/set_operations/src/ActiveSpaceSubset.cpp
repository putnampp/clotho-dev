#include "ActiveSpaceSubset.h"

// static
typename ActiveSpaceSubset::space_type ActiveSpaceSubset::m_space;

size_t ActiveSpaceSubset::enumerated_space_size() {
    return m_space.size();
}

ActiveSpaceSubset::ActiveSpaceSubset() : m_subset() {}

ActiveSpaceSubset::ActiveSpaceSubset( const ActiveSpaceSubset & other ) :
    m_subset( other.m_subset ) {
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
        m_subset[ p ] = false;
    }
}

ActiveSpaceSubset::iterator ActiveSpaceSubset::begin() {
    return iterator( &m_subset );
}

ActiveSpaceSubset::iterator ActiveSpaceSubset::end()  {
    return iterator( &m_subset, bitset_type::npos );
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
        //assert( m_subset.size() != 0 && m_subset.size() == rhs.m_subset.size() );
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

ActiveSpaceSubset & ActiveSpaceSubset::operator^=( const ActiveSpaceSubset & rhs ) {
    if( m_subset.size() > rhs.m_subset.size() ) {
        bitset_type b( rhs.m_subset);
        b.resize( m_subset.size() );
        m_subset ^= b;
    } else {
        if( m_subset.size() < rhs.m_subset.size() ) {
            m_subset.resize( rhs.m_subset.size());
        }
        m_subset ^= rhs.m_subset;
    }
    return *this;
}

ActiveSpaceSubset ActiveSpaceSubset::operator|( const ActiveSpaceSubset & rhs ) const {
    ActiveSpaceSubset tmp( *this );
    return tmp |= rhs;
}

ActiveSpaceSubset ActiveSpaceSubset::operator&( const ActiveSpaceSubset & rhs ) const {
    ActiveSpaceSubset tmp( *this );
    return tmp &= rhs;
}

ActiveSpaceSubset ActiveSpaceSubset::operator-( const ActiveSpaceSubset & rhs ) const {
    ActiveSpaceSubset tmp( *this );
    return tmp -= rhs;
}

ActiveSpaceSubset ActiveSpaceSubset::operator^( const ActiveSpaceSubset & rhs ) const {
    ActiveSpaceSubset tmp( *this );
    return tmp -= rhs;
}

size_t ActiveSpaceSubset::count() const {
    return m_subset.count();
}

size_t ActiveSpaceSubset::num_blocks() const {
    return m_subset.num_blocks();
}

ActiveSpaceSubset::~ActiveSpaceSubset() { }
