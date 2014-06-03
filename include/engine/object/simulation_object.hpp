#ifndef SIMULATION_OBJECT_HPP_
#define SIMULATION_OBJECT_HPP_

#include "object.hpp"
#include "../EventManager.hpp"
#include "../event_processor.hpp"
#include "../event_performer.hpp"
#include "../event_transceiver.hpp"
#include "../event_router.hpp"
//#include "poolable.h"

template< class E, class ES, class EM = EventManager< E, ES > >
class SimulationObject : 
    public object,
//    public poolable,
    public event_manager< E >,
    public event_processor,
    public event_transceiver< E >
{
public:
    typedef E event_t;
    typedef ES event_set_t;
    typedef EM event_manager_t;
    typedef event_router< E >   event_router_t;
    typedef event_performer< E > event_performer_t;

    typedef typename event_t::event_id_t event_id_t;

    SimulationObject( ) : 
        object( ),
        m_evt_manager( NULL ),
        m_router( NULL ),
        m_performer( NULL ),
        m_local_time( SystemClock::ZERO ),
//        m_pool_idx( -1 ),
        m_next_eid(0)
    {
        m_evt_manager = new event_manager_t();
    }

    SimulationObject( event_router_t * router, system_id::manager_id_t man, system_id::object_id_t oid ) :
        object( man, oid ),
        m_evt_manager( NULL ),
        m_router( router ),
        m_performer( NULL ),
        m_local_time( SystemClock::ZERO ),
        m_next_eid(0)
    {
        m_evt_manager = new event_manager_t();
    }

    SimulationObject( const SimulationObject & so ) :
        object( so ),
        m_evt_manager( so.m_evt_manager ),
        m_router( so.m_router ),
        m_performer( so.m_performer ),
        m_local_time( so.m_local_time ),
        m_next_eid( so.m_next_eid )
    {}

    virtual void process() {
        if( m_performer == NULL ) return;

        /*const*/ event_t * tmp = peekEvent( m_id );

        // while there are concurrent events
        while( tmp != NULL && tmp->isReceivedAt( m_local_time ) ) {
            tmp = getEvent( m_id );

            m_performer->perform_event( tmp );

            //tmp->release();
        //    delete tmp;
            tmp = peekEvent( m_id );
        }

//        reset_processed();

        if( tmp != NULL ) {
            vtime_t t = tmp->getReceived();
            m_router->notifyNextEvent( m_id, t );
        }
    }

    event_performer_t * getEventPerformer() {
        return m_performer;
    }

    virtual void sendEvent( /*const*/ event_t * evt ) {
        m_router->routeEvent( evt );
    }

    virtual void sendEvent( E * evt, const system_id & id, const vtime_t & t ) {
        m_router->routeEvent( evt, id, t );
    }

    virtual bool receiveEvent( /*const*/ event_t * evt ) {
        vtime_t t = evt->getReceived();
        return insertEventAt(evt, t );
    }

    virtual bool receiveEvent( event_t * evt, const vtime_t & t ) {
        return insertEventAt( evt, t );
    }

    virtual void updateLocalTime( const vtime_t & t ) {
        if( m_local_time <= t ) {
            m_local_time = t;
        } else {
            assert(false);
        }
    }

    void updatePerformer( event_performer_t * ep ) {
        m_performer = ep;
    }

    virtual const vtime_t & getCurrentTime() const {
        return m_local_time;
    }

    // event_manager interface
    inline bool  insertEvent( /*const*/ event_t * e) {
        return m_evt_manager->insertEvent( e );
    }

    bool insertEventAt( /*const*/ event_t * e, const vtime_t & t ) {
        return m_evt_manager->insertEventAt( e, t );
    }

    inline void reset_pending() {
        m_evt_manager->reset_pending();
    }

    inline void reset_processed() {
        m_evt_manager->reset_processed();
    }

    inline /*const*/ event_t * getEvent( const system_id & id) {
        return m_evt_manager->getEvent( id );
    }

    inline /*const*/ event_t * peekEvent( const system_id & id ) const {
        return m_evt_manager->peekEvent( id );
    }

    inline size_t  pendingEventCount( const system_id & id ) const {
        return m_evt_manager->pendingEventCount( id );
    }

    inline size_t  processedEventCount( const system_id & id ) const {
        return m_evt_manager->processedEventCount( id );
    }

    inline size_t  canceledEventCount( const system_id & id ) const {
        return m_evt_manager->canceledEventCount(id);
    }

    event_id_t getNextEventID( unsigned int offset = 1) {
        return m_next_eid++;
//        event_id_t tmp = __sync_fetch_and_add( &m_next_eid, offset );
//        return tmp;
    }

    virtual ~SimulationObject() {
        if( m_evt_manager != NULL ) delete m_evt_manager;
    }
protected:

    bool compareID( const system_id & id ) {
        return m_id == id;
    }

    event_manager_t * m_evt_manager;
    event_router_t * m_router;
    event_performer_t * m_performer;
    vtime_t     m_local_time;
//    size_t      m_pool_idx;

    event_id_t  m_next_eid;
};

#endif  // SIMULATION_OBJECT_HPP_
