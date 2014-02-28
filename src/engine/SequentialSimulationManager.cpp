#include "SequentialSimulationManager.h"
#include <cassert>

#include <algorithm>
#include <iostream>

using std::cout;
using std::endl;
using std::lower_bound;
using std::upper_bound;

const string INIT_PHASE_K = "init";
const string SIMULATE_PHASE_K = "simulate";
const string FINALIZE_PHASE_K = "finalize";

SequentialSimulationManager::SequentialSimulationManager( application * app, system_id::manager_id_t id ) :
    SimulationManager( id ),
    m_app(app),
    m_sim_time( SystemClock::ZERO ),
    m_sim_until( SystemClock::POSITIVE_INFINITY ),
    m_sim_complete(false),
    m_nPendingEvents(0),
    m_nProcessedEvents(0),
    m_stats( new SimulationStats() )
{}

SequentialSimulationManager::SequentialSimulationManager( application * app, shared_ptr< SimulationStats > stats, system_id::manager_id_t id ) :
    SimulationManager( id ),
    m_app(app),
    m_sim_time( SystemClock::ZERO ),
    m_sim_until( SystemClock::POSITIVE_INFINITY ),
    m_sim_complete(false),
    m_nPendingEvents(0),
    m_nProcessedEvents(0),
    m_stats( stats )
{

}

SequentialSimulationManager::~SequentialSimulationManager() {
    while( !m_objects.empty() ) {
        object * tmp = m_objects.begin()->second;
        m_objects.erase( m_objects.begin());
        if( tmp ) {
            cout << tmp->getSystemID() << " was never unregistered" << endl;
            delete tmp;
        }
    }

    while( !m_ordered_objs.empty() ) {
        getNextObject();
    }

    m_objects_next.clear();
}

void SequentialSimulationManager::registerObject( object * obj ) {
    if( obj == NULL ) return;

    assert( obj->getSystemID() != m_id );

    pair_object_timestamp ot = make_pair( obj->getSystemID(), SystemClock::POSITIVE_INFINITY );

    m_objects[ ot.first ] = obj;
    m_objects_next[ ot.first ] = m_ordered_objs.insert( m_ordered_objs.end(), ot );
}

void SequentialSimulationManager::unregisterObject( object * obj ) {
    if( obj == NULL ) return;

    object_handle_map_t::iterator it = m_objects.find( obj->getSystemID() );
    if( it != m_objects.end() ) {
        m_nPendingEvents += obj->pendingEventCount();
        m_nProcessedEvents += obj->processedEventCount();

        object_next_event_map_t::iterator nit = m_objects_next.find( obj->getSystemID() );
        assert( nit != m_objects_next.end() );

        m_ordered_objs.erase( nit->second );
        m_objects_next.erase(nit);
        m_objects.erase( it );
    }
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
    object_next_event_map_t::iterator it = m_objects_next.find( obj );

    assert( it != m_objects_next.end() );

    pair_object_timestamp ot = make_pair( obj, t );

    ordered_object_exe_t::iterator pos = upper_bound( m_ordered_objs.begin(), it->second, ot, m_ot_comp );

    if( it->second == m_ordered_objs.end() ) {
        it->second = m_ordered_objs.insert( pos, ot );
    } else if( it->second != pos ) {
//        m_ordered_objs.erase( it->second );
//        it->second = m_ordered_objs.insert( pos, ot );
        m_ordered_objs.splice( pos, m_ordered_objs, it->second );
        it->second->second = t;
    } else if( pos->first == obj ) {
        it->second->second = t;
    } else {
        assert( false );
    }
}

SequentialSimulationManager::pair_object_timestamp SequentialSimulationManager::getNextObject() {
    pair_object_timestamp ot = m_ordered_objs.front();

    // rotate the head node to the end of the list
    // and set its time to be POSITIVE_INFINITY
    m_ordered_objs.splice( m_ordered_objs.end(), m_ordered_objs, m_ordered_objs.begin() );
    m_ordered_objs.back().second = SystemClock::POSITIVE_INFINITY;

    return ot;
}

void SequentialSimulationManager::initialize() {
    m_stats->startPhase( INIT_PHASE_K );
    m_app->initialize();
    m_stats->stopPhase( INIT_PHASE_K );
}

void SequentialSimulationManager::simulate( const event::vtime_t & until ) {
    setSimulateUntil( until );

    m_stats->startPhase( SIMULATE_PHASE_K );
    while(! m_ordered_objs.empty() ) {
        pair_object_timestamp ot = getNextObject();

        if( ot.second == SystemClock::POSITIVE_INFINITY || setSimulationTime( ot.second ) ) {
            break;
        }

        object * obj = m_objects[ ot.first ];
        obj->updateLocalTime( ot.second );
        obj->process();
    }

    m_stats->stopPhase( SIMULATE_PHASE_K );
}

void SequentialSimulationManager::finalize() {
    m_stats->startPhase( FINALIZE_PHASE_K );

    m_app->finalize();

    m_stats->stopPhase( FINALIZE_PHASE_K );

    m_stats->setProcessedEvents( m_nProcessedEvents );
    m_stats->setPendingEvents( m_nPendingEvents );

/*    cout << "\nSequentialSimulationManager: Events Processed = "
         << m_nProcessedEvents
         << "; Events Pending = "
         << m_nPendingEvents
         << endl;
*/
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
