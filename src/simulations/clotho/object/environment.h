#ifndef ENVIRONMENT_H_
#define ENVIRONMENT_H_

#include "../clotho.h"
#include "../clotho_object.h"
//#include "engine/simulation_object.h"

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

using std::deque;
using std::vector;
using std::set;
using std::unordered_map;
using std::unordered_set;


//class Environment : public SimulationObject< ClothoEventSet > {
class Environment : public ClothoObject {
public:
    friend class EventPerformer< Environment, ClothoEvent >;

    typedef vector< system_id > individual_group_t;
    typedef pair< individual_group_t *, size_t > pair_individual_group_offset;
    typedef unordered_map< system_id, pair_individual_group_offset > individual_group_lookup_t;
    typedef typename individual_group_lookup_t::iterator        lookup_iterator;
    typedef typename individual_group_lookup_t::const_iterator  lookup_citerator;
    

    Environment( simulation_manager * manager );

    Environment( simulation_manager * manager, GeneticMap::Ptr gmap, selection_model * s, reproduction * r );

    virtual void initialize();
 
    virtual void perform_event( const event * );

    virtual void finalize();

    system_id getIndividual();

    virtual ~Environment() {}

protected:

    void handle_birth( const ClothoEvent * ce );
    void handle_maturity( const ClothoEvent * ce );
    void handle_death( const ClothoEvent * ce );

//    void addIndividual( const system_id & id );
//    void removeIndividual( const system_id & id );
    
    individual_group_t m_males, m_females;
    individual_group_lookup_t m_active_individuals;
    deque< system_id > m_available_individuals;
    set< system_id > m_pending; 

    GeneticMap::Ptr     m_genetic_map;
    selection_model *   m_selection_model;
    reproduction *      m_reproduction_model;

    unsigned int        m_nIndAlloc;

    static EventPerformer< Environment, ClothoEvent > m_evt_performer;
};

#endif  // ENVIRONMENT_H_
