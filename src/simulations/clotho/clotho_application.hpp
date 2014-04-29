#ifndef CLOTHO_APPLICATION_HPP_
#define CLOTHO_APPLICATION_HPP_

#include "clotho.h"
#include "engine/application.h"
#include "engine/simulation_manager_impl.hpp"

#include "clotho_application_initializer.hpp"
#include "clotho_application_finalizer.hpp"
#include "object/environment2.hpp"

#include <type_traits>
#include <vector>

#include <iostream>

template < class MGR, class ENV, class ENABLED = void >
class ClothoApplication;

template < class MGR, class ENV >
class ClothoApplication< MGR, ENV,
    typename std::enable_if< std::is_base_of< SimulationManager< ClothoEvent, ClothoObject >, MGR>::value && std::is_base_of<ClothoObject, ENV>::value >::type >
    : public application {
public:
    typedef MGR     manager_t;
    typedef MGR *   manager_ptr_t;
    typedef ENV     environment_t;
    typedef ENV *   environment_ptr_t;

    friend class initializer::ClothoAppInitializer;
    friend class finalizer::ClothoAppFinalizer;

    typedef std::vector< system_id > sim_object_id_list_t;

    ClothoApplication( shared_ptr< iRNG > r ) : 
        m_sim_manager( NULL ),
        m_rng( r ),
        m_nFounder(0)
    {}

    ClothoApplication( manager_ptr_t m, shared_ptr< iRNG > r ) :
        m_sim_manager( m ),
        m_rng( r ),
        m_nFounder(0)
    {}

    void setSimulationManager( manager_ptr_t manager ) {
        m_sim_manager = manager;
    }

    void setFounderSize( unsigned int s ) {
        m_nFounder = s;
    }
    
    void initialize() {
        assert( m_sim_manager );
        initializer::ClothoAppInitializer::init(this);
    }

    void finalize() {
        assert( m_sim_manager );
        finalizer::ClothoAppFinalizer::finalize(this);
    }

    virtual ~ClothoApplication() {}

protected:
    manager_ptr_t           m_sim_manager;
    shared_ptr< iRNG >      m_rng;
    sim_object_id_list_t    m_objects;
    
    unsigned int            m_nFounder;
};

#endif  // CLOTHO_APPLICATION_HPP_
