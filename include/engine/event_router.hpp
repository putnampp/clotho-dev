#ifndef EVENT_ROUTER_HPP_
#define EVENT_ROUTER_HPP_

//#include "engine.h"

template < class E >
struct event_router {
    virtual void routeEvent( /*const*/ E * evt ) = 0;
    virtual void routeEvent( const typename E::receiver_t &, E * evt ) = 0;
    virtual void routeEvent( E * , const typename E::receiver_t &, const typename E::vtime_t & ) = 0;

    virtual void notifyNextEvent( const typename E::receiver_t &, const typename E::vtime_t & ) = 0;
    virtual ~event_router() {}
};

#endif  // EVENT_ROUTER_HPP_
