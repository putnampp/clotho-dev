#ifndef ACTIVESPACE_H_
#define ACTIVESPACE_H_

#include <boost/dynamic_bitset.hpp>
#include <map>
#include <vector>

//
// Chose to implement "space" as a map of reference counted values
// instead of a multimap of values because the multimap approach
// will enumerate every instance of a value resulting in
// duplicate instances of the same value.  This consequently increases the space
// needed to represent a subset.  Furthermore, it complicates the set operations
//
//
class ActiveSpace {
public:
    typedef double value_type;
    typedef size_t indirect_type;
    typedef std::map< value_type, indirect_type >       map_type;
    typedef std::vector< typename map_type::iterator >  indexed_map_type;
    typedef boost::dynamic_bitset<unsigned long>        bitset_type;

    static const indirect_type npos = -1;

    ActiveSpace();

    indirect_type   getValue( value_type v, bool bCreate = true );

    value_type      operator[]( indirect_type ind );
    indirect_type operator[]( value_type v );

    virtual ~ActiveSpace();
protected:
    map_type            m_active;
    indexed_map_type    m_indexed;
    bitset_type         m_free_space;
};

#endif  // ACTIVESPACE_H_
