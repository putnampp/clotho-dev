#ifndef EVENT_ROUTER_HPP_
#define EVENT_ROUTER_HPP_

#include "engine.h"

template < class E >
struct event_router {
    virtual void routeEvent( /*const*/ E * evt ) = 0;
    virtual void routeEvent( const system_id &, E * evt ) = 0;
    virtual void notifyNextEvent( const system_id &, const SystemClock::vtime_t & ) = 0;
    virtual ~event_router() {}
};

#endif  // EVENT_ROUTER_HPP_
