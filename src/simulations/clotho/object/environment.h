#ifndef ENVIRONMENT_H_
#define ENVIRONMENT_H_

#include "../clotho.h"
#include "../clotho_object.h"
//#include "engine/simulation_object.h"

#include "GeneticMap.h"

#include <list>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "../event/clotho_event.h"
#include "../event_performer.h"

#include "reproduction.h"

using std::list;
using std::vector;
using std::unordered_map;
using std::unordered_set;


//class Environment : public SimulationObject< ClothoEventSet > {
class Environment : public ClothoObject {
public:
//    typedef unordered_map< const system_id, Population * > id_to_population_map_t;
    friend class EventPerformer< Environment, ClothoEvent >;

    Environment( simulation_manager * manager );

    Environment( simulation_manager * manager, GeneticMap::Ptr gmap, reproduction * r );

    virtual void initialize();
 
    virtual void perform_event( const event * );

    virtual void finalize();

    system_id getIndividual();

    virtual ~Environment() {}

protected:

    void handle_birth( const ClothoEvent * ce );
    void handle_death( const ClothoEvent * ce );

//    void addIndividual( const system_id & id );
//    void removeIndividual( const system_id & id );

    unordered_set< system_id > m_active_individuals;
    list< system_id > m_available_individuals;

    GeneticMap::Ptr     m_genetic_map;
    reproduction *      m_reproduction_model;

    static EventPerformer< Environment, ClothoEvent > m_evt_performer;
};

#endif  // ENVIRONMENT_H_
