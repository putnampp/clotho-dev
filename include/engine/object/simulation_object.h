#ifndef SIMULATION_OBJECT_H_
#define SIMULATION_OBJECT_H_

#include "object_impl.h"
#include "EventManager.h"
#include "event_transceiver.h"

template< class ES >
class SimulationObject : 
    virtual public Object,
    virtual public EventManager< ES >
{
public:
    SimulationObject( ) : Object( ) {}
    SimulationObject( simulation_manager * manager ) : 
        Object( manager )
    {}

    virtual ~SimulationObject() { }
protected:
};

#endif  // SIMULATION_OBJECT_H_
