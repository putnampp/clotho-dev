#ifndef BIRTH_EVENT_H_
#define BIRTH_EVENT_H_

#include "clotho_event.h"
//#include "engine/pager.hpp"

class BirthEvent : public ClothoEvent {
public:
    typedef ObjectManager< BirthEvent, EVENT_PAGE_SIZE > object_manager_t;

//    static const event_type_t TYPE_ID = 1;
    CLOTHO_EVENT_ID(1)

    void init( const vtime_t & tSent, const vtime_t & tRecv,
                const system_id & sender, const system_id & receiver,
                event_id_t eid, Sex s = UNKNOWN );
    void init( const vtime_t & tSent, const vtime_t & tRecv,
                const system_object * sender, const system_object * receiver,
                event_id_t eid, Sex s = UNKNOWN );

    //inline event_type_t getEventType() const { return CEID; }

    Sex getSex() const;

    static void * operator new( size_t s ) {
        void * res = m_pool.malloc();
        return res;
    }

    static void operator delete( void * ptr ) {
        m_pool.free( (BirthEvent * ) ptr );
    }


    virtual ~BirthEvent() {}

    BirthEvent() {}

    BirthEvent( const vtime_t & tSent, const vtime_t & tRecv,
                const system_id & sender, const system_id & receiver,
                event_id_t eid, Sex s = UNKNOWN );
    BirthEvent( const vtime_t & tSent, const vtime_t & tRecv,
                const system_object * sender, const system_object * receiver,
                event_id_t eid, Sex s = UNKNOWN );
protected:
    Sex m_sex;

    static object_manager_t m_pool;
};

#endif  // BIRTH_EVENT_H_
