#ifndef ENVIRONMENT_HPP_
#define ENVIRONMENT_HPP_

#include "../clotho.h"
#include "../clotho_object.h"
#include "../event/clotho_event.h"
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

#include "individual.hpp"
//#include "individual_properties.hpp"

using std::deque;
using std::vector;
using std::set;
using std::unordered_map;
using std::unordered_set;
using std::swap;


template < class LCM, /*class VT,*/ class IND, class SMODEL, class Enable = void >
class TEnvironment;

template < class LCM, /* class VT,*/ class IND, class SMODEL >
class TEnvironment < LCM,/* VT,*/ IND, SMODEL, typename std::enable_if< std::is_base_of< life_cycle::life_cycle_model, LCM >::value >::type >
    : public ClothoObject {
public:
    typedef vector< system_id > individual_group_t;
    typedef std::pair< individual_group_t *, size_t > pair_individual_group_offset;
    typedef unordered_map< system_id, pair_individual_group_offset > individual_group_lookup_t;
    typedef unordered_map< unsigned char, individual_group_t * > gender_group_map_t;
    typedef typename individual_group_lookup_t::iterator        lookup_iterator;
    typedef typename individual_group_lookup_t::const_iterator  lookup_citerator;

//    typedef variant_map< VT >       variant_map_t;
    typedef IND individual_t;
    typedef individual_t *  individual_ptr_t;
    typedef typename SMODEL::parameter_t   selection_parameter_t;

    typedef selection::MateSelector< SMODEL > selection_model_t;

    friend class initializer::EnvironmentInitializer;
    friend class finalizer::EnvironmentFinalizer;
    friend class life_cycle::EnvironmentLifeCycle<LCM>;

    friend selection_model_t;

    TEnvironment( simulation_manager * manager, shared_ptr< iRNG > r ) :
        ClothoObject( manager ),
        m_rng(r),
        //m_variants( new variant_map_t(r) ),
        m_sel_param( NULL ),
        m_nIndAlloc(0),
        m_nFounder(0),
        m_nMateOps(0)
    {
        ClothoObject::setSimulationManager( manager );
    }

    TEnvironment( simulation_manager * manager, shared_ptr< iRNG > r,/* std::shared_ptr< variant_map_t > vmap,*/ selection_parameter_t * s ) :
        ClothoObject( manager ),
        m_rng(r),
//        m_variants( vmap ),
        m_sel_param( s ),
        m_nIndAlloc(0),
        m_nFounder(0),
        m_nMateOps(0)
    {
        ClothoObject::setSimulationManager( manager );
    }

    void setFounderSize( unsigned int s ) {
        m_nFounder = s;
    }

    selection_parameter_t * selection_parameter() {
        return m_sel_param;
    }

    virtual void initialize() {
        initializer::EnvironmentInitializer::init(this);
    }
 
    virtual void perform_event( const event * e) {
        life_cycle::EnvironmentLifeCycle<LCM>::handle_event( this, e );
    }

    virtual void finalize() {
        finalizer::EnvironmentFinalizer::finalize(this);
        ClothoObject::finalize();
    }

    virtual ~TEnvironment() {
        while( !m_gender_group_map.empty() ) {
            individual_group_t * t = m_gender_group_map.begin()->second;
            m_gender_group_map.erase( m_gender_group_map.begin());
            if( t ) delete t;
        }
    }
protected:

    system_id getIndividual( ) {
        system_id id(0);
        if( m_available_individuals.empty() ) {
            ++m_nIndAlloc;
            individual_ptr_t ind = new individual_t( m_sim_manager, getSystemID()/*, NULL*/ );

            id = ind->getSystemID();

            //std::cout << "Created new individual: " << id << std::endl;
        } else {
            // future consideration: may be slightly more efficient to use back() & pop_back()?
            // initially thought that least recently used individual shell was more
            // preferrable than most recently used shell. though not sure
            id = m_available_individuals.front();
            m_available_individuals.pop_front();

            individual_ptr_t t = dynamic_cast< individual_ptr_t >( m_sim_manager->getObject( id ) );
            assert( t );
            t->reset();

            assert( m_available_individuals.empty() || m_available_individuals.front() != id );
        }

        m_pending.insert(id);

        return id;
    }

    void activateIndividual( const system_id & id, unsigned char gtype = -1 ) {
        m_pending.erase( id );

        gender_group_map_t::iterator it = m_gender_group_map.find(gtype);
        if( it == m_gender_group_map.end() ) {
            std::pair< gender_group_map_t::iterator, bool > res = m_gender_group_map.insert( make_pair( gtype, new individual_group_t() ) );
            assert( res.second );

            it = res.first;
        }

        m_active_individuals.insert( make_pair( id, make_pair( it->second, it->second->size())));
        it->second->push_back( id );
    }

    void deactivateIndividual( const system_id & id ) {
        lookup_iterator it = m_active_individuals.find( id );
        assert( it != m_active_individuals.end() );

        individual_group_t * igroup = it->second.first;
        size_t offset = it->second.second;

        if( offset != igroup->size() - 1 ) {
            // if the dead individual is not the last individual in the vector
            // swap the dead individual with the last individual in the vector
            // and update the previously last individual's offset to be the offset of
            // the dead individual
            swap( igroup->back(), igroup->at(offset) );
            assert( igroup->back() == id );

            m_active_individuals[ igroup->at(offset) ].second = offset;
        }

        it->second.first->pop_back();
        m_active_individuals.erase(it);
        m_available_individuals.push_back( id );
    }

    shared_ptr< iRNG > m_rng;
//    individual_group_t m_males, m_females;
    gender_group_map_t m_gender_group_map;
    individual_group_lookup_t m_active_individuals;
    deque< system_id > m_available_individuals;
    set< system_id > m_pending; 

    //std::shared_ptr< variant_map_t >       m_variants;
    selection_parameter_t  *   m_sel_param;

    unsigned int        m_nIndAlloc;
    unsigned int        m_nFounder;

    unsigned int        m_nMateOps;
};

#endif  // ENVIRONMENT_HPP_
