#ifndef INHERIT_EVENT_HPP_
#define INHERIT_EVENT_HPP_

#include "../clotho_event.h"

static const string INHERIT_EVENT_K = "inherit_event";

template < class GM >
class InheritEvent : public ClothoEvent {
public:
    typedef GM gamete_t;

    // by default gamete index is unknown (-1)
    // gamete index can be used to identify which "source" ("parent")
    // a gamete was inherited from (ie. source gender)
    InheritEvent( const vtime_t & tSent, const vtime_t & tRecv,
                const system_object * sender, const system_object * receiver,
                event_id_t eid, gamete_t * z, unsigned char gidx = -1) :
        ClothoEvent( tSent, tRecv, sender, receiver, eid ),
        m_gidx( gidx ),
        m_gamete( z )
    {}

    InheritEvent( const vtime_t & tSent, const vtime_t & tRecv,
                const system_id & sender, const system_id & receiver,
                event_id_t eid, gamete_t * z, unsigned char gidx = -1) :
        ClothoEvent( tSent, tRecv, sender, receiver, eid ),
        m_gidx( gidx ),
        m_gamete( z )
    {}
    inline const event_type_t & getEventType() const {
        return INHERIT_EVENT_K;
    }

    unsigned char getParentIndex() const { return m_gidx; }
    gamete_t * getGamete() const { return m_gamete; }

    virtual ~InheritEvent() {}
protected:
    unsigned char   m_gidx;
    gamete_t *      m_gamete;
};

#endif  // INHERIT_EVENT_HPP_
