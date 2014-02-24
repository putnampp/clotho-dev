#ifndef EVENT_PROCESSOR_H_
#define EVENT_PROCESSOR_H_

#include "event_interface.h"

struct event_processor {
    virtual void initialize() = 0;
    virtual void process() = 0;
    virtual void finalize() = 0;

    virtual ~event_processor() {}
};

#endif  // EVENT_PROCESSOR_H_
