#include "system_clock.h"

#include <string>
#include <boost/lexical_cast.hpp>

using std::string;

template <> template <>
unsigned int system_clock< unsigned int >::toVTime< string >( const string & t ) {
    unsigned int nt = boost::lexical_cast< unsigned int >( t );
    return nt;
}

template <> template <>
unsigned int system_clock< unsigned int >::toVTime< unsigned int >( const unsigned int & t ) {
    return t;
}
