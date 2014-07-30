#ifndef DIFFERENCE_OPERATION_HPP_
#define DIFFERENCE_OPERATION_HPP_

#include <algorithm>
#include <string>
#include <memory>

template < class SetType, class ResultType = SetType >
struct difference_operation {
    typedef SetType set_type;
    typedef ResultType result_type;

    void operator()( const set_type & s1, const set_type & s2, result_type & res ) {
        std::set_difference( s1.begin(), s1.end(), s2.begin(), s2.end(), std::back_inserter( res ) );
    }

    std::string operation_name() {
        static const std::string n("difference");
        return n;
    }
};

template < class SetType, class ResultType = SetType >
struct symmetric_difference_operation {
    typedef SetType set_type;
    typedef ResultType result_type;

    void operator()( const set_type & s1, const set_type & s2, result_type & res ) {
        std::set_symmetric_difference( s1.begin(), s1.end(), s2.begin(), s2.end(), std::back_inserter( res ) );
    }
    std::string operation_name() {
        static const std::string n("symmetric_difference");
        return n;
    }
};

#endif  // DIFFERENCE_OPERATION_HPP_
