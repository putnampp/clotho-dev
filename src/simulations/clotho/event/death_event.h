#ifndef DEATH_EVENT_H_
#define DEATH_EVENT_H_

#include "../clotho_event.h"
//#include "pooler.hpp"

class DeathEvent : public ClothoEvent {
public:
    static const event_type_t TYPE_ID = 2;

    //friend class Pooler< DeathEvent >;

//    static DeathEvent * getOrCreate() {
//        return DeathEvent::m_pool->getOrCreate();
//    }

    void init( const vtime_t & tSent, const vtime_t & tRecv,
                const system_id & sender, const system_id & receiver,
                event_id_t eid );

    void init( const vtime_t & tSent, const vtime_t & tRecv,
                const system_object * sender, const system_object * receiver,
                event_id_t eid );

    inline event_type_t getEventType() const { return TYPE_ID; }

    static void * operator new( size_t s ) {
//        static shared_ptr< Pager< DeathEvent > > pages( new Pager< DeathEvent >() );

//        void * res = pages->malloc();
        void * res = m_pool.malloc();
        return res;
    }

    static void operator delete( void * ptr ) {
//        Pager< DeathEvent >::release( ( DeathEvent * )ptr );
        m_pool.free( (DeathEvent *) ptr );
    }

//    void release();

    virtual ~DeathEvent() {}
//protected:
    DeathEvent() {}

    DeathEvent( const vtime_t & tSent, const vtime_t & tRecv,
                const system_id & sender, const system_id & receiver,
                event_id_t eid );

    DeathEvent( const vtime_t & tSent, const vtime_t & tRecv,
                const system_object * sender, const system_object * receiver,
                event_id_t eid );

//    static shared_ptr< Pooler< DeathEvent > > m_pool;
    static boost::object_pool< DeathEvent > m_pool;
};

#endif  // DEATH_EVENT_H_
