#ifndef INDIVIDUAL_LIFE_CYCLE_HPP_
#define INDIVIDUAL_LIFE_CYCLE_HPP_

#include "../life_cycle.hpp"

namespace life_cycle {

template < typename LC >
class IndividualLifeCycle {
public:
    template < typename IND, typename EVT >
    static void handle_event( IND * ind, const EVT * evt ) { assert(false); }

protected:
};
    
}   // namespace life_cycle

#endif  // INDIVIDUAL_LIFE_CYCLE_HPP_
