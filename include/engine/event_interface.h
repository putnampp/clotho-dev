#ifndef _EVENT_INTERFACE_H_
#define _EVENT_INTERFACE_H_

#include "engine.h"
#include <ostream>

struct event {
    typedef unsigned int            event_id_t;
    typedef SystemClock::vtime_t    vtime_t;
    typedef system_id               receiver_t;
    typedef system_id               sender_t;

    virtual const event_id_t &  getEventID()    const = 0;
    virtual const sender_t &   getSender()     const = 0;
    virtual const receiver_t &   getReceiver()   const = 0;

    virtual const vtime_t &  getSent()       const = 0;
    virtual const vtime_t &  getReceived()   const = 0;

    virtual bool operator<( const event * rhs ) const = 0;
    virtual bool operator==( const event * rhs ) const = 0;

    virtual bool isReceivedAt( const vtime_t & t ) const = 0;

    virtual ~event() {}
};

std::ostream & operator<<( std::ostream &, const event * );
std::ostream & operator<<( std::ostream &, const event & );

#endif  // _EVENT_INTERFACE_H_
