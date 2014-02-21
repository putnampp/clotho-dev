#include "SequentialSimulationManager.h"
#include "cassert"

#include <algorithm>

using std::lower_bound;

struct object_timestamp_comp {
    bool operator()( const SequentialSimulationManager::pair_object_timestamp & lhs, const SequentialSimulationManager::pair_object_timestamp & rhs ) const {
        return lhs.second < rhs.second;
    }
};

system_id::object_id_t SequentialSimulationManager::m_next_object_id = 0;

SequentialSimulationManager::SequentialSimulationManager( application * app, system_id::manager_id_t id ) :
    m_app(app),
    m_id( id, m_next_object_id++ )
{}

const system_id & SequentialSimulationManager::getSystemID() const {
    return m_id;
}

system_id::manager_id_t SequentialSimulationManager::getManagerID() const {
    return m_id.getManagerID();
}

void SequentialSimulationManager::registerObject( object * obj ) {
    system_id obj_id( m_id.getManagerID(), m_next_object_id++ );
    obj->setID();

    m_objects[ obj_id ] = obj;
    m_objects_next[ obj_id ] = m_ordered_objs.end();
}

void SequentialSimulationManager::unregisterObject( object * obj ) {
    object_handle_map_t::iterator it = m_objects.find( obj->getSystemID() );
    ASSERT( it != m_objects.end() );

    m_objects.erase( it );
}

size_t SequentialSimulationManager::getObjectCount() const {
    return m_objects.size();
}

void SequentialSimulationManager::routeEvent( const event * evt ) {
    object_handle_map_t::iterator it = m_objects.find( evt->getReceiver() );

    ASSERT( it != m_objects.end() );

    it->second->receiveEvent( evt );
}

void SequentialSimulationManager::notifyNextEvent( const system_id & obj, const event::vtime_t & t ) {
    object_next_event_map_t::iterator it = m_objects_next.find( obj );

    ASSERT( it != m_objects_next.end() );
    ASSERT( (it->second == m_ordered_objs.end()) || (t < it->second->second) );

    pair_object_timestamp ot = make_pair( obj, t );

    ordered_object_exe_t::iterator pos = lower_bound( m_ordered_objs.begin(), it->second, ot, object_timestamp_comp());

    if( it->second == m_ordered_objs.end() ) {
        it->second = m_ordered_objs.insert( pos, ot );
    } else if( it->second != pos ) {
        m_ordered_objs.erase( it->second );
        it->second = m_ordered_objs.insert( pos, p );
    } else if( pos->first == obj ) {
        pos->second = t;
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

void SequentialSimulationManager::simulate() {
    while(! m_ordered_objs.empty() ) {
        pair_object_timestamp ot = getNextObject();

        if( setSimulationTime( ot.second ) ) break;

        m_objects[ ot.first ]->executeProcess();
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
