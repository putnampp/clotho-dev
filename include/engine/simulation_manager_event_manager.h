#ifndef SIMULATION_MANAGER_EVENT_MANAGER_H_
#define SIMULATION_MANAGER_EVENT_MANAGER_H_

#include "event_manager.h"

template < class ES >
class SimulationManagerEventManager : virtual public event_manager {
public:

    virtual ~SimulationManagerEventManager() {}
protected:
    const event * getEvent( const system_id & ) { return NULL; }
    const event * peekEvent( const system_id & ) const { return NULL; }
    size_t pendingEventCount( const system_id & ) const { return 0; }
    size_t processedEventCount( const system_id & ) const { return 0; }
    
    void insertEvent( const event * ) {}
};

#endif  // SIMULATION_MANAGER_EVENT_MANAGER_H_
