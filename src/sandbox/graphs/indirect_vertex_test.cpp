#include <iostream>
#include <vector>
#include <map>

#include "utility/graph/vertex.hpp"

namespace utility {
namespace graph {
namespace vertex {

template < template < class , class , class... > TT,
            class T1, class T2, class... TARGS,
           template < class, class...> TT2,
            class T3, class... TT2ARGS >
class indirect_vertex< TT< T1, T2, TARGS... >, Key, 
}   // namespace vertex
}   // namespace graph
}   // namespace utility

typedef std::multimap< int, int >   space_type;
typedef utility::graph::indirect_vertex< space_type, size_t, std::vector< typename space_type::iterator > > indexed_space_type;

int main() {
    indexed_space_type mySpace;
    indexed_space_type mySpace2;

    for( int i = 0; i < 10; ++i ) {
        indexed_space_type::value_type v(i, i);

        std::cout << mySpace.link( v ) << std::endl;
    }

    for( int i = 10; i < 20; ++i ) {
        indexed_space_type::value_type v(i, i);
        std::cout << mySpace2.link( v ) << std::endl;
    }

    return 0;
}
