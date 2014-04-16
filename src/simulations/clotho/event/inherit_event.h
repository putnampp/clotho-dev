#ifndef INHERIT_EVENT_H_
#define INHERIT_EVENT_H_

#include "clotho_event.h"
#include "gamete.h"

extern const string INHERIT_EVENT_K;

class InheritEvent : public ClothoEvent {
public:
    InheritEvent( const vtime_t & tSent, const vtime_t & tRecv,
                const system_id & sender, const system_id & receiver,
                event_id_t eid, Sex parent, gamete * z );
    InheritEvent( const vtime_t & tSent, const vtime_t & tRecv,
                const system_object * sender, const system_object * receiver,
                event_id_t eid, Sex parent, gamete * z );

    inline const event_type_t & getEventType() const {
        return INHERIT_EVENT_K;
    }

    Sex getParentSex() const;
    gamete * getGamete() const;

    virtual ~InheritEvent() {}
protected:
    Sex         m_parent_sex;
    gamete *    m_gamete;
};

#endif  // INHERIT_EVENT_H_
