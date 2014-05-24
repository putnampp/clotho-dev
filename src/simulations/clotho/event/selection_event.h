#ifndef SELECTION_EVENT_H_
#define SELECTION_EVENT_H_

#include "../clotho_event.h"

class SelectionEvent : public ClothoEvent {
public:
    static const event_type_t TYPE_ID = 8;

    SelectionEvent() {}

    SelectionEvent( const vtime_t & tSent, const vtime_t & tRecv,
                const system_id & sender, const system_id & receiver,
                event_id_t eid, Sex s, const system_id & pending_offspring );
    SelectionEvent( const vtime_t & tSent, const vtime_t & tRecv,
                const system_object * sender, const system_object * receiver,
                event_id_t eid, Sex s, const system_id & pending_offspring );

    inline event_type_t getEventType() const { return TYPE_ID; }

    Sex getSex() const;
    system_id getOffspring() const;

    static void * operator new( size_t s ) {
//        static shared_ptr< Pager< SelectionEvent > > pages( new Pager< SelectionEvent >() );

//        void * res = pages->malloc();
        void * res = m_pool.malloc();
        return res;
    }

    static void operator delete( void * ptr ) {
//        Pager< SelectionEvent >::release( ( SelectionEvent * )ptr );
        m_pool.free( (SelectionEvent *) ptr );
    }
//    void release() {}

    virtual ~SelectionEvent() {}

protected:
    Sex m_sex;
    system_id   m_offspring;

    static boost::object_pool< SelectionEvent > m_pool;
};

#endif  // SELECTION_EVENT_H_
