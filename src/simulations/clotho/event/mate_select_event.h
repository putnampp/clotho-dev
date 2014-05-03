#ifndef MATE_SELECT_EVENT_H_
#define MATE_SELECT_EVENT_H_

#include "../clotho_event.h"

class MateSelectEvent : public ClothoEvent {
public:
    static const event_type_t TYPE_ID = 7;

    MateSelectEvent( const vtime_t & tSent, const vtime_t & tRecv,
                const system_id & sender, const system_id & receiver,
                event_id_t eid );
    MateSelectEvent( const vtime_t & tSent, const vtime_t & tRecv,
                const system_object * sender, const system_object * receiver,
                event_id_t eid );

    inline event_type_t getEventType() const { return TYPE_ID; }

    virtual ~MateSelectEvent() {}

protected:
};

#endif  // MATE_SELECT_EVENT_H_
