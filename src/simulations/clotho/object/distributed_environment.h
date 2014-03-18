#ifndef DISTRIBUTED_ENVIRONMENT_H_
#define DISTRIBUTED_ENVIRONMENT_H_

#include "../clotho.h"
#include "../clotho_object.h"
//#include "engine/simulation_object.h"
//
#include "environment.h"
#include "environment_snapshot.h"

#include "GeneticMap.h"

#include <deque>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <set>

#include "../event/clotho_event.h"
#include "../event/maturity_event.h"
#include "../event_performer.h"

#include "../models/selection_model.h"
#include "reproduction.h"

using std::deque;
using std::vector;
using std::set;
using std::unordered_map;
using std::unordered_set;

class DistributedEnvironment : public Environment {
public:
    typedef vector< pair< system_id, EnvironmentSnapshot > > neighborhood_t;
    DistributedEnvironment( simulation_manager * manager, const system_id & global_env );

    DistributedEnvironment( simulation_manager * manager, const system_id & global_env, GeneticMap::Ptr gmap, selection_model * s, reproduction * r );

    virtual void process();

    virtual void perform_event( const event * );

    virtual void addNeighborEnvironment( const system_id & env );
 
    virtual ~DistributedEnvironment() {}

protected:

    virtual void handle_birth( const ClothoEvent * ce );
    virtual void handle_maturity( const ClothoEvent * ce );
    virtual void handle_signal_mate( const ClothoEvent * ce );
    virtual void handle_snapshot( const ClothoEvent * ce );

    virtual void determineMate( const MaturityEvent * me );

    void updateSnapShot();

    system_id   m_global_env;
    neighborhood_t m_neighbors;

    EnvironmentSnapshot m_snapshot;
};

#endif  // DISTRIBUTED_ENVIRONMENT_H_
