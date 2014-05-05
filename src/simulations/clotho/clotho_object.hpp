#ifndef CLOTHO_OBJECT_HPP_
#define CLOTHO_OBJECT_HPP_

#include "clotho.h"
#include "engine/simulation_object.hpp"
#include "engine/simulation_manager_impl.hpp"
#include "clotho_event.h"
#include "engine/ltsf_queue.hpp"

#include "active_poolable.h"

class ClothoObject;

//class ClothoObject : public SimulationObject< ClothoEvent, std::multiset< const ClothoEvent *, ltsf_event_order > > {
class ClothoObject :
    public SimulationObject< ClothoEvent, ltsf_queue< typename ClothoEvent::vtime_t, const ClothoEvent > >,
    public active_poolable
{
public:
//    typedef SimulationObject< ClothoEvent, std::multiset< const ClothoEvent * , ltsf_event_order > > base_object_t;
    typedef SimulationObject< ClothoEvent, ltsf_queue< typename ClothoEvent::vtime_t, const ClothoEvent > > base_object_t;
    typedef SimulationManager< ClothoEvent, ClothoObject > simulation_manager_t;


    ClothoObject( ) : 
        base_object_t(),
        m_sim_manager( NULL ),
        m_active_idx( -1 )
    {}

    ClothoObject( simulation_manager_t * manager, system_id::manager_id_t man, system_id::object_id_t oid ) : 
        base_object_t( (typename base_object_t::event_router_t *) manager, man, oid ),
        m_sim_manager( manager ),
        m_active_idx( -1 )
    {
//        setSimulationManager( manager );
    }

    ClothoObject * split() {
        return m_sim_manager->getSimulationObject();
    }

    virtual void initialize() {
        m_local_time = m_sim_manager->getSimulationTime();
    }

/*    virtual void setSimulationManager( simulation_manager_t * sim ) {
        if( m_sim_manager == sim ) return;

        if( m_sim_manager != NULL ) {
            m_sim_manager->unregisterObject(this);
        }

        if( sim == NULL ) return;

        m_sim_manager = sim;

        setID( m_sim_manager->getNextObjectID() );

        m_sim_manager->registerObject( this );
    }*/

    virtual void finalize() {
        if( m_sim_manager ) {
            m_sim_manager->unregisterObject( this );
        }
    }

    size_t getActiveIndex() const { return m_active_idx; }
    void setActiveIndex( size_t idx ) { m_active_idx = idx; }

    virtual ~ClothoObject() {}

protected:
    simulation_manager_t * m_sim_manager;
    size_t m_active_idx;
};

#endif  // CLOTHO_OBJECT_HPP_
