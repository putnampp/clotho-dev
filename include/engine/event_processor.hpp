#ifndef EVENT_PROCESSOR_HPP_
#define EVENT_PROCESSOR_HPP_

struct event_processor {
    virtual void process() = 0;

    virtual ~event_processor() {}
};

#endif  // EVENT_PROCESSOR_HPP_
