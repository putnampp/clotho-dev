#include "SequentialSimulationManager.h"
#include <cassert>

#include <algorithm>

#include <iostream>

using std::cout;
using std::endl;
using std::lower_bound;

struct object_timestamp_comp {
    bool operator()( const SequentialSimulationManager::pair_object_timestamp & lhs, const SequentialSimulationManager::pair_object_timestamp & rhs ) const {
        return lhs.second < rhs.second;
    }
};

SequentialSimulationManager::SequentialSimulationManager( application * app, system_id::manager_id_t id ) :
    m_app(app),
    m_id( id, 0 ),
    m_sim_time( SystemClock::getZero() ),
    m_sim_until( SystemClock::getPositiveInfinity() ),
    m_sim_complete(false),
    m_next_object_id( 1 )
{}

SequentialSimulationManager::~SequentialSimulationManager() {
    while( !m_objects.empty() ) {
        object * tmp = m_objects.begin()->second;
        m_objects.erase( m_objects.begin());
        delete tmp;
    }

    while( !m_ordered_objs.empty() ) {
        getNextObject();
    }

    m_objects_next.clear();
}

const system_id & SequentialSimulationManager::getSystemID() const {
    return m_id;
}

system_id::manager_id_t SequentialSimulationManager::getManagerID() const {
    return m_id.getManagerID();
}

const system_id SequentialSimulationManager::getNextObjectID() {
    return system_id( m_id.getManagerID(), m_next_object_id++);
}

void SequentialSimulationManager::registerObject( object * obj ) {
    if( obj == NULL ) return;

    assert( obj->getSystemID() != system_id(0) );

    m_objects[ obj->getSystemID() ] = obj;
    m_objects_next[ obj->getSystemID() ] = m_ordered_objs.end();
}

void SequentialSimulationManager::unregisterObject( object * obj ) {
    if( obj == NULL ) return;

    object_handle_map_t::iterator it = m_objects.find( obj->getSystemID() );
    if( it != m_objects.end() )
        m_objects.erase( it );
}

size_t SequentialSimulationManager::getObjectCount() const {
    return m_objects.size();
}

object * SequentialSimulationManager::getObject( const system_id & id ) const {
    object_handle_map_t::const_iterator it = m_objects.find( id );
    if( it == m_objects.end() ) return NULL;

    return it->second;
}

void SequentialSimulationManager::routeEvent( const event * evt ) {
    object_handle_map_t::iterator it = m_objects.find( evt->getReceiver() );

    assert( it != m_objects.end() );

    it->second->receiveEvent( evt );
}

void SequentialSimulationManager::notifyNextEvent( const system_id & obj, const event::vtime_t & t ) {
    cout << "Simulation Manager notified of next event" << endl;
    object_next_event_map_t::iterator it = m_objects_next.find( obj );

    assert( it != m_objects_next.end() );

    pair_object_timestamp ot = make_pair( obj, t );

    ordered_object_exe_t::iterator pos = lower_bound( m_ordered_objs.begin(), it->second, ot, object_timestamp_comp());

    if( it->second == m_ordered_objs.end() ) {
        it->second = m_ordered_objs.insert( pos, ot );
    } else if( it->second != pos ) {
        m_ordered_objs.erase( it->second );
        it->second = m_ordered_objs.insert( pos, ot );
    } else if( pos->first == obj ) {
        it->second->second = t;
    } else {
        assert( false );
    }
}

SequentialSimulationManager::pair_object_timestamp SequentialSimulationManager::getNextObject() {
    pair_object_timestamp ot = m_ordered_objs.front();

    m_ordered_objs.pop_front();
    m_objects_next[ ot.first ] = m_ordered_objs.end();

    return ot;
}

void SequentialSimulationManager::initialize() {
    m_app->initialize();
}

void SequentialSimulationManager::simulate( const event::vtime_t & until ) {
    setSimulateUntil( until );

    while(! m_ordered_objs.empty() ) {
        pair_object_timestamp ot = getNextObject();

        cout << "Simulating event: " << ot.first << " @ " << ot.second << endl;

        if( setSimulationTime( ot.second ) ) {
            cout << "Simulation Complete" << endl;
            break;
        }

        object * obj = m_objects[ ot.first ];
        obj->updateLocalTime( ot.second );
        obj->process();
    }
}

void SequentialSimulationManager::finalize() {
    m_app->finalize();
}

bool SequentialSimulationManager::isSimulationComplete() const {
    return m_sim_complete;
}

bool SequentialSimulationManager::setSimulationTime( const event::vtime_t & t ) {
    assert( m_sim_time <= t );
    if( m_sim_time != t ) {
        m_sim_time = t;
        m_sim_complete = (m_sim_time >= m_sim_until);
    }

    return m_sim_complete;
}

void SequentialSimulationManager::setSimulateUntil( const event::vtime_t & t ) {
    m_sim_until = t;
}

const event::vtime_t & SequentialSimulationManager::getSimulationTime() const {
    return m_sim_time;
}
