#ifndef SIMULATIONMANAGER_HPP_
#define SIMULATIONMANAGER_HPP_

#include "object.hpp"

template < class OBJ >
class simulation_manager : public object {
public:
    typedef OBJ object_t;

    virtual const vtime_t & getSimulationTime() const = 0;
    virtual bool isSimulationComplete() const = 0;

    virtual const system_id getNextObjectID() = 0;

    virtual void registerObject( object_t * obj ) = 0;
    virtual void unregisterObject( object_t * obj ) = 0;

    virtual size_t getObjectCount() const = 0;
    virtual object_t * getObject( const system_id & id ) const = 0;

    virtual void simulate( const event::vtime_t & until ) = 0;

    virtual void notifyNextEvent( const system_id &, const vtime_t & ) = 0;

    virtual ~simulation_manager() {}

protected:
    simulation_manager( system_id::manager_id_t man_id, system_id::object_id_t obj_id ) :
        object( man_id, obj_id )
    {}

};

#endif  // SIMULATIONMANAGER_HPP_
