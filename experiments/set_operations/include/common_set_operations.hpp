#ifndef COMMON_SET_OPERATIONS_HPP_
#define COMMON_SET_OPERATIONS_HPP_

namespace set_operations {

template < class S, class SIterator >
struct set_op {
    typedef S set_type;
    typedef SIterator set_iterator;

    template < class ResultType, class OP >
    ResultType operator()( set_type S, OP
};

}

#endif  // COMMON_SET_OPERATIONS_HPP_
