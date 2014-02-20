#ifndef PTR_TYPE_H_
#define PTR_TYPE_H_

#include <memory>

template < class O >
struct ptr_type {
    typedef std::shared_ptr< O > Ptr;
};

#endif  // PTR_TYPE_H_
