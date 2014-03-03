#ifndef SEQUENTIALSIMULATIONMANAGER_HPP_
#define SEQUENTIALSIMULATIONMANAGER_HPP_

//#include "simulation_manager.h"
#include "simulation_manager_impl.h"
#include "application.h"

#include <unordered_map>
#include <list>
#include <vector>
#include <set>

#include "simulation_stats.h"
#include "FSBAllocator.hh"

using std::unordered_map;
using std::pair;
using std::make_pair;
using std::list;
using std::vector;
using std::set;

using std::cout;
using std::endl;
using std::lower_bound;
using std::upper_bound;


static const string INIT_PHASE_K = "init";
static const string SIMULATE_PHASE_K = "simulate";
static const string FINALIZE_PHASE_K = "finalize";

template < class ES >
class SequentialSimulationManager : public SimulationManager< ES > {
public:
    typedef pair< object *, event::vtime_t > pair_object_timestamp;
//    typedef list< pair_object_timestamp, FSBAllocator< pair_object_timestamp > > ordered_object_exe_t;
//    typedef pair< object *, ordered_object_exe_t::iterator > pair_object_handle_t;
//    typedef vector< pair_object_handle_t > object_handle_map_t;

    typedef vector< pair_object_timestamp > object_handle_map_t;
    typedef pair< event::vtime_t, system_id > pair_timestamp_object;

    struct timestamp_object_comp {
        bool operator()( const pair_timestamp_object lhs, const pair_timestamp_object rhs ) const {
            return ((lhs.first < rhs.first) || (lhs.first == rhs.first && lhs.second < rhs.second));
        }
    };

    typedef set< pair_timestamp_object, timestamp_object_comp  > ordered_object_exe_t;
    typedef typename ordered_object_exe_t::iterator _iterator;

    SequentialSimulationManager( application *, system_id::manager_id_t id = 0 );
    SequentialSimulationManager( application *, shared_ptr< SimulationStats >,  system_id::manager_id_t id = 0 );

    virtual const event::vtime_t & getSimulationTime() const;
    virtual bool  isSimulationComplete() const;

    virtual const system_id getNextObjectID();

    virtual void registerObject( object * obj );
    virtual void unregisterObject( object * obj );

    virtual size_t getObjectCount() const;
    virtual object * getObject( const system_id & id ) const;

    virtual void initialize();
    virtual void simulate( const event::vtime_t & until );
    virtual void finalize();

    virtual void routeEvent( const event * evt );
    virtual void notifyNextEvent( const system_id & obj, const event::vtime_t & t );

    virtual ~SequentialSimulationManager();
protected:

    virtual pair_object_timestamp getNextObject();

    virtual bool setSimulationTime( const event::vtime_t & t );
    virtual void setSimulateUntil( const event::vtime_t & t );

private:

    application *   m_app;

    event::vtime_t    m_sim_time;
    event::vtime_t    m_sim_until;
    bool m_sim_complete;

    object_handle_map_t m_objects;
    ordered_object_exe_t m_ordered_objs;

    unsigned int    m_nPendingEvents, m_nProcessedEvents;
    unsigned int    m_nRegisteredObjs;

    shared_ptr< SimulationStats > m_stats;

    struct object_timestamp_comp {
        bool operator()( const SequentialSimulationManager::pair_object_timestamp & lhs, const SequentialSimulationManager::pair_object_timestamp & rhs ) const {
            return lhs.second < rhs.second;
        }
    };
    object_timestamp_comp m_ot_comp;
};

//
// IMPLEMENTATION
//

template< class ES >
SequentialSimulationManager<ES>::SequentialSimulationManager( application * app, system_id::manager_id_t id ) :
    SimulationManager< ES >( id ),
    m_app(app),
    m_sim_time( SystemClock::ZERO ),
    m_sim_until( SystemClock::POSITIVE_INFINITY ),
    m_sim_complete(false),
    m_nPendingEvents(0),
    m_nProcessedEvents(0),
    m_nRegisteredObjs(0),
    m_stats( new SimulationStats() )
{}

template< class ES >
SequentialSimulationManager<ES>::SequentialSimulationManager( application * app, shared_ptr< SimulationStats > stats, system_id::manager_id_t id ) :
    SimulationManager<ES>( id ),
    m_app(app),
    m_sim_time( SystemClock::ZERO ),
    m_sim_until( SystemClock::POSITIVE_INFINITY ),
    m_sim_complete(false),
    m_nPendingEvents(0),
    m_nProcessedEvents(0),
    m_nRegisteredObjs(0),
    m_stats( stats )
{

}

template< class ES >
SequentialSimulationManager<ES>::~SequentialSimulationManager() {
    while( !m_objects.empty() ) {
        object * tmp = m_objects.back().first;
        m_objects.pop_back();
        if( tmp ) {
            cout << tmp->getSystemID() << " was never unregistered" << endl;
            delete tmp;
        }
    }

//    while( !m_ordered_objs.empty() ) {
//        m_ordered_objs.pop_back();
//    }
    m_ordered_objs.clear();
}

template< class ES >
const system_id SequentialSimulationManager<ES>::getNextObjectID() {
    system_id nid( this->getManagerID(), m_objects.size() );
    m_objects.push_back( make_pair((object *)NULL, SystemClock::POSITIVE_INFINITY) );
    return nid;
}

template< class ES >
void SequentialSimulationManager<ES>::registerObject( object * obj ) {
    if( obj == NULL ) return;

    assert( obj->getSystemID() != this->m_id );

    if( m_objects[ obj->getObjectID() ].first == NULL ) {
        //cout << "Registering: " << obj->getSystemID() << endl;
//        pair_object_timestamp ot = make_pair( obj->getSystemID(), SystemClock::POSITIVE_INFINITY );

        m_objects[ obj->getObjectID() ].first = obj;
//        m_objects[ obj->getObjectID() ].second = m_ordered_objs.insert( m_ordered_objs.end(), ot );
        ++m_nRegisteredObjs;
    } else {
        pair_timestamp_object to = make_pair( m_objects[ obj->getObjectID() ].second, obj->getSystemID() );
        _iterator it = m_ordered_objs.find( to );
        if( it != m_ordered_objs.end() ) {
            m_ordered_objs.erase( it );
        }

        m_objects[ obj->getObjectID() ].second = SystemClock::POSITIVE_INFINITY;
    }
}

template< class ES >
void SequentialSimulationManager<ES>::unregisterObject( object * obj ) {
    if( obj == NULL ) return;

    if( m_objects[ obj->getObjectID() ].first != NULL ) {
        //cout << "Unregistering: " << obj->getSystemID() << endl;
        m_nPendingEvents += obj->pendingEventCount(obj->getSystemID());
        m_nProcessedEvents += obj->processedEventCount(obj->getSystemID());

        if( m_objects[ obj->getObjectID() ].second != SystemClock::POSITIVE_INFINITY ) {
            pair_timestamp_object to = make_pair( m_objects[ obj->getObjectID() ].second, obj->getSystemID());
            _iterator it = m_ordered_objs.find( to );
            assert( it != m_ordered_objs.end() );

            m_ordered_objs.erase( it );
        }

        m_objects[ obj->getObjectID() ].first = NULL;
        m_objects[ obj->getObjectID() ].second = SystemClock::POSITIVE_INFINITY;
        --m_nRegisteredObjs;
    }
}

template< class ES >
size_t SequentialSimulationManager<ES>::getObjectCount() const {
//    return m_objects.size();
    return m_nRegisteredObjs;
}

template< class ES >
object * SequentialSimulationManager<ES>::getObject( const system_id & id ) const {
    return m_objects[ id.getObjectID() ].first;
}

template< class ES >
void SequentialSimulationManager<ES>::routeEvent( const event * evt ) {
    assert( m_objects.size() > evt->getReceiver().getObjectID() );
    m_objects[ evt->getReceiver().getObjectID() ].first->receiveEvent( evt );
}

template< class ES >
void SequentialSimulationManager<ES>::notifyNextEvent( const system_id & obj, const event::vtime_t & t ) {
    if( m_objects[ obj.getObjectID() ].second != SystemClock::POSITIVE_INFINITY ) {
        pair_timestamp_object f = make_pair( m_objects[ obj.getObjectID() ].second, obj );

        _iterator it = m_ordered_objs.find( f );

        assert( it != m_ordered_objs.end() );

        m_ordered_objs.erase( it );
    }

    pair_timestamp_object to = make_pair( t, obj );
    m_ordered_objs.insert( to );
    m_objects[ obj.getObjectID() ].second = t;
//    pair_object_timestamp ot = make_pair( obj, t );

//    ordered_object_exe_t::iterator it = m_objects[ obj.getObjectID() ].second;

//    ordered_object_exe_t::iterator pos = upper_bound( m_ordered_objs.begin(), it, ot, m_ot_comp );

//    if( it == m_ordered_objs.end() ) {
//        it = m_ordered_objs.insert( pos, ot );
//    } else if( it != pos ) {
//        m_ordered_objs.splice( pos, m_ordered_objs, it );
//        it->second = t;
//    } else if( pos->first == obj ) {
//        it->second = t;
//    } else {
//        assert( false );
//    }
}

template< class ES >
typename SequentialSimulationManager<ES>::pair_object_timestamp SequentialSimulationManager<ES>::getNextObject() {
//    pair_object_timestamp ot = m_ordered_objs.front();

    // rotate the head node to the end of the list
    // and set its time to be POSITIVE_INFINITY
//    m_ordered_objs.splice( m_ordered_objs.end(), m_ordered_objs, m_ordered_objs.begin() );
//    m_ordered_objs.back().second = SystemClock::POSITIVE_INFINITY;

    pair_object_timestamp ot = m_objects[ m_ordered_objs.begin()->second.getObjectID() ];
    m_ordered_objs.erase( m_ordered_objs.begin() );

    m_objects[ ot.first->getObjectID()].second = SystemClock::POSITIVE_INFINITY;
    return ot;
}

template< class ES >
void SequentialSimulationManager<ES>::initialize() {
    m_stats->startPhase( INIT_PHASE_K );
    m_app->initialize();
    m_stats->stopPhase( INIT_PHASE_K );
}

template< class ES >
void SequentialSimulationManager<ES>::simulate( const event::vtime_t & until ) {
    setSimulateUntil( until );

    m_stats->startPhase( SIMULATE_PHASE_K );
    while(! m_ordered_objs.empty() ) {
        pair_object_timestamp ot = getNextObject();

        if( ot.second == SystemClock::POSITIVE_INFINITY || setSimulationTime( ot.second ) ) {
            break;
        }

        ot.first->updateLocalTime( ot.second );
        ot.first->process();
    }

    m_stats->stopPhase( SIMULATE_PHASE_K );
}

template< class ES >
void SequentialSimulationManager<ES>::finalize() {
    m_stats->startPhase( FINALIZE_PHASE_K );

    m_app->finalize();

    m_stats->stopPhase( FINALIZE_PHASE_K );

    m_stats->setProcessedEvents( m_nProcessedEvents );
    m_stats->setPendingEvents( m_nPendingEvents );

}

template< class ES >
bool SequentialSimulationManager<ES>::isSimulationComplete() const {
    return m_sim_complete;
}

template< class ES >
bool SequentialSimulationManager<ES>::setSimulationTime( const event::vtime_t & t ) {
    assert( m_sim_time <= t );
    if( m_sim_time != t ) {
        m_sim_time = t;
        m_sim_complete = (m_sim_time >= m_sim_until);
    }

    return m_sim_complete;
}

template< class ES >
void SequentialSimulationManager<ES>::setSimulateUntil( const event::vtime_t & t ) {
    m_sim_until = t;
}

template< class ES >
const event::vtime_t & SequentialSimulationManager<ES>::getSimulationTime() const {
    return m_sim_time;
}
#endif  // SEQUENTIALSIMULATIONMANAGER_H_
