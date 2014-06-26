#ifndef SELECTION_EVENT_H_
#define SELECTION_EVENT_H_

#include "clotho_event.h"

class SelectionEvent : public ClothoEvent {
public:
//    static const event_type_t TYPE_ID = 8;

    CLOTHO_EVENT_ID( 8 )

    typedef ObjectManager< SelectionEvent, EVENT_PAGE_SIZE > object_manager_t;

    SelectionEvent() {}

    SelectionEvent( const vtime_t & tSent, const vtime_t & tRecv,
                const system_id & sender, const system_id & receiver,
                event_id_t eid, Sex s, const system_id & pending_offspring );
    SelectionEvent( const vtime_t & tSent, const vtime_t & tRecv,
                const system_object * sender, const system_object * receiver,
                event_id_t eid, Sex s, const system_id & pending_offspring );

//    inline event_type_t getEventType() const { return TYPE_ID; }

    Sex getSex() const;
    system_id getOffspring() const;

    static void * operator new( size_t s ) {
        void * res = m_pool.malloc();
        return res;
    }

    static void operator delete( void * ptr ) {
        m_pool.free( (SelectionEvent *) ptr );
    }

    virtual ~SelectionEvent() {}

protected:
    Sex m_sex;
    system_id   m_offspring;

    static object_manager_t m_pool;
};

#endif  // SELECTION_EVENT_H_
