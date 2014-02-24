#ifndef TEST_OBJECT_H_
#define TEST_OBJECT_H_

#include "object.h"

#include <ostream>
#include <list>

using std::ostream;
using std::list;

class TestObject : public object {
public:
    TestObject( ) : m_id(0), m_time(0), m_sim(NULL) { }

    TestObject( simulation_manager * simman ) : m_id(0), m_time(0), m_sim(NULL) {
        setSimulationManager(simman);
    }

    virtual const system_id &       getSystemID() const {
        return m_id;
    }
    virtual system_id::object_id_t  getObjectID() const {
        return m_id.getObjectID();
    }

    virtual void setID( const system_id & id ) {
        m_id = id;
    }

    virtual void setSimulationManager( simulation_manager * sim ) {
        m_sim = sim;

        setID( m_sim->getNextObjectID() );
        m_sim->registerObject( this );
    }

    virtual void initialize() {}
    virtual void process() {}
    virtual void finalize() {}

    virtual size_t pendingEventCount() {
        return m_events.size();
    }

    virtual const event * getEvent() {
        if( m_events.empty() ) return NULL;
        const event * tmp = m_events.front();
        m_events.pop_front();
        return tmp;
    }

    virtual const event * peekEvent() const {
        if( m_events.empty() ) return NULL;
        return m_events.front();
    }

    virtual void insertEvent( const event * evt ) {
        m_events.push_back( evt );    
    }

    virtual void sendEvent( const event * evt ) {
        if( evt->getReceiver() != getSystemID() ) {
            m_sim->routeEvent( evt );
        } else {
            receiveEvent( evt );
        }
    }

    virtual void receiveEvent( const event * evt ) {
        insertEvent( evt );
    }

    virtual void updateLocalTime( const vtime_t & t ) {
        m_time = t;
    }

    virtual const vtime_t & getCurrentTime() const {
        return m_time;
    }

    virtual ~TestObject() {
        m_sim->unregisterObject( this );
    }
protected:
    system_id   m_id;
    vtime_t m_time;
    simulation_manager * m_sim;

    list< const event * > m_events;
};

ostream & operator<<( ostream & out, const TestObject & obj ) {
    out << obj.getSystemID() << " - " << obj.getCurrentTime();
    return out;
}

#endif  // TEST_OBJECT_H_
