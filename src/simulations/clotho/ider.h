#ifndef IDER_H_
#define IDER_H_

#include <string>

template < class I >
struct IDer {
    typedef std::string id_type_t;

    id_type_t operator()( const I & i ) const;
    id_type_t operator()( const I * i ) const;
};

#endif  // IDER_H_
