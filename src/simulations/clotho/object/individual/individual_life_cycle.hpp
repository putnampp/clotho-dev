#ifndef INDIVIDUAL_LIFE_CYCLE_GUARD_HPP_
#define INDIVIDUAL_LIFE_CYCLE_GUARD_HPP_

namespace life_cycle {

//template < typename LC >
template < typename LC, class IND, class EVT >
class IndividualLifeCycle {
public:
//    template < typename IND, typename EVT >
    static void handle_event( IND * ind, const EVT * evt ) { assert(false); }

protected:
};

}   // namespace life_cycle

#endif  // INDIVIDUAL_LIFE_CYCLE_GUARD_HPP_
