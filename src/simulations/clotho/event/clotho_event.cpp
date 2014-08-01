#include "clotho_event.h"

std::ostream & operator<<( std::ostream & o, const ClothoEvent & ce ) {
    o << "Sent: {" << ce.getSender() << ", " << ce.getSent()
      << "}; Received: {" << ce.getReceiver() << ", " << ce.getReceived()
      << "}; Event ID: " << ce.getEventID()
      << "; Type: " << ce.getEventType();
    return o;
}
