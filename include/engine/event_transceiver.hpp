#ifndef EVENT_TRANSCEIVER_HPP_
#define EVENT_TRANSCEIVER_HPP_

template < class E >
struct event_transceiver {
    virtual void sendEvent( /*const*/ E * ) = 0;
    virtual void sendEvent( E *, const typename E::receiver_t & rt,  const typename E::vtime_t & ) = 0;
    virtual bool receiveEvent( /*const*/ E * ) = 0;
    virtual bool receiveEvent( E *, const typename E::vtime_t & ) = 0;

    virtual ~event_transceiver() {}
};

#endif  // EVENT_TRANSCEIVER_HPP_
