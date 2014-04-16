#ifndef DISTRIBUTED_ENVIRONMENT_HPP_
#define DISTRIBUTED_ENVIRONMENT_HPP_

#include "../clotho.h"
#include "../clotho_object.h"

#include "environment.hpp"
#include "environment_snapshot.h"

#include "GeneticMap.h"

#include <deque>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <set>

#include "../event/clotho_event.h"
#include "../event_performer.h"

#include "../models/selection_model.h"
#include "reproduction.h"

#include "rng/rng.hpp"

#include "../models/life_cycle_model.h"
#include <type_traits>

using std::deque;
using std::vector;
using std::set;
using std::unordered_map;
using std::unordered_set;

template < class LCM, class Enabled = void >
class TDistributedEnvironment;

template < class LCM >
class TDistributedEnvironment < LCM, typename std::enable_if< std::is_base_of< life_cycle_model, LCM >::value >::type >
    : public TEnvironment<LCM> {
public:
    typedef vector< pair< system_id, EnvironmentSnapshot > > neighborhood_t;

    TDistributedEnvironment( simulation_manager * manager, const system_id & global_env ) :
        TEnvironment<LCM>( manager ),
        m_global_env( global_env ),
        m_snapshot(),
        m_view_threshold( 0.5 )
    {
        ClothoObject::setSimulationManager( manager );
        if( m_global_env == DefaultSystemID ) {
            m_global_env = ClothoObject::getSystemID();
            m_neighbors.push_back( make_pair( ClothoObject::getSystemID(), m_snapshot ) );  // root is its own neighbor
        }
    }

    TDistributedEnvironment( simulation_manager * manager, const system_id & global_env, GeneticMap::Ptr gmap, selection_model * s, reproduction * r, shared_ptr< iRNG > rng, double threshold ) :
        TEnvironment<LCM>( manager, gmap, s, r ),
        m_global_env( global_env ),
        m_snapshot(),
        m_rng( rng->clone() ),
        m_view_threshold( threshold )
    {
        ClothoObject::setSimulationManager( manager );
        if( m_global_env == DefaultSystemID ) {
            m_global_env = ClothoObject::getSystemID();
            m_neighbors.push_back( make_pair( ClothoObject::getSystemID(), m_snapshot ) );  // root is its own neighbor
        }
    }

    virtual void process() { assert(false); }

    virtual void perform_event( const event * ) { assert(false); }

    virtual void addNeighborEnvironment( const system_id & env_id ) {
        bool bDuplicate = false;
        for( neighborhood_t::iterator it = m_neighbors.begin(); it != m_neighbors.end(); it++ ) {
            if( (*it).first == env_id ) {
                bDuplicate = true;
                break;
            }
        }

        if( !bDuplicate ) {
            m_neighbors.push_back( make_pair( env_id, EnvironmentSnapshot() ) );

            cout << ClothoObject::getSystemID() << " -> " << env_id << endl;
        }
    }
 
    virtual ~TDistributedEnvironment() {}

protected:

    virtual void handle_birth( const ClothoEvent * ce );
    virtual void handle_maturity( const ClothoEvent * ce );
    virtual void handle_signal_mate( const ClothoEvent * ce );
    virtual void handle_snapshot( const ClothoEvent * ce );
    virtual void handle_selection( const ClothoEvent * ce );

    void updateSnapShot() {
        m_snapshot.male_count = TEnvironment<LCM>::m_males.size();
        m_snapshot.female_count = TEnvironment<LCM>::m_females.size();
        m_snapshot.pending_count = TEnvironment<LCM>::m_pending.size();
        m_snapshot.available_count = ((TEnvironment<LCM>::m_available_individuals.empty())? 0: TEnvironment<LCM>::m_available_individuals.size());
    }

    system_id   m_global_env;
    neighborhood_t m_neighbors;

    EnvironmentSnapshot m_snapshot;
    shared_ptr< iRNG >  m_rng;
    double              m_view_threshold;
};

#endif  // DISTRIBUTED_ENVIRONMENT_H_
