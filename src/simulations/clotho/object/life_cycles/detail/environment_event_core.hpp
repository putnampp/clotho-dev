#ifndef ENVIRONMENT_EVENT_CORE_HPP_
#define ENVIRONMENT_EVENT_CORE_HPP_

#include "object/life_cycles/detail/life_cycle_handler.hpp"

#include "object/environment/environment3.hpp"
#include "clotho_event.hpp"

namespace life_cycle {
namespace detail {

#define BEGIN_BOILER_EVENT( event_type ) \
template < template < class, class... > class TT, class T1, class... REST, class LC > \
struct life_cycle_handler < TEnvironment< TT< T1, REST...> > , event_type, LC > { \
    static void perform( TEnvironment< TT< T1, REST...> > * env, event_type * evt ) 

#define BEGIN_BOILER_EVENT_AND_LC( event_type, life_cycle_type ) \
template< template < class, class... > class TT, class T1, class... REST > \
struct life_cycle_handler < TEnvironment< TT< T1, REST...> >, event_type, life_cycle_type > { \
    static void perform( TEnvironment< TT< T1, REST...> > * env, event_type * evt ) 

#define END_BOILER_EVENT };


BEGIN_BOILER_EVENT( BirthEvent ) {
    system_id sender_id = evt->getSender();
//    env->activateIndividual( sender_id );

    ClothoEvent::vtime_t ctime = evt->getReceived();
    ClothoObject * co = env->getClothoObject();

    system_id env_id = evt->getReceiver();

    ClothoEvent::vtime_t mtime = ctime + LC::MATURITY_OFFSET;
    MaturityEvent * me = new MaturityEvent( ctime, mtime, env_id, sender_id, co->getNextEventID(), sender_id );
    co->sendEvent( me, sender_id, mtime );

    ClothoEvent::vtime_t dtime = ctime + LC::DEATH_OFFSET;
    DeathEvent * de = new DeathEvent( ctime, dtime, env_id, sender_id, co->getNextEventID() );
    co->sendEvent(de, sender_id, dtime);

    delete evt;
}
END_BOILER_EVENT


#undef BEGIN_BOILER_EVENT_AND_LC
#undef BEGIN_BOILER_EVENT
#undef END_BOILER_EVENT

}   // namespace detail
}   // namespace life_cycle

#endif  // ENVIRONMENT_EVENT_CORE_HPP_
