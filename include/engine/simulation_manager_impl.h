#ifndef SIMULATION_MANAGER_IMPL_H_
#define SIMULATION_MANAGER_IMPL_H_

#include "simulation_manager.h"

class SimulationManager : virtual public simulation_manager
{
public:
    inline const system_id & getSystemID() const {
        return m_id;
    }

    inline system_id::manager_id_t getManagerID() const {
        return m_id.getManagerID();
    }

    inline system_id::object_id_t getObjectID() const {
        return m_id.getObjectID();
    }

    virtual const system_id getNextObjectID() {
        return system_id( m_id.getManagerID(), m_next_object_id++);
    }

    virtual ~SimulationManager() { }
protected:
    SimulationManager( system_id::manager_id_t man_id  ) :
        m_id( man_id, -1 )
        , m_next_object_id(0)
    { }

    void setID( const system_id & id ) {
        m_id = id;
    }
    
    system_id   m_id;
    system_id::object_id_t   m_next_object_id;
};

#endif  // SIMULATION_MANAGER_IMPL_H_
