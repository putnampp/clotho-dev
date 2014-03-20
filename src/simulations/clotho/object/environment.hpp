#ifndef ENVIRONMENT_HPP_
#define ENVIRONMENT_HPP_

#include "../clotho.h"
#include "../clotho_object.h"
#include "../event/clotho_event.h"
#include "../models/selection_model.h"
#include "../models/life_cycle_model.h"

#include "GeneticMap.h"     // from libcchmc
#include "reproduction.h"   // from libcchmc

#include <deque>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <type_traits>

using std::deque;
using std::vector;
using std::set;
using std::unordered_map;
using std::unordered_set;

template < class LCM, class Enable = void >
class TEnvironment;

template < class LCM >
class TEnvironment < LCM, typename std::enable_if< std::is_base_of< life_cycle_model, LCM >::value >::type >
    : public ClothoObject {
public:
    typedef vector< system_id > individual_group_t;
    typedef pair< individual_group_t *, size_t > pair_individual_group_offset;
    typedef unordered_map< system_id, pair_individual_group_offset > individual_group_lookup_t;
    typedef typename individual_group_lookup_t::iterator        lookup_iterator;
    typedef typename individual_group_lookup_t::const_iterator  lookup_citerator;

    TEnvironment( simulation_manager * manager ) :
        ClothoObject( manager ),
        m_genetic_map( new GeneticMap() ),
        m_selection_model( NULL ),
        m_reproduction_model( NULL ),
        m_nIndAlloc(0),
        m_nFounder(0)
    {
        ClothoObject::setSimulationManager( manager );
    }

    TEnvironment( simulation_manager * manager, GeneticMap::Ptr gmap, selection_model * s, reproduction * r ) :
        ClothoObject( manager ),
        m_genetic_map( gmap ),
        m_selection_model( s ),
        m_reproduction_model( r ),
        m_nIndAlloc(0),
        m_nFounder(0)
    {
        ClothoObject::setSimulationManager( manager );
    }

    void setFounderSize( unsigned int s ) {
        m_nFounder = s;
    }

    virtual void initialize() { assert(false); }
 
    virtual void perform_event( const event * ) { assert(false); }

    virtual void finalize() { assert(false); }

    system_id getIndividual() { assert(false); }

    virtual ~TEnvironment() {}

protected:

    virtual void handle_birth( const ClothoEvent * ce );
    virtual void handle_maturity( const ClothoEvent * ce );
    virtual void handle_death( const ClothoEvent * ce );

    individual_group_t m_males, m_females;
    individual_group_lookup_t m_active_individuals;
    deque< system_id > m_available_individuals;
    set< system_id > m_pending; 

    GeneticMap::Ptr     m_genetic_map;
    selection_model *   m_selection_model;
    reproduction *      m_reproduction_model;

    unsigned int        m_nIndAlloc;
    unsigned int        m_nFounder;
};

#endif  // ENVIRONMENT_HPP_
