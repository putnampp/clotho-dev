#ifndef BIRTH_EVENT_H_
#define BIRTH_EVENT_H_

#include "../clotho_event.h"
//#include "pooler.hpp"
#include "engine/pager.hpp"

class BirthEvent : public ClothoEvent {
public:
    static const event_type_t TYPE_ID = 1;

//    friend class Pooler< BirthEvent >;

//    static BirthEvent * getOrCreate() {
//        return BirthEvent::m_pool->getOrCreate();
//    }

    void init( const vtime_t & tSent, const vtime_t & tRecv,
                const system_id & sender, const system_id & receiver,
                event_id_t eid, Sex s = UNKNOWN );
    void init( const vtime_t & tSent, const vtime_t & tRecv,
                const system_object * sender, const system_object * receiver,
                event_id_t eid, Sex s = UNKNOWN );

    inline event_type_t getEventType() const { return TYPE_ID; }

    Sex getSex() const;

    static void * operator new( size_t s ) {
//        static shared_ptr< Pager< BirthEvent > > pages( new Pager< BirthEvent >() );
//        void * res = pages->malloc();
        void * res = m_pool.malloc();
        return res;
    }

    static void operator delete( void * ptr ) {
//        Pager< BirthEvent >::release( ( BirthEvent * )ptr );
        m_pool.free( (BirthEvent * ) ptr );
    }

//    void release();

    virtual ~BirthEvent() {}
//protected:
    BirthEvent() {}

    BirthEvent( const vtime_t & tSent, const vtime_t & tRecv,
                const system_id & sender, const system_id & receiver,
                event_id_t eid, Sex s = UNKNOWN );
    BirthEvent( const vtime_t & tSent, const vtime_t & tRecv,
                const system_object * sender, const system_object * receiver,
                event_id_t eid, Sex s = UNKNOWN );
protected:
    Sex m_sex;

    //static shared_ptr< Pooler< BirthEvent > > m_pool;
    static boost::object_pool< BirthEvent > m_pool;
};

#endif  // BIRTH_EVENT_H_
