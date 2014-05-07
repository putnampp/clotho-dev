#ifndef EVENT_TRANSCEIVER_HPP_
#define EVENT_TRANSCEIVER_HPP_

template < class E >
struct event_transceiver {
    virtual void sendEvent( /*const*/ E * ) = 0;
    virtual void receiveEvent( /*const*/ E * ) = 0;

    virtual ~event_transceiver() {}
};

#endif  // EVENT_TRANSCEIVER_HPP_
