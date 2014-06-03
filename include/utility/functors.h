#ifndef FUNCTORS_H_
#define FUNCTORS_H_

#include <limits>
#include <cmath>

template < class T >
struct equal_eps {
    bool operator()( const T & l, const T & r ) const {
        return comp(l, r, std::is_floating_point<T>() );
    }

    bool comp( const T & l, const T & r, std::true_type ) const {
        return (std::fabs( l - r ) <= std::numeric_limits<T>::epsilon());
    }

    bool comp( const T & l, const T & r, std::false_type ) const {
        return l == r;
    }
};

template < class V >
struct less_than_order {
    bool operator()( const V & l, const V & r ) const {
        return comp( l, r, std::is_pointer< V >() );
    }

    bool comp( const V & l, const V & r, std::false_type ) {
        return l < r;
    }

    bool comp( const V & l, const V & r, std::true_type ) {
        return (( l != r ) && (*l < *r));
    }
};

#endif  // FUNCTORS_H_
