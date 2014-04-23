#ifndef ENVIRONMENT_LIFE_CYCLE_HPP_
#define ENVIRONMENT_LIFE_CYCLE_HPP_

#include "../life_cycle.hpp"

namespace life_cycle {

template < typename LC >
class EnvironmentLifeCycle {
public:
    template < typename ENV, typename EVT >
    static void handle_event( ENV * env, const EVT * evt );

protected:
};

}   // namespace life_cycle

#endif  // ENVIRONMENT_LIFE_CYCLE_HPP_
