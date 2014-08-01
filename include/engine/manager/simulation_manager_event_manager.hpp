#ifndef SIMULATION_MANAGER_EVENT_MANAGER_HPP_
#define SIMULATION_MANAGER_EVENT_MANAGER_HPP_

#include "event_manager.hpp"

template < class E, class ES >
class SimulationManagerEventManager : virtual public event_manager< E > {
public:

    virtual ~SimulationManagerEventManager() {}
protected:
    void reset_pending() { }
    void reset_processed() {}

    const event_t * getEvent( const system_id & ) {
        return NULL;
    }
    const event * peekEvent( const system_id & ) const {
        return NULL;
    }
    size_t pendingEventCount( const system_id & ) const {
        return 0;
    }
    size_t processedEventCount( const system_id & ) const {
        return 0;
    }
    size_t  canceledEventCount( const system_id & ) const {
        return 0;
    }

    bool insertEvent( const event * ) {
        return false;
    }
};

#endif  // SIMULATION_MANAGER_EVENT_MANAGER_HPP_
