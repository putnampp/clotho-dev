#ifndef EVENT_TRANSCEIVER_H_
#define EVENT_TRANSCEIVER_H_

#include "event_interface.h"

struct event_transceiver {
    virtual void sendEvent( const event * ) = 0;
    virtual void receiveEvent( const event * ) = 0;

    virtual ~event_transceiver() {}
};

#endif  // EVENT_TRANSCEIVER_H_
