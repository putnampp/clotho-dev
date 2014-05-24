#ifndef MATE_EVENT_H_
#define MATE_EVENT_H_

#include "../clotho_event.h"
//#include "pooler.hpp"


class MateEvent : public ClothoEvent {
public:
    static const event_type_t TYPE_ID = 6;
//    friend class Pooler< MateEvent >;

//    static MateEvent * getOrCreate() {
//        return MateEvent::m_pool->getOrCreate();
//    }

    void init( const vtime_t & tSent, const vtime_t & tRecv,
                const system_id & sender, const system_id & receiver,
                event_id_t eid, const system_id & offspring );
    void init( const vtime_t & tSent, const vtime_t & tRecv,
                const system_object * sender, const system_object * receiver,
                event_id_t eid, const system_object * offspring );

    inline event_type_t getEventType() const { return TYPE_ID; }

    const system_id & getOffspringID() const;

    static void * operator new( size_t s ) {
//        static shared_ptr< Pager< MateEvent > > pages( new Pager< MateEvent >() );

//        void * res = pages->malloc();
        void * res = m_pool.malloc();
        return res;
    }

    static void operator delete( void * ptr ) {
//        Pager< MateEvent >::release( ( MateEvent * )ptr );
        m_pool.free( (MateEvent *) ptr );
    }
//    void release();

    virtual ~MateEvent() {}
//protected:
    MateEvent() {}

    MateEvent( const vtime_t & tSent, const vtime_t & tRecv,
                const system_id & sender, const system_id & receiver,
                event_id_t eid, const system_id & offspring );
    MateEvent( const vtime_t & tSent, const vtime_t & tRecv,
                const system_object * sender, const system_object * receiver,
                event_id_t eid, const system_object * offspring );
protected:
    system_id   m_offspring;

//    static shared_ptr< Pooler< MateEvent > > m_pool;
    static boost::object_pool< MateEvent > m_pool;
};

#endif  // MATE_EVENT_H_
