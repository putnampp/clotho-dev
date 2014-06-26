#ifndef MATURITY_EVENT_H_
#define MATURITY_EVENT_H_

#include "clotho_event.h"

class MaturityEvent : public ClothoEvent {
public:
    typedef ObjectManager< MaturityEvent > object_manager_t;

//    static const event_type_t TYPE_ID = 5;

    CLOTHO_EVENT_ID( 5 )

    void init( const vtime_t & tSent, const vtime_t & tRecv,
                const system_id & sender, const system_id & receiver,
                event_id_t eid, const system_id & obj, Sex s = UNKNOWN );
    void init( const vtime_t & tSent, const vtime_t & tRecv,
                const system_object * sender, const system_object * receiver,
                event_id_t eid, const system_id & obj, Sex s = UNKNOWN );

    //inline event_type_t getEventType() const { return TYPE_ID; }

    system_id getMatureObject() const;
    Sex getSex() const;

    static void * operator new( size_t s ) {
        void * res = m_pool.malloc();
        return res;
    }

    static void operator delete( void * ptr ) {
        m_pool.free( (MaturityEvent *) ptr );
    }

    virtual ~MaturityEvent() {}

//protected:
    MaturityEvent() {}

    MaturityEvent( const vtime_t & tSent, const vtime_t & tRecv,
                const system_id & sender, const system_id & receiver,
                event_id_t eid, const system_id & obj, Sex s = UNKNOWN );
    MaturityEvent( const vtime_t & tSent, const vtime_t & tRecv,
                const system_object * sender, const system_object * receiver,
                event_id_t eid, const system_id & obj, Sex s = UNKNOWN );
protected:
    system_id m_mature_obj;
    Sex m_sex;

    static object_manager_t m_pool;
};

#endif  // MATURITY_EVENT_H_
