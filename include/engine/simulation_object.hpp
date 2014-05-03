#ifndef SIMULATION_OBJECT_HPP_
#define SIMULATION_OBJECT_HPP_

#include "object.hpp"
#include "EventManager.hpp"
#include "event_processor.hpp"
#include "event_transceiver.hpp"
#include "event_router.hpp"
#include "poolable.h"

template< class E, class ES >
class SimulationObject : 
    public object,
    public poolable,
    public EventManager< E, ES >,
    public event_processor< E >,
    public event_transceiver< E >
{
public:
    typedef typename EventManager< E, ES >::event_t event_t;
    typedef event_router< E >   event_router_t;

    SimulationObject( ) : 
        object( ),
        m_router( NULL ),
        m_local_time( SystemClock::ZERO ),
        m_pool_idx( -1 ),
        m_next_eid(0)
    {}

    SimulationObject( event_router_t * router ) :
        object( ),
        m_router( router ),
        m_local_time( SystemClock::ZERO ),
        m_pool_idx(-1),
        m_next_eid(0)
    { }

    SimulationObject( const SimulationObject & so ) :
        object( so ),
        m_router( so.m_router ),
        m_local_time( so.m_local_time ),
        m_pool_idx( so.m_pool_idx ),
        m_next_eid( so.m_next_eid )
    {}

    size_t getPoolIndex() const { return m_pool_idx; }
    void setPoolIndex( size_t idx ) { m_pool_idx = idx; }

    virtual void process() {
        const event_t * tmp = EventManager<E, ES>::peekEvent( m_id );

        // while there are concurrent events
        while( tmp != NULL && tmp->isReceivedAt( m_local_time ) ) {
            tmp = EventManager<E, ES>::getEvent( m_id );

            perform_event( tmp );

            tmp = EventManager<E, ES>::peekEvent( m_id );
        }

        EventManager<E, ES>::reset_processed();

        if( tmp != NULL ) {
            m_router->notifyNextEvent( getSystemID(), tmp->getReceived() );
        }
    }

    virtual void perform_event( const event_t * e ) {}

    virtual void sendEvent( const event_t * evt ) {
        if( compareID( evt->getReceiver() ) ) {
            receiveEvent( evt );
        } else {
            m_router->routeEvent( evt );
        }
    }

    virtual void receiveEvent( const event_t * evt ) {
        if( EventManager<E,ES>::insertEvent( evt ) ) {
            m_router->notifyNextEvent( getSystemID(), evt->getReceived() );
        }
    }

    virtual void updateLocalTime( const vtime_t & t ) {
        assert( m_local_time <= t );

        if( m_local_time < t ) {
            m_local_time = t;
        }
    }

    virtual const vtime_t & getCurrentTime() const {
        return m_local_time;
    }

    virtual ~SimulationObject() { }
protected:

    bool compareID( const system_id & id ) {
        return m_id == id;
    }

    event::event_id_t getNextEventID() {
        return m_next_eid++;
    }

    event_router_t * m_router;
    vtime_t     m_local_time;
    size_t      m_pool_idx;

    event::event_id_t  m_next_eid;
};

#endif  // SIMULATION_OBJECT_HPP_
