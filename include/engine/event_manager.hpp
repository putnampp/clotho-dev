#ifndef EVENT_MANAGER_HPP_
#define EVENT_MANAGER_HPP_

#include "event_interface.h"

template < class EVT = event >
struct event_manager {
    typedef EVT event_t;

    virtual bool  insertEvent( const EVT * ) = 0;

    virtual void reset_pending() = 0;
    virtual void reset_processed() = 0;

    virtual const EVT * getEvent( const system_id & ) = 0;
    virtual const EVT * peekEvent( const system_id & ) const = 0;

    virtual size_t  pendingEventCount( const system_id & ) const = 0;
    virtual size_t  processedEventCount( const system_id & ) const = 0;
    virtual size_t  canceledEventCount( const system_id & ) const = 0;

    virtual ~event_manager() {}
};

#endif  // EVENT_MANAGER_HPP_
