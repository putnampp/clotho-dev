#ifndef EVENT_MANAGER_H_
#define EVENT_MANAGER_H_

#include "event_interface.h"

struct event_manager {
    virtual void  insertEvent( const event * ) = 0;

    virtual const event * getEvent() = 0;
    virtual const event * peekEvent() const = 0;

    virtual size_t pendingEventCount() const = 0;
    virtual size_t processedEventCount() const = 0;

    virtual ~event_manager() {}
};

#endif  // EVENT_MANAGER_H_
