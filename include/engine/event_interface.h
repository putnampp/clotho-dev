#ifndef _EVENT_INTERFACE_H_
#define _EVENT_INTERFACE_H_

#include "engine.h"
#include <ostream>

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

std::ostream & operator<<( std::ostream &, const event * );
std::ostream & operator<<( std::ostream &, const event & );

#endif  // _EVENT_INTERFACE_H_
