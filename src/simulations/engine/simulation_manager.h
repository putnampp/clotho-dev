#ifndef SIMULATIONMANAGER_H_
#define SIMULATIONMANAGER_H_

#include "system_id.h"

struct simulation_manager {
    virtual const system_id & getSystemID() const = 0;
    virtual system_id::manager_id_t getManagerID() const = 0;

    virtual void registerObject( object * obj ) = 0;
    virtual void unregisterObject( object * obj ) = 0;

    virtual void initializeSimulation() = 0;
    virtual void simulate() = 0;
    virtual void finalize() = 0;

    virtual void routeEvent( const event * evt ) = 0;

    virtual ~simulation_manager() {}
};

#endif  // SIMULATIONMANAGER_H_
