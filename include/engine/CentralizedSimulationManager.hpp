#ifndef CENTRALIZEDSIMULATIONMANAGER_HPP_
#define CENTRALIZEDSIMULATIONMANAGER_HPP_

#include "simulation_manager_impl.h"
#include "application.h"

#include "simulation_stats.h"
#include <map>

using std::map;
using std::pair;
using std::make_pair;
using std::swap;

template < class ES >
class CentralizedSimulationManager : public SimulationManager< ES > {
public:
    typedef pair< event::vtime_t, size_t > pair_time_offset;
    typedef pair< object *, pair_time_offset > pair_object_timestamp;
    typedef vector< pair_object_timestamp > object_handle_map_t;
/*
    struct object_timestamp_comp {
        bool operator()( const CentralizedSimulationManager::pair_object_timestamp & lhs, const CentralizedSimulationManager::pair_object_timestamp & rhs ) const {
            return (lhs.second < rhs.second) || ( lhs.second == rhs.second && lhs.first->getSystemID() < rhs.first->getSystemID() );
        }
    };
*/
    typedef vector< object * > object_group_t;
    typedef pair< event::vtime_t, object_group_t * > concurrent_group_t;
    typedef map< event::vtime_t, object_group_t * > ordered_object_exe_t;
    typedef typename ordered_object_exe_t::iterator _iterator;

    CentralizedSimulationManager( shared_ptr< application >, system_id::manager_id_t id = 0 );
    CentralizedSimulationManager( shared_ptr< application >, shared_ptr< SimulationStats >,  system_id::manager_id_t id = 0 );

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

    virtual ~CentralizedSimulationManager();
protected:

    virtual concurrent_group_t getNextObjectGroup();

    virtual bool setSimulationTime( const event::vtime_t & t );
    virtual void setSimulateUntil( const event::vtime_t & t );

protected:
    void moveObject( object * obj, event::vtime_t t );

private:
    shared_ptr< application >   m_app;

    event::vtime_t    m_sim_time;
    event::vtime_t    m_sim_until;
    bool m_sim_complete;

    object_handle_map_t m_objects;
    ordered_object_exe_t m_ordered_objs;

    unsigned int    m_nPendingEvents, m_nProcessedEvents;
    unsigned int    m_nRegisteredObjs;

    shared_ptr< SimulationStats > m_stats;
};

//
// Implementation
//

template < class ES >
CentralizedSimulationManager<ES>::CentralizedSimulationManager( shared_ptr< application > app, system_id::manager_id_t id ) :
    SimulationManager<ES>(id),
    m_app( app ),
    m_sim_time( SystemClock::ZERO ),
    m_sim_until( SystemClock::POSITIVE_INFINITY ),
    m_nPendingEvents(0),
    m_nProcessedEvents(0),
    m_nRegisteredObjs(0),
    m_stats( new SimulationStats() )
{}

template < class ES >
CentralizedSimulationManager<ES>::CentralizedSimulationManager( shared_ptr< application > app, shared_ptr< SimulationStats > stats, system_id::manager_id_t id ) :
    SimulationManager<ES>(id),
    m_app( app ),
    m_sim_time( SystemClock::ZERO ),
    m_sim_until( SystemClock::POSITIVE_INFINITY ),
    m_nPendingEvents(0),
    m_nProcessedEvents(0),
    m_nRegisteredObjs(0),
    m_stats( stats )
{}

template < class ES >
CentralizedSimulationManager<ES>::~CentralizedSimulationManager() {
    unsigned int nUnfinalized = 0;
    while( !m_objects.empty() ) {
        object * tmp = m_objects.back().first;
        m_objects.pop_back();
        if( tmp ) {
            ++nUnfinalized;
            tmp->finalize();
            delete tmp;
        }
    }
    cout << nUnfinalized << " objects were finalized AFTER simulation finalization." << endl;
    m_ordered_objs.clear();
}

template < class ES >
const system_id CentralizedSimulationManager<ES>::getNextObjectID() {
    system_id nid( this->getManagerID(), m_objects.size() );
    m_objects.push_back( make_pair( (object *)NULL, make_pair(SystemClock::POSITIVE_INFINITY, 0 ) ) );
    return nid;
}

template< class ES >
void CentralizedSimulationManager<ES>::registerObject( object * obj ) {
    if( obj == NULL ) return;

    assert( obj->getSystemID() != this->m_id );

    if( m_objects[ obj->getObjectID() ].first == NULL ) {
        m_objects[ obj->getObjectID() ].first = obj;
        ++m_nRegisteredObjs;
    } else {
        moveObject( obj, SystemClock::POSITIVE_INFINITY );
    }
}

template< class ES >
void CentralizedSimulationManager<ES>::unregisterObject( object * obj ) {
    if( obj == NULL ) return;

    if( m_objects[ obj->getObjectID() ].first != NULL ) {
        m_nPendingEvents += obj->pendingEventCount(obj->getSystemID());
        m_nProcessedEvents += obj->processedEventCount(obj->getSystemID());

        if( m_objects[ obj->getObjectID() ].second.first != SystemClock::POSITIVE_INFINITY ) {
            moveObject( obj, SystemClock::POSITIVE_INFINITY );
        }

        m_objects[ obj->getObjectID() ].first = NULL;
        --m_nRegisteredObjs;
    }
}

template < class ES >
void CentralizedSimulationManager<ES>::moveObject( object * obj, event::vtime_t newT ) {
    pair_time_offset pto = m_objects[ obj->getObjectID() ].second;

    if( pto.first != SystemClock::POSITIVE_INFINITY ) {
        // objects with time set to POSITIVE_INFINITY are not "queued"
        _iterator it = m_ordered_objs.find( pto.first );

        if( it != m_ordered_objs.end() ) {
            object_group_t * g = it->second;

            assert( pto.second < g->size() &&  g->at( pto.second ) == obj );

            if( g->back() != obj ) {
                // object is not at the end of vector
                // swap with object at the end of the vector
                // and update that objects offset value to
                // be that of the current obj 
                system_id other = g->back()->getSystemID();

                (*g)[ pto.second ] = g->back();
                g->back() = obj;

                assert( g->back() == obj );

                m_objects[ other.getObjectID() ].second.second = pto.second;
            }
            g->pop_back();
        }
    }

    m_objects[ obj->getObjectID() ].second.first = newT;
    if( newT != SystemClock::POSITIVE_INFINITY ) {
        _iterator it = m_ordered_objs.find( newT );
        if( it == m_ordered_objs.end() ) {
            // no other objects have events scheduled at newT
            pair< _iterator, bool > res = m_ordered_objs.insert( make_pair(newT, new object_group_t() ) );
            assert( res.second );
            it = res.first;
        }

        m_objects[ obj->getObjectID() ].second.second = it->second->size();
        it->second->push_back( obj );
    }
}

template< class ES >
size_t CentralizedSimulationManager<ES>::getObjectCount() const {
    return m_nRegisteredObjs;
}


template< class ES >
object * CentralizedSimulationManager<ES>::getObject( const system_id & id ) const {
    return m_objects[ id.getObjectID() ].first;
}

template< class ES >
void CentralizedSimulationManager<ES>::routeEvent( const event * evt ) {
    assert( m_objects.size() > evt->getReceiver().getObjectID() );
    m_objects[ evt->getReceiver().getObjectID() ].first->receiveEvent( evt );
}

template< class ES >
void CentralizedSimulationManager<ES>::notifyNextEvent( const system_id & obj, const event::vtime_t & t ) {
    moveObject( m_objects[ obj.getObjectID() ].first, t );
}

template< class ES >
typename CentralizedSimulationManager<ES>::concurrent_group_t CentralizedSimulationManager<ES>::getNextObjectGroup() {
    concurrent_group_t ot = *m_ordered_objs.begin();
    m_ordered_objs.erase( m_ordered_objs.begin() );

    return ot;
}

template< class ES >
void CentralizedSimulationManager<ES>::initialize() {
    m_stats->startPhase( INIT_PHASE_K );
    m_app->initialize();
    m_stats->stopPhase( INIT_PHASE_K );
}

template< class ES >
void CentralizedSimulationManager<ES>::simulate( const event::vtime_t & until ) {
    setSimulateUntil( until );

    m_stats->startPhase( SIMULATE_PHASE_K );
    while(! m_ordered_objs.empty() ) {
        concurrent_group_t ot = getNextObjectGroup();

        if( ot.first == SystemClock::POSITIVE_INFINITY || setSimulationTime( ot.first ) ) {
            break;
        }

        for( object_group_t::iterator it = ot.second->begin(); it != ot.second->end(); it++ ) {
            object * obj = (*it);
            m_objects[ obj->getObjectID() ].second.first = SystemClock::POSITIVE_INFINITY;
            obj->updateLocalTime(ot.first);
            obj->process();

            const event * e = obj->peekEvent( obj->getSystemID() );
            if( e != NULL ) {
                moveObject( obj, e->getReceived() );
            } else {
                moveObject( obj, SystemClock::POSITIVE_INFINITY );
            }
        }

        ot.second->clear();
        delete ot.second;
    }

    m_stats->stopPhase( SIMULATE_PHASE_K );

    cout << "End simulation time: " <<  getSimulationTime() << endl;
}

template< class ES >
void CentralizedSimulationManager<ES>::finalize() {
    m_stats->startPhase( FINALIZE_PHASE_K );

    m_app->finalize();

    m_stats->stopPhase( FINALIZE_PHASE_K );

    m_stats->setProcessedEvents( m_nProcessedEvents );
    m_stats->setPendingEvents( m_nPendingEvents );

}

template< class ES >
bool CentralizedSimulationManager<ES>::isSimulationComplete() const {
    return m_sim_complete;
}

template< class ES >
bool CentralizedSimulationManager<ES>::setSimulationTime( const event::vtime_t & t ) {
    assert( m_sim_time <= t );
    if( m_sim_time != t ) {
        m_sim_time = t;
        m_sim_complete = (m_sim_time >= m_sim_until);
    }

    return m_sim_complete;
}

template< class ES >
void CentralizedSimulationManager<ES>::setSimulateUntil( const event::vtime_t & t ) {
    m_sim_until = t;
}

template< class ES >
const event::vtime_t & CentralizedSimulationManager<ES>::getSimulationTime() const {
    return m_sim_time;
}
#endif  // CENTRALIZEDSIMULATIONMANAGER_HPP_
