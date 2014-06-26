#ifndef CLOTHO_EVENT_CORE_HPP_
#define CLOTHO_EVENT_CORE_HPP_

#include "object/life_cycles/detail/life_cycle_handler.hpp"
#include "clotho_event.hpp"

namespace life_cycle {
namespace detail {

#define TID( event ) event::CEID

#define BOILER_PLATE_PERFORM( event )  life_cycle_handler< OBJ, event, LC >::perform( o, ( event * ) e )

#if !defined USE_SWITCH_EVENT_TYPE_CHECK

#define BASE_TYPE_CHECK( id, event ) if ( id == TID(event) ) { BOILER_PLATE_PERFORM( event ); }
#define ADD_TYPE_CHECK( id, event ) else BASE_TYPE_CHECK(id, event)
#define END_TYPE_CHECK else { assert(false); }
#define BEGIN_TYPE_CHECK( id, event ) BASE_TYPE_CHECK( id, event )

#else

#define BASE_TYPE_CHECK( id, event ) case TID(event): { BOILER_PLATE_PERFORM( event ); break; }
#define ADD_TYPE_CHECK( id, event ) BASE_TYPE_CHECK(id, event)
#define END_TYPE_CHECK default: { assert(false); } }
#define BEGIN_TYPE_CHECK( id, event ) switch( id ) { BASE_TYPE_CHECK( id, event )

#endif

template < typename OBJ, typename LC >
struct life_cycle_handler< OBJ, ClothoEvent, LC > {
    static void perform( OBJ * o, ClothoEvent * e ) {
        clotho_event_type   eid = e->getEventType();

        BEGIN_TYPE_CHECK( eid, BirthEvent )
        ADD_TYPE_CHECK( eid, DeathEvent )
        ADD_TYPE_CHECK( eid, EnvironmentSnapshotEvent )
        ADD_TYPE_CHECK( eid, InheritEvent )
        ADD_TYPE_CHECK( eid, MateEvent )
        ADD_TYPE_CHECK( eid, MateSelectEvent )
        ADD_TYPE_CHECK( eid, MaturityEvent )
        ADD_TYPE_CHECK( eid, SelectionEvent )
        ADD_TYPE_CHECK( eid, SignalMateEvent )
        END_TYPE_CHECK
    }
};

#undef BASE_TYPE_CHECK
#undef ADD_TYPE_CHECK
#undef BEGIN_TYPE_CHECK
#undef END_TYPE_CHECK

} // namespace detail
} // namespace life_cycle

#endif  // CLOTHO_EVENT_CORE_HPP_
