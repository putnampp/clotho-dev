#ifndef INDIRECT_VERTEX_DECL_HPP_
#define INDIRECT_VERTEX_DECL_HPP_

#include "utility/iterator_helper.hpp"

namespace utility {
namespace graph {
namespace vertex {

struct static_value_space_tag {};

template < class ValueSpace, class Key, class Value, class KVMap, class Tag = static_value_space_tag >
class indirect_vertex {
public:
    typedef ValueSpace  value_space_type;
    typedef Key         key_type;
    typedef Value       value_type;
    typedef KVMap       map_type;

    typedef typename utility::iterator_helper< value_space_type >::type value_space_iterator;

    indirect_vertex();
    indirect_vertex( const indirect_vertex< ValueSpace, Key, Value, KVMap, Tag > & v);

    key_type    operator[]( value_type v );
    value_type  operator[]( key_type k );

    void link( key_type k );
    void unlink( key_type k );

    bool operator==( const indirect_vertex< ValueSpace, Key, Value, KVMap, Tag > & v );

    virtual ~indirect_vertex();
protected:

    void addValue( value_type v );
    void removeValue( value_type v );

    map_type                m_mapping;
    static value_space_type m_value_space;
};

}   // namespace vertex
}   // namespace graph
}   // namespace utility

#endif  // INDIRECT_VERTEX_DECL_HPP_
