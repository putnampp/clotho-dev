#include "system_clock.h"

#include <string>
#include <boost/lexical_cast.hpp>

using std::string;

template<>
const system_clock< unsigned int >::vtime_t system_clock< unsigned int >::POSITIVE_INFINITY = -1;

template<>
const system_clock< unsigned int >::vtime_t system_clock< unsigned int >::ZERO = 0;

template <> template <>
system_clock< unsigned int >::vtime_t system_clock< unsigned int >::toVTime< string >( const string & t ) {
    unsigned int nt = boost::lexical_cast< unsigned int >( t );
    return nt;
}

template <> template <>
system_clock< unsigned int >::vtime_t system_clock< unsigned int >::toVTime< unsigned int >( const unsigned int & t ) {
    return t;
}
