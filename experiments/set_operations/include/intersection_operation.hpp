#ifndef INTERSECTION_OPERATION_HPP_
#define INTERSECTION_OPERATION_HPP_

#include <algorithm>
#include <string>
#include <memory>

template < class SetType, class ResultType = SetType >
struct intersection_operation {
    typedef SetType set_type;
    typedef ResultType result_type;

    void operator()( const set_type & s1, const set_type & s2, result_type & res ) {
        std::set_intersection( s1.begin(), s1.end(), s2.begin(), s2.end(), std::back_inserter( res ) );
    }
};

#endif  // INTERSECTION_OPERATION_HPP_
