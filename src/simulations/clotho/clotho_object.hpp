#ifndef CLOTHO_OBJECT_HPP_
#define CLOTHO_OBJECT_HPP_

#include "clotho.h"
#include "engine/simulation_object.hpp"
#include "engine/simulation_manager_impl.hpp"
#include "clotho_event.h"

class ClothoObject;

class ClothoObject : public SimulationObject< ClothoEvent, std::multiset< const ClothoEvent *, ltsf_event_order > > {
public:
    typedef SimulationObject< ClothoEvent, std::multiset< const ClothoEvent * , ltsf_event_order > > base_object_t;

    typedef SimulationManager< ClothoEvent, ClothoObject > simulation_manager_t;

    ClothoObject( ) : 
        base_object_t(),
        m_sim_manager( NULL )
    {}

    ClothoObject( simulation_manager_t * manager ) : 
        base_object_t( (typename base_object_t::event_router_t *) manager ),
        m_sim_manager( NULL )
    {
        setSimulationManager( manager );
    }

    virtual void initialize() {
        m_local_time = m_sim_manager->getSimulationTime();
    }

    virtual void setSimulationManager( simulation_manager_t * sim ) {
        if( m_sim_manager != NULL ) {
            m_sim_manager->unregisterObject(this);
        }

        if( sim == NULL ) return;

        m_sim_manager = sim;

        setID( m_sim_manager->getNextObjectID() );

        m_sim_manager->registerObject( this );
    }

    virtual void finalize() {
        if( m_sim_manager ) {
            m_sim_manager->unregisterObject( this );
        }
    }

    virtual ~ClothoObject() {}

protected:
    simulation_manager_t * m_sim_manager;
};

#endif  // CLOTHO_OBJECT_HPP_
