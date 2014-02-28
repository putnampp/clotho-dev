#ifndef EVENT_FUNCTOR_H_
#define EVENT_FUNCTOR_H_

#include "event_interface.h"

struct ltsf_event_order {
    bool operator()( const event * lhs, const event * rhs ) const {
        // event operator<( const event * ) const is defined;
        return *lhs < rhs;
    }
};

#endif  // EVENT_FUNCTOR_H_
