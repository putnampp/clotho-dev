#ifndef INHERIT_EVENT_2_HPP_
#define INHERIT_EVENT_2_HPP_

#include "clotho_event.h"

#include "genetics/gamete.h"

class InheritEvent : public ClothoEvent {
public:

//    static const event_type_t TYPE_ID = 4;
    CLOTHO_EVENT_ID(4)

    typedef Gamete::pointer gamete_ptr_t;

    typedef ObjectManager< InheritEvent, EVENT_PAGE_SIZE > object_manager_t;

    // by default gamete index is unknown (-1)
    // gamete index can be used to identify which "source" ("parent")
    // a gamete was inherited from (ie. source gender)
    void init( const vtime_t & tSent, const vtime_t & tRecv,
               const system_object * sender, const system_object * receiver,
               event_id_t eid, gamete_ptr_t  z, unsigned char gidx = -1) {
        ClothoEvent::init( tSent, tRecv, sender, receiver, eid );
        m_gidx = gidx;
        m_gamete = z;
    }

    void init( const vtime_t & tSent, const vtime_t & tRecv,
               const system_id & sender, const system_id & receiver,
               event_id_t eid, gamete_ptr_t z, unsigned char gidx = -1) {
        ClothoEvent::init( tSent, tRecv, sender, receiver, eid );
        m_gidx = gidx;
        m_gamete = z;
    }

//    inline event_type_t getEventType() const { return TYPE_ID; }

    unsigned char getParentIndex() const {
        return m_gidx;
    }
    gamete_ptr_t getGamete() const {
        return m_gamete;
    }

    static void * operator new( size_t s ) {
        void * res = m_pool.malloc();
        return res;
    }

    static void operator delete( void * ptr ) {
        m_pool.free( (InheritEvent *) ptr );
    }

    virtual ~InheritEvent() {}
//protected:

    InheritEvent() {}

    // by default gamete index is unknown (-1)
    // gamete index can be used to identify which "source" ("parent")
    // a gamete was inherited from (ie. source gender)
    InheritEvent( const vtime_t & tSent, const vtime_t & tRecv,
                  const system_object * sender, const system_object * receiver,
                  event_id_t eid, gamete_ptr_t z, unsigned char gidx = -1) :
        ClothoEvent( tSent, tRecv, sender, receiver, eid ),
        m_gidx( gidx ),
        m_gamete( z ) {
    }

    InheritEvent( const vtime_t & tSent, const vtime_t & tRecv,
                  const system_id & sender, const system_id & receiver,
                  event_id_t eid, gamete_ptr_t z, unsigned char gidx = -1) :
        ClothoEvent( tSent, tRecv, sender, receiver, eid ),
        m_gidx( gidx ),
        m_gamete( z ) {
    }

protected:
    unsigned char   m_gidx;
    gamete_ptr_t        m_gamete;

    static object_manager_t m_pool;
};

#endif  // INHERIT_EVENT_2_HPP_
