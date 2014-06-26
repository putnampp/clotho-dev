#ifndef LIFE_CYCLE_HANDLER_HPP_
#define LIFE_CYCLE_HANDLER_HPP_

namespace life_cycle {
namespace detail {

template < typename OBJ, typename EVT, typename LC >
struct life_cycle_handler {
    static void perform( OBJ * o, EVT * e );
};

//
// Implementation
//
template < typename OBJ, typename EVT, typename LC > 
void life_cycle_handler< OBJ, EVT, LC >::perform( OBJ * o, EVT * e ) { }

}   // namespace detail
}   // namespace life_cycle

#endif  // LIFE_CYCLE_HANDLER_HPP_
