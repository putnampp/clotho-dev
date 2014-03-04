#ifndef MATURITY_EVENT_H_
#define MATURITY_EVENT_H_

#include "clotho_event.h"

extern const string MATURITY_EVENT_K;

class MaturityEvent : public ClothoEvent {
public:
    MaturityEvent( const vtime_t & tSent, const vtime_t & tRecv,
                const system_id & sender, const system_id & receiver,
                event_id_t eid );
    MaturityEvent( const vtime_t & tSent, const vtime_t & tRecv,
                const system_object * sender, const system_object * receiver,
                event_id_t eid );

    virtual const event_type_t & getEventType() const;

    virtual ~MaturityEvent() {}

protected:
    system_id   m_offspring;
};

#endif  // MATURITY_EVENT_H_
