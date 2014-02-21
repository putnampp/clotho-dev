#ifndef _EVENT_INTERFACE_H_
#define _EVENT_INTERFACE_H_

#include "system_id.h"
#include "system_clock.h"

struct event {
    typedef unsigned int            event_id_t;
    typedef SystemClock::vtime_t    vtime_t;

    virtual const event_id_t &  getEventID()    const = 0;
    virtual const system_id &   getSender()     const = 0;
    virtual const system_id &   getReceiver()   const = 0;

    virtual const vtime_t &  getSent()       const = 0;
    virtual const vtime_t &  getReceived()   const = 0;

    virtual bool operator<( const event * rhs ) const = 0;
    virtual bool operator==( const event * rhs ) const = 0;

    virtual ~event() {}
};

#endif  // _EVENT_INTERFACE_H_
