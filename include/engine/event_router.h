#ifndef EVENT_ROUTER_H_
#define EVENT_ROUTER_H_

#include "event_interface.h"

struct event_router {
    virtual void routeEvent( const event * evt ) = 0;
    virtual ~event_router() {}
};

#endif  // EVENT_ROUTER_H_
