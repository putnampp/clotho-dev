#ifndef OBJECT_COMMON_IMPL_H_
#define OBJECT_COMMON_IMPL_H_

#include "object.h"

#include "simulation_manager.h"

#include <iostream>

using std::cout;
using std::endl;

class Object : virtual public object {
public:
    const system_id & getSystemID() const {
        return m_id;
    }

    system_id::manager_id_t getManagerID() const {
        return m_id.getManagerID();
    }

    system_id::object_id_t getObjectID() const {
        return m_id.getObjectID();
    }

    void setSimulationManager( simulation_manager * sim ) {
        if( m_sim_manager != NULL ) {
            m_sim_manager->unregisterObject(this);
        }

        if( sim == NULL ) return;

        m_sim_manager = sim;

        setID( m_sim_manager->getNextObjectID() );

        m_sim_manager->registerObject( this );
    }

    virtual void initialize() {
        m_local_time = m_sim_manager->getSimulationTime();
    }

    virtual void process() {
        const event * tmp = peekEvent( m_id );

        // while there are concurrent events
        while( tmp != NULL && tmp->getReceived() == m_local_time ) {
            tmp = getEvent( m_id );

            //cout << m_id << "@" << getCurrentTime() << ": " <<  tmp << "\n";
            perform_event( tmp );

            tmp = peekEvent( m_id );

            if( tmp != NULL ) {
                m_sim_manager->notifyNextEvent( getSystemID(), tmp->getReceived() );
            }
        }
    }

    virtual void perform_event( const event * e ) {}

    virtual void finalize() { 
        if( m_sim_manager )
            m_sim_manager->unregisterObject( this );
    }

    virtual void sendEvent( const event * evt ) {
        if( evt->getReceiver() == getSystemID() ) {
            receiveEvent( evt );
        } else {
            m_sim_manager->routeEvent( evt );
        }
    }

    virtual void receiveEvent( const event * evt ) {
        if( insertEvent( evt ) ) {
            m_sim_manager->notifyNextEvent( getSystemID(), evt->getReceived() );
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

    virtual ~Object() { }

protected:
    Object( ) :
        m_id(0),
        m_sim_manager( NULL ),
        m_local_time( SystemClock::ZERO ),
        m_next_eid( 0 )
    {}


    Object( simulation_manager * manager ) :
        m_id(0),
        m_sim_manager( NULL ),
        m_local_time( SystemClock::ZERO ),
        m_next_eid(0)
    {
        setSimulationManager( manager );
    }

    void setID( const system_id & id ) {
        m_id = id;
    }

    event::event_id_t getNextEventID() {
        return m_next_eid++;
    }

    system_id   m_id;
    simulation_manager * m_sim_manager;
    vtime_t     m_local_time;
    event::event_id_t  m_next_eid;
};

#endif  // OBJECT_COMMON_IMPL_H_
