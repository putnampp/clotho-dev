#ifndef SIGNAL_MATE_EVENT_H_
#define SIGNAL_MATE_EVENT_H_

#include "../clotho_event.h"

class SignalMateEvent : public ClothoEvent {
public:
    static const event_type_t TYPE_ID = 9;

    SignalMateEvent() {}
    SignalMateEvent( const vtime_t & tSent, const vtime_t & tRecv,
                const system_id & sender, const system_id & receiver,
                event_id_t eid, Sex s, unsigned int idx, const system_id & offspring );
    SignalMateEvent( const vtime_t & tSent, const vtime_t & tRecv,
                const system_object * sender, const system_object * receiver,
                event_id_t eid, Sex s, unsigned int idx, const system_id & offspring );

    inline event_type_t getEventType() const { return TYPE_ID; }

    Sex getSex() const;
    unsigned int getIndex() const;

    system_id   getOffspring() const;

    static void * operator new( size_t s ) {
//        static shared_ptr< Pager< SignalMateEvent > > pages( new Pager< SignalMateEvent >() );

//        void * res = pages->malloc();
        void * res = m_pool.malloc();
        return res;
    }

    static void operator delete( void * ptr ) {
//        Pager< SignalMateEvent >::release( ( SignalMateEvent * )ptr );
        m_pool.free( (SignalMateEvent *) ptr );
    }
//    void release() {}

    virtual ~SignalMateEvent() {}
protected:
    Sex             m_sex;
    unsigned int    m_index;
    system_id       m_offspring;

    static boost::object_pool< SignalMateEvent > m_pool;
};

#endif  // SIGNAL_MATE_EVENT_H_
