#include "engine/event_interface.h"

std::ostream & operator<<( std::ostream & o, const event * e ) {
    o << (*e );
    return o;
}

std::ostream & operator<<( std::ostream & o, const event & e ) {
    o   << e.getSender() << " @ " << e.getSent()
        << " -> "
        << e.getReceiver() << " @ " << e.getReceived()
        << " [" << e.getEventID() << "]";
    return o;
}
