#ifndef INHERIT_EVENT_HPP_
#define INHERIT_EVENT_HPP_

#include "../clotho_event.h"
//#include "pooler.hpp"

template < class GM >
class InheritEvent : public ClothoEvent {
public:

    static const event_type_t TYPE_ID = 4;
    typedef GM gamete_t;

//    friend class Pooler< InheritEvent< GM > >;

//    static InheritEvent< GM > * getOrCreate() {
//        return m_pool->getOrCreate();
//    }

    // by default gamete index is unknown (-1)
    // gamete index can be used to identify which "source" ("parent")
    // a gamete was inherited from (ie. source gender)
    void init( const vtime_t & tSent, const vtime_t & tRecv,
                const system_object * sender, const system_object * receiver,
                event_id_t eid, gamete_t * z, unsigned char gidx = -1) {
        ClothoEvent::init( tSent, tRecv, sender, receiver, eid );
        m_gidx = gidx;
        m_gamete = z;
    }

    void init( const vtime_t & tSent, const vtime_t & tRecv,
                const system_id & sender, const system_id & receiver,
                event_id_t eid, gamete_t * z, unsigned char gidx = -1) {
        ClothoEvent::init( tSent, tRecv, sender, receiver, eid );
        m_gidx = gidx;
        m_gamete = z;
    }

    inline event_type_t getEventType() const { return TYPE_ID; }

    unsigned char getParentIndex() const { return m_gidx; }
    gamete_t * getGamete() const { return m_gamete; }

    static void * operator new( size_t s ) {
//        static shared_ptr< Pager< InheritEvent< GM > > > pages( new Pager< InheritEvent< GM > >() );

//        void * res = pages->malloc();
        void * res = m_pool.malloc();
        return res;
    }

    static void operator delete( void * ptr ) {
//        Pager< InheritEvent<GM> >::release( ( InheritEvent< GM > * )ptr );
        m_pool.free( (InheritEvent< GM > *) ptr );
    }
//    void release() {
//        m_pool->release(this);
//    }

    virtual ~InheritEvent() {}
//protected:

    InheritEvent() {}

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

protected:
    unsigned char   m_gidx;
    gamete_t *      m_gamete;

//    static shared_ptr< Pooler< InheritEvent< GM > > > m_pool;
//    static boost::object_pool< InheritEvent< GM > > m_pool;
    static ObjectManager< InheritEvent< GM > > m_pool;
};

template < class GM >
ObjectManager< InheritEvent< GM > > InheritEvent<GM>::m_pool;
//boost::object_pool< InheritEvent< GM > > InheritEvent<GM>::m_pool;
//template < class GM >
//shared_ptr< Pooler< InheritEvent< GM > > > InheritEvent<GM>::m_pool( new Pooler< InheritEvent< GM > >() );

#endif  // INHERIT_EVENT_HPP_
