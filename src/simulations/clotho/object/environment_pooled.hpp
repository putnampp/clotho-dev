#ifndef ENVIRONMENTPOOLED_HPP_
#define ENVIRONMENTPOOLED_HPP_

#include "../clotho.h"
#include "../clotho_object.hpp"
#include "../clotho_event.h"
#include "../models/selection_model.h"
#include "../models/life_cycle_model.h"

#include "environment_initializer.hpp"
#include "environment_life_cycle.hpp"
#include "environment_finalizer.hpp"

#include "variant_map.hpp"

#include "selection.hpp"
#include "reproduction.hpp"   // from libcchmc

#include <deque>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <type_traits>
#include <functional>

#include "../active_pool.hpp"
#include "../app_object.hpp"

#include "individual2.hpp"

using std::deque;
using std::vector;
using std::set;
using std::unordered_map;
using std::unordered_set;
using std::swap;


template < class LCM, class IND, class SMODEL, class Enable = void >
class TEnvironment;

template < class LCM, class IND, class SMODEL >
class TEnvironment < LCM, IND, SMODEL, typename std::enable_if< std::is_base_of< life_cycle::life_cycle_model, LCM >::value >::type >
    : public AppObject
/*    : public ClothoObject*/ {
public:
//    typedef typename ClothoObject::simulation_manager_t simulation_manager_t;

//    typedef vector< system_id > individual_group_t;
//    typedef std::pair< individual_group_t *, size_t > pair_individual_group_offset;
//    typedef unordered_map< system_id, pair_individual_group_offset > individual_group_lookup_t;
//    typedef unordered_map< unsigned char, individual_group_t * > gender_group_map_t;
//    typedef typename individual_group_lookup_t::iterator        lookup_iterator;
//    typedef typename individual_group_lookup_t::const_iterator  lookup_citerator;
//

//    typedef variant_map< VT >       variant_map_t;
    typedef IND individual_t;
    typedef individual_t *  individual_ptr_t;
    typedef typename SMODEL::parameter_t   selection_parameter_t;

    typedef active_pool< size_t, void > active_pool_t;

    typedef selection::MateSelector< SMODEL > selection_model_t;
    typedef TEnvironment < LCM, IND, SMODEL > environment_t;

    typedef ClothoObject::event_t event_t;

    friend class initializer::EnvironmentInitializer;
    friend class finalizer::EnvironmentFinalizer;
    friend class life_cycle::EnvironmentLifeCycle<LCM, environment_t, ClothoEvent>;

    friend selection_model_t;

//    TEnvironment( simulation_manager_t * manager, shared_ptr< iRNG > r ) :
//        ClothoObject( manager ),
//        m_rng(r),
//        //m_variants( new variant_map_t(r) ),
//        m_sel_param( NULL ),
//        m_nIndAlloc(0),
//        m_nFounder(0)/*,
//        m_nMateOps(0)*/
//    {
//        ClothoObject::setSimulationManager( manager );
//    }

//    TEnvironment( ClothoObject * co, shared_ptr< iRNG > r ) :
//        m_clotho_object(co),
//        m_rng(r),
//        m_sel_param(NULL),
//        m_nFounder(0)
//    {}

//    TEnvironment( simulation_manager_t * manager, shared_ptr< iRNG > r,/* std::shared_ptr< variant_map_t > vmap,*/ selection_parameter_t * s ) :
//        ClothoObject( manager ),
//        m_rng(r),
//        m_variants( vmap ),
//        m_sel_param( s ),
//        m_nIndAlloc(0),
//        m_nFounder(0)/*,
//        m_nMateOps(0)*/
//    {
//        ClothoObject::setSimulationManager( manager );
//    }

    TEnvironment( ClothoObject * co, shared_ptr< iRNG > r, selection_parameter_t * s = NULL ) :
        AppObject( co ),
        m_rng(r),
        m_sel_param(s),
        m_nFounder(0)
    {
        if( m_clotho_object != NULL ) m_clotho_object->updatePerformer( this );
    }

    TEnvironment( const TEnvironment < LCM, IND, SMODEL > & cenv ) :
        AppObject( cenv ),
        m_rng( cenv.m_rng ),
        m_sel_param( cenv.m_sel_param ),
        m_nFounder( cenv.m_nFounder )
    {
        assert(false);
        //if( m_clotho_object != NULL ) m_clotho_object->updatePerformer( this );
    }

    void setFounderSize( unsigned int s ) {
        m_nFounder = s;
    }

    selection_parameter_t * selection_parameter() {
        return m_sel_param;
    }

    virtual void initialize() {
        life_cycle::EnvironmentLifeCycle<LCM, environment_t, ClothoEvent>::initialize();
        initializer::EnvironmentInitializer::init(this);
    }
 
    virtual void perform_event( const event_t * e) {
        life_cycle::EnvironmentLifeCycle<LCM, environment_t, ClothoEvent>::handle_event( this, e );
    }

    virtual void finalize() {
        finalizer::EnvironmentFinalizer::finalize(this);
        m_clotho_object->finalize();
    }

    size_t getActiveIndividualCount() const {
        return m_individual_pool.getActiveNodeCount();
    }

    size_t getPendingIndividualCount() const {
        return m_individual_pool.getPendingNodeCount();
    }

    individual_t & getActiveIndividualAt( size_t active_offset ) {
        size_t idx = m_individual_pool.getActiveObjectAt( active_offset );
        return m_individuals.at( idx );
    }

    
    size_t activateNextIndividual() {
        return m_individual_pool.activateNextPendingObject();
    }

    size_t inactivateNextIndividual() {
        return m_individual_pool.inactivateNextActiveObject();
    }

    virtual ~TEnvironment() {
        m_individual_pool.reset();
        m_individuals.clear();

//        while( !m_gender_group_map.empty() ) {
//            individual_group_t * t = m_gender_group_map.begin()->second;
//            m_gender_group_map.erase( m_gender_group_map.begin());
//            if( t ) delete t;
//        }
    }
protected:

//    individual_t & getIndividual( system_id & id ) const {
//        return m_individual_pool.getPoolObject( m_id_pool_index_map.at(id) );
//    }

    
//    const system_id & getIndividual( ) {
//        return getIndividualObject().getSystemID();
//    }

    individual_t & getIndividual( system_id & id ) {
        size_t idx = m_individual_pool.getPoolObject( m_id_pool_index_map.at(id) );
        return m_individuals.at( idx );
    }

    individual_t & getIndividualAt( size_t idx ) {
        return m_individuals.at( idx );
    }

//    individual_t & getIndividualObject() {
//        size_t idx = getIndividualIndex();
//        return m_individuals.at(idx);
//    }

    size_t getIndividualIndex() {
        size_t idx = m_individual_pool.getInactiveObject();
        if( idx == active_pool_t::UNKNOWN_INDEX ) {
            idx = m_individuals.size();
            ClothoObject * o = m_clotho_object->split();

            m_individuals.push_back( individual_t(o, m_clotho_object ) );
            //individual_t & ind = m_individuals.back();

            o->setActiveIndex( m_individual_pool.setPoolObject( idx ) ); // newly set objects are 'inactive'
            m_individual_pool.pendingObject( idx ); // need to move to 'pending'
            m_id_pool_index_map.insert( std::make_pair( o->getSystemID(), o->getActiveIndex() ));
        }

        return idx;
    }

    void activateIndividual( const system_id & id, unsigned char gtype = -1 ) {
        
        m_individual_pool.activateObject( m_id_pool_index_map[id] );
    }

    void deactivateIndividual( const system_id & id ) {
        size_t pool_idx = m_id_pool_index_map[id];
        size_t idx = m_individual_pool.inactiveObject( pool_idx );

        m_individuals[idx].reset();
    }

    shared_ptr< iRNG > m_rng;
    selection_parameter_t  *   m_sel_param;

    std::vector< individual_t >  m_individuals;
    std::unordered_map< system_id, size_t > m_id_pool_index_map;
    active_pool_t       m_individual_pool;

    unsigned int        m_nFounder;
};

#endif  // ENVIRONMENTPOOLED_HPP_
