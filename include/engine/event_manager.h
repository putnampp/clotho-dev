#ifndef EVENT_MANAGER_H_
#define EVENT_MANAGER_H_

#include "event_interface.h"

struct event_manager {
    virtual bool  insertEvent( const event * ) = 0;

    virtual const event * getEvent( const system_id & ) = 0;
    virtual const event * peekEvent( const system_id & ) const = 0;

    virtual size_t pendingEventCount( const system_id & ) const = 0;
    virtual size_t processedEventCount( const system_id & ) const = 0;

    virtual ~event_manager() {}
};

#endif  // EVENT_MANAGER_H_
