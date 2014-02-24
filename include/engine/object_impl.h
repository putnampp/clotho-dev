#ifndef OBJECT_COMMON_IMPL_H_
#define OBJECT_COMMON_IMPL_H_

#include "object.h"

class Object : virtual public object {
public:
    const system_id & getSystemID() const {
        return m_id;
    }

    system_id::object_id_t getObjectID() const {
        return m_id.getObjectID();
    }

    void setSimulationManager( simulation_manager * sim ) {
        if( m_sim_manager != NULL ) {
            m_sim_manager->unregister(this);
        }

        m_sim_manager = sim;

        setID( m_sim_manager->getNextObjectID() );

        m_sim_manager->registerObject( this );
    }

    virtual void initialize() {
        m_local_time = m_sim_manager->getSimulationTime();
    }

    virtual void executeProcess() {
        const event * tmp = peekEvent();

        // while there are concurrent events
        while( tmp != NULL && tmp->getReceived() == m_local_time ) {
            tmp = getEvent();

            handleEvent( tmp );
            
            tmp = peekEvent();
        }
    }

    virtual void finalize() { }

    virtual size_t pendingEventCount() = 0;
    virtual const event * getEvent() = 0;
    virtual const event * peekEvent() = 0;
    virtual void receiveEvent( const event * evt ) = 0;

    virtual void sendEvent( const event * evt ) {
        if( evt->getReceiver() == getSystemID() ) {
            receiveEvent( evt );
        } else {
            m_sim_manager->routeEvent( evt );
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

    virtual ~Object() {
        m_sim_manager->unregisterObject( this );
    }

protected:
    Object( ) : 
        m_id(0),
        m_sim_manager( NULL ),
        m_local_time( SystemClock::getZero() )
    {}


    Object( simulation_manager * manager ) :
        m_id(0),
        m_sim_manager( NULL ),
        m_local_time( SystemClock::getZero() )
    {
        setSimulationManager( manager );
    }

    void setID( const system_id & id ) {
        m_id = id;
    }

    system_id   m_id;
    simulation_manager * m_sim_manager;
    vtime_t     m_local_time;
};

#endif  // OBJECT_COMMON_IMPL_H_
