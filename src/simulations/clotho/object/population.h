#ifndef POPULATION_H_
#define POPULATION_H_

#include "../clotho.h"
#include "engine/simulation_object.h"

#include <vector>
#include <unordered_map>

using std::vector;
using std::unordered_map;

class Population : public SimulationObject< ClothoEventSet > {
public:
    typedef std::vector< const system_id > grouped_by_sex_t;
    typedef std::unordered_map< const system_id, pair< population_by_sex_t *, size_t > > id_to_sex_map_t;

    Population( simulation_manager * manager ) :
        SimulationObject(manager)
    {
        setSimulationManager( manager );
    }

    virtual void perform_event( const event * );

    virtual ~Population() {}
protected:
    grouped_by_sex_t    m_males, m_females, m_unk;
    id_to_sex_map_t     m_individuals;
};

#endif  // POPULATION_H_
