#ifndef ACTIVESPACESUBSET_H_
#define ACTIVESPACESUBSET_H_

#include "ActiveSpace.h"
#include "bit_iterator.hpp"

class ActiveSpaceSubset {
public:
    typedef ActiveSpace                         space_type;
    typedef typename space_type::value_type     value_type;
    typedef typename space_type::bitset_type    bitset_type;

    typedef  bit_iterator< bitset_type >    iterator;

    ActiveSpaceSubset();
    ActiveSpaceSubset( const ActiveSpaceSubset & other );

    void add( value_type v );
    void remove( value_type v );

    iterator begin();
    iterator end();

    ActiveSpaceSubset & operator=( const ActiveSpaceSubset & rhs );

    ActiveSpaceSubset & operator&=( const ActiveSpaceSubset & rhs );
    ActiveSpaceSubset & operator|=( const ActiveSpaceSubset & rhs );
    ActiveSpaceSubset & operator-=( const ActiveSpaceSubset & rhs );
    ActiveSpaceSubset & operator^=( const ActiveSpaceSubset & rhs );

    ActiveSpaceSubset operator|( const ActiveSpaceSubset & rhs ) const;
    ActiveSpaceSubset operator&( const ActiveSpaceSubset & rhs ) const;
    ActiveSpaceSubset operator-( const ActiveSpaceSubset & rhs ) const;
    ActiveSpaceSubset operator^( const ActiveSpaceSubset & rhs ) const;

    virtual ~ActiveSpaceSubset();

protected:
    bitset_type         m_subset;
    static space_type   m_space;
};

#endif  // ACTIVESPACESUBSET_H_
