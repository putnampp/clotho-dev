#ifndef EVENT_PERFORMER_HPP_
#define EVENT_PERFORMER_HPP_

template < class E >
struct event_performer {
    virtual void perform_event( /*const*/ E * ) = 0;
    virtual ~event_performer() {}
};

#endif  // EVENT_PERFORMER_HPP_
