#ifndef SIMULATIONMANAGER_H_
#define SIMULATIONMANAGER_H_

#include "system_id.h"
#include "object.h"

struct simulation_manager {
    virtual const system_id & getSystemID() const = 0;
    virtual system_id::manager_id_t getManagerID() const = 0;

    virtual const event::vtime_t & getSimulationTime() const = 0;
    virtual bool isSimulationComplete() const = 0;

    virtual const system_id & getNextObjectID() = 0;

    virtual void registerObject( object * obj ) = 0;
    virtual void unregisterObject( object * obj ) = 0;

    virtual size_t getObjectCount() const = 0;
    virtual object * getObject( const system_id & id ) const = 0;

    virtual void initialize() = 0;
    virtual void simulate( const event::vtime_t & until ) = 0;
    virtual void finalize() = 0;

    virtual void routeEvent( const event * evt ) = 0;

    virtual void notifyNextEvent( const system_id &, const event::vtime_t & ) = 0;

    virtual ~simulation_manager() {}
};

#endif  // SIMULATIONMANAGER_H_
