#ifndef MATE_EVENT_H_
#define MATE_EVENT_H_

#include "../clotho_event.h"

extern const string MATE_EVENT_K;

class MateEvent : public ClothoEvent {
public:
    MateEvent( const vtime_t & tSent, const vtime_t & tRecv,
                const system_id & sender, const system_id & receiver,
                event_id_t eid, const system_id & offspring );
    MateEvent( const vtime_t & tSent, const vtime_t & tRecv,
                const system_object * sender, const system_object * receiver,
                event_id_t eid, const system_object * offspring );

    virtual const event_type_t & getEventType() const;

    virtual const system_id & getOffspringID() const;

    virtual ~MateEvent() {}

protected:
    system_id   m_offspring;
};

#endif  // MATE_EVENT_H_
