#ifndef SEQUENTIALSIMULATIONMANAGER_H_
#define SEQUENTIALSIMULATIONMANAGER_H_

//#include "simulation_manager.h"
#include "simulation_manager_impl.h"
#include "application.h"

#include <unordered_map>
#include <list>

#include "simulation_stats.h"

using std::unordered_map;
using std::pair;
using std::make_pair;
using std::list;

class SequentialSimulationManager : public SimulationManager {
public:
    typedef unordered_map< system_id, object * > object_handle_map_t;
    typedef pair< system_id, event::vtime_t > pair_object_timestamp;
    typedef list< pair_object_timestamp > ordered_object_exe_t;
    typedef unordered_map< system_id, ordered_object_exe_t::iterator > object_next_event_map_t;

    SequentialSimulationManager( application *, system_id::manager_id_t id = 0 );
    SequentialSimulationManager( application *, shared_ptr< SimulationStats >,  system_id::manager_id_t id = 0 );

    virtual const event::vtime_t & getSimulationTime() const;
    virtual bool  isSimulationComplete() const;

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
    object_next_event_map_t m_objects_next;

    unsigned int    m_nPendingEvents, m_nProcessedEvents;

    shared_ptr< SimulationStats > m_stats;

    struct object_timestamp_comp {
        bool operator()( const SequentialSimulationManager::pair_object_timestamp & lhs, const SequentialSimulationManager::pair_object_timestamp & rhs ) const {
            return lhs.second < rhs.second;
        }
    };
};

#endif  // SEQUENTIALSIMULATIONMANAGER_H_
