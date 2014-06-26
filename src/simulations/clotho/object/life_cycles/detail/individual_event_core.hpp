#ifndef INDIVIDUAL_EVENT_CORE_HPP_
#define INDIVIDUAL_EVENT_CORE_HPP_

#include "object/life_cycles/detail/life_cycle_handler.hpp"

#include "object/individual.hpp"
#include "object/individual_properties.hpp"
#include "clotho_event.hpp"

namespace life_cycle {
namespace detail {

#define BEGIN_BOILER_EVENT( event_type ) \
template < class G, unsigned char P, class S, class LC > \
struct life_cycle_handler < TIndividual< G, P, S, individual_props< G, P, S> > , event_type, LC > { \
    static void perform( TIndividual< G, P, S, individual_props< G, P, S> > * ind, event_type * evt ) 

#define BEGIN_BOILER_EVENT_AND_LC( event_type, life_cycle_type ) \
template < class G, unsigned char P, class S > \
struct life_cycle_handler < TIndividual< G, P, S, individual_props< G, P, S> > , event_type, life_cycle_type > { \
    static void perform( TIndividual< G, P, S, individual_props< G, P, S> > * ind, event_type * evt ) 

#define END_BOILER_EVENT };

BEGIN_BOILER_EVENT( BirthEvent ) {
    ClothoEvent::vtime_t ctime = evt->getReceived();
    ind->getProperties()->setDOB( ctime );

    ClothoObject * co = ind->getClothoObject();
        
    system_id obj_id = co->getSystemID();
    system_id env_id = ind->getEnvironmentID();

    evt->init( ctime, ctime, obj_id, env_id, co->getNextEventID() );
    co->sendEvent( evt, env_id, ctime );
}
END_BOILER_EVENT

BEGIN_BOILER_EVENT( DeathEvent ) {
    ind->getProperties()->died();

    ClothoEvent::vtime_t ctime = evt->getReceived();
    ClothoObject * co = ind->getClothoObject();

    system_id obj_id = co->getSystemID();
    system_id env_id = ind->getEnvironmentID();

    // re-purpose the event 
    evt->init( ctime, ctime, obj_id, env_id, co->getNextEventID() );
    co->sendEvent( evt, env_id, ctime );
}
END_BOILER_EVENT

BEGIN_BOILER_EVENT( InheritEvent ) {
    // assert that alive individual is not inheriting
    // new genetic material

    typedef typename TIndividual< G, P, S, individual_props<G,P,S> >::gamete_type gamete_t;
    typedef typename gamete_t::pointer gamete_ptr;
    typedef typename TIndividual< G, P, S, individual_props<G,P,S> >::properties_type properties_type;

    properties_type * prop = ind->getProperties();
    assert( !prop->isAlive() );

    prop->inheritFrom( evt->getSender(), evt->getGamete(), evt->getParentIndex() );

    if( prop->hasSourceGametes() ) {
        ClothoEvent::vtime_t ctime = evt->getReceived();
        ClothoEvent::vtime_t bday = LC::nextGeneration( ctime );

        ClothoObject * co = ind->getClothoObject();
        system_id id = co->getSystemID();

        BirthEvent * be = new BirthEvent( ctime, bday, id, id, co->getNextEventID() );

        co->sendEvent( be, id, bday );
    }

    delete evt;
}
END_BOILER_EVENT

BEGIN_BOILER_EVENT( MateEvent ) {
    typedef typename TIndividual< G, P, S, individual_props<G,P,S> >::gamete_type gamete_t;
    typedef typename gamete_t::pointer gamete_ptr;
    typedef InheritEvent ievent_t;
        
    gamete_ptr z = TIndividual< G, P, S, individual_props<G,P,S> >::reproduce( ind );

    ClothoEvent::vtime_t ctime = evt->getReceived();
    ClothoObject * co = ind->getClothoObject();

    system_id id = co->getSystemID();
    system_id off_id = evt->getOffspringID();

    ievent_t * ie = new ievent_t( ctime, ctime, id, off_id, co->getNextEventID(), z );
    co->sendEvent( ie, off_id, ctime );

    delete evt;
}
END_BOILER_EVENT

#undef BEGIN_BOILER_EVENT_AND_LC
#undef BEGIN_BOILER_EVENT
#undef END_BOILER_EVENT

} // namespace detail
} // namespace life_cycle

#endif  // INDIVIDUAL_EVENT_CORE_HPP_
