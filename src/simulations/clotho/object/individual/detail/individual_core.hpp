#ifndef INDIVIDUAL_CORE_HPP_
#define INDIVIDUAL_CORE_HPP_

#include "object/individual/individual3.hpp"

namespace objects {
namespace individual {

#define BOILER_PLATE_TEMPLATE template < class G, unsigned char P, class S, class PROP >
#define BOILER_PLATE_SPECIALIZED_CLASS TIndividual< G, P, S, PROP >
#define BOILER_PLATE_STRUCTOR( method ) BOILER_PLATE_TEMPLATE BOILER_PLATE_SPECIALIZED_CLASS::method
#define BOILER_PLATE_METHOD( t, m ) BOILER_PLATE_TEMPLATE t BOILER_PLATE_SPECIALIZED_CLASS::m


BOILER_PLATE_STRUCTOR( TIndividual )() :
    AppObject(),
    m_props()
{ }

BOILER_PLATE_STRUCTOR( TIndividual )( ClothoObject * co, ClothoObject * env ) :
    AppObject( co ),
    m_props()
{}

BOILER_PLATE_STRUCTOR( TIndividual )( const BOILER_PLATE_SPECIALIZED_CLASS & ind ) :
    AppObject( ind ),
    m_props( ind.m_prop )
{}

BOILER_PLATE_METHOD( void, initialize )() {
    typedef InheritEvent ievent_t;

    ClothoObject * co = getClothoObject();
    ClothoEvent::vtime_t ctime = co->getCurrentTime();

    system_id id = co->getSystemID();

    ClothoObject::event_id_t n_eid = co->getNextEventID(PLOIDY);

    for( unsigned char i = 0; i < PLOIDY; ++i ) {
        ievent_t * ie = new ievent_t( ctime, ctime, id, id, n_eid++, gamete_type::EMPTY_GAMETE.copy());
        co->sendEvent( ie, id, ctime );
    }
}

BOILER_PLATE_METHOD(void, finalize )(){
    getClothoObject()->finalize();
}

BOILER_PLATE_METHOD(void, reset)() {
    m_props.reset();
}

BOILER_PLATE_STRUCTOR( ~TIndividual )() {}

#undef BOILER_PLATE_TEMPLATE
#undef BOILER_PLATE_SPECIALIZED_CLASS
#undef BOILER_PLATE_STRUCTOR
#undef BOILER_PLATE_METHOD

}   // namespace individual
}   // namespace objects

#endif  // INDIVIDUAL_CORE_HPP_
