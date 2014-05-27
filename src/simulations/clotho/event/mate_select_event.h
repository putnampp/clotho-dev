#ifndef MATE_SELECT_EVENT_H_
#define MATE_SELECT_EVENT_H_

#include "../clotho_event.h"
//#include "pooler.hpp"

class MateSelectEvent : public ClothoEvent {
public:
    static const event_type_t TYPE_ID = 7;

//    friend class Pooler< MateSelectEvent >;

//    static MateSelectEvent * getOrCreate();
 
    void init( const vtime_t & tSent, const vtime_t & tRecv,
                const system_id & sender, const system_id & receiver,
                event_id_t eid );
    void init( const vtime_t & tSent, const vtime_t & tRecv,
                const system_object * sender, const system_object * receiver,
                event_id_t eid );

    inline event_type_t getEventType() const { return TYPE_ID; }

    static void * operator new( size_t s ) {
//        static shared_ptr< Pager< MateSelectEvent > > pages( new Pager< MateSelectEvent >() );

//        void * res = pages->malloc();
        void * res = m_pool.malloc();
        return res;
    }

    static void operator delete( void * ptr ) {
//        Pager< MateSelectEvent >::release( ( MateSelectEvent * )ptr );
        m_pool.free( (MateSelectEvent * )ptr );
    }
//    void release();

    virtual ~MateSelectEvent() {}

//protected:
    MateSelectEvent() {}

    MateSelectEvent( const vtime_t & tSent, const vtime_t & tRecv,
                const system_id & sender, const system_id & receiver,
                event_id_t eid );
    MateSelectEvent( const vtime_t & tSent, const vtime_t & tRecv,
                const system_object * sender, const system_object * receiver,
                event_id_t eid );

//    static shared_ptr< Pooler< MateSelectEvent > > m_pool;
//    static boost::object_pool< MateSelectEvent > m_pool;
    static ObjectManager< MateSelectEvent > m_pool;
};

#endif  // MATE_SELECT_EVENT_H_
