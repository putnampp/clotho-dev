#ifndef INHERIT_EVENT_H_
#define INHERIT_EVENT_H_

#include "../clotho_event.h"
#include "gamete.h"

class InheritEvent : public ClothoEvent {
public:
    static const event_type_t TYPE_ID = 3;

    InheritEvent( const vtime_t & tSent, const vtime_t & tRecv,
                const system_id & sender, const system_id & receiver,
                event_id_t eid, Sex parent, gamete * z );
    InheritEvent( const vtime_t & tSent, const vtime_t & tRecv,
                const system_object * sender, const system_object * receiver,
                event_id_t eid, Sex parent, gamete * z );

    inline event_type_t getEventType() const { return TYPE_ID; }

    Sex getParentSex() const;
    gamete * getGamete() const;

    virtual ~InheritEvent() {}
protected:
    Sex         m_parent_sex;
    gamete *    m_gamete;
};

#endif  // INHERIT_EVENT_H_
