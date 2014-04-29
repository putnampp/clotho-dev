#ifndef SIMULATION_MANAGER_IMPL_HPP_
#define SIMULATION_MANAGER_IMPL_HPP_

#include "simulation_manager.hpp"
//#include "simulation_manager_event_manager.h"
#include "event_router.hpp"

template < class E, class O >
class SimulationManager : 
    public simulation_manager< O >,
    public event_router< E >
{
public:
    typedef E event_t;
    typedef typename simulation_manager< O >::object_t object_t;
    typedef typename simulation_manager< O >::vtime_t vtime_t;

    virtual const system_id getNextObjectID() {
        return system_id( simulation_manager<O>::getManagerID(), m_next_object_id++);
    }

    virtual ~SimulationManager() { }
protected:
    SimulationManager( system_id::manager_id_t man_id  ) :
        simulation_manager< O >( man_id, -1 )
        , m_next_object_id(0)
    { }

    system_id::object_id_t   m_next_object_id;
};

#endif  // SIMULATION_MANAGER_IMPL_H_
