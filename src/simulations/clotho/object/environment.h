#ifndef ENVIRONMENT_H_
#define ENVIRONMENT_H_

#include "../clotho.h"
#include "engine/simulation_object.h"

#include <list>
#include <vector>
#include <unordered_map>

using std::list;
using std::vector;
using std::unordered_map;


class Environment : public SimulationObject< ClothoEventSet > {
public:
    typedef unordered_map< const system_id, Population * > id_to_population_map_t;

    Environment( simulation_manager * manager ) :
        SimulationObject( manager )
    {
        setSimulationManager( manager );
    }

    virtual void perform_event( const event * );

    virtual ~Environment() {}

protected:
    system_id getIndividual();

    void addIndividual();
    void removeIndividual();

    list< system_id > m_available_individuals;
};

#endif  // ENVIRONMENT_H_
