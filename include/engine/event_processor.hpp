#ifndef EVENT_PROCESSOR_HPP_
#define EVENT_PROCESSOR_HPP_

template < class EVT >
struct event_processor {
    virtual void process() = 0;

    virtual void perform_event( const EVT * ) = 0;

    virtual ~event_processor() {}
};

#endif  // EVENT_PROCESSOR_HPP_
