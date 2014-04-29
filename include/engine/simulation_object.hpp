#ifndef SIMULATION_OBJECT_HPP_
#define SIMULATION_OBJECT_HPP_

#include "object.hpp"
#include "EventManager.hpp"
#include "event_processor.hpp"
#include "event_transceiver.hpp"
#include "event_router.hpp"

template< class E, class ES >
class SimulationObject : 
    public object,
    public EventManager< E, ES >,
    public event_processor< E >,
    public event_transceiver< E >
{
public:
    typedef typename EventManager< E, ES >::event_t event_t;
    typedef event_router< E >   event_router_t;
//    typedef SIM simulation_manager_t;

    SimulationObject( ) : 
        object( ),
        m_router( NULL ),
        m_local_time( SystemClock::ZERO ),
        m_next_eid(0)
    {}

    SimulationObject( event_router_t * router ) :
        object( ),
        m_router( router ),
        m_local_time( SystemClock::ZERO ),
        m_next_eid(0)
    { }
/*
    virtual void initialize() {
        m_local_time = m_sim_manager->getSimulationTime();
    }

    virtual void setSimulationManager( simulation_manager_t * sim ) {
        if( m_sim_manager != NULL ) {
            m_sim_manager->unregisterObject(this);
        }

        if( sim == NULL ) return;

        m_sim_manager = sim;

        setID( m_sim_manager->getNextObjectID() );

        m_sim_manager->registerObject( this );
    }
*/
    virtual void process() {
        const event_t * tmp = EventManager<E, ES>::peekEvent( m_id );

        // while there are concurrent events
        while( tmp != NULL && tmp->getReceived() == m_local_time ) {
            tmp = EventManager<E, ES>::getEvent( m_id );

            perform_event( tmp );

            tmp = EventManager<E, ES>::peekEvent( m_id );

            if( tmp != NULL ) {
                m_router->notifyNextEvent( getSystemID(), tmp->getReceived() );
            }
        }

        EventManager<E, ES>::reset_processed();
    }

    virtual void perform_event( const event_t * e ) {}

    virtual void sendEvent( const event_t * evt ) {
        if( evt->getReceiver() == getSystemID() ) {
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
/*
    virtual void finalize() { 
        if( m_sim_manager )
            m_sim_manager->unregisterObject( this );
    }
*/
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

    event::event_id_t getNextEventID() {
        return m_next_eid++;
    }

 //   simulation_manager_t * m_sim_manager;
    event_router_t * m_router;
    vtime_t     m_local_time;

    event::event_id_t  m_next_eid;
};

#endif  // SIMULATION_OBJECT_HPP_
