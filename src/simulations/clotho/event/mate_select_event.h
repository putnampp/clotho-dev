#ifndef MATE_SELECT_EVENT_H_
#define MATE_SELECT_EVENT_H_

#include "../clotho_event.h"

class MateSelectEvent : public ClothoEvent {
public:
    typedef ObjectManager< MateSelectEvent, EVENT_PAGE_SIZE > object_manager_t;

    static const event_type_t TYPE_ID = 7;
 
    void init( const vtime_t & tSent, const vtime_t & tRecv,
                const system_id & sender, const system_id & receiver,
                event_id_t eid );
    void init( const vtime_t & tSent, const vtime_t & tRecv,
                const system_object * sender, const system_object * receiver,
                event_id_t eid );

    inline event_type_t getEventType() const { return TYPE_ID; }

    static void * operator new( size_t s ) {
        void * res = m_pool.malloc();
        return res;
    }

    static void operator delete( void * ptr ) {
        m_pool.free( (MateSelectEvent * )ptr );
    }

    virtual ~MateSelectEvent() {}

//protected:
    MateSelectEvent() {}

    MateSelectEvent( const vtime_t & tSent, const vtime_t & tRecv,
                const system_id & sender, const system_id & receiver,
                event_id_t eid );
    MateSelectEvent( const vtime_t & tSent, const vtime_t & tRecv,
                const system_object * sender, const system_object * receiver,
                event_id_t eid );

    static object_manager_t m_pool;
};

#endif  // MATE_SELECT_EVENT_H_
