#ifndef INDIVIDUAL3_HPP_
#define INDIVIDUAL3_HPP_

#include "config.hpp"

//#include "individual3_initializer.hpp"
//#include "individual3_life_cycle.hpp"
//#include "individual3_finalizer.hpp"

#include "individual_properties.hpp"
#include <type_traits>


template < class G, unsigned char P = 2, class S = system_id, 
            class PROP = individual_props< G, P, S> >
class TIndividual : public AppObject {
public:
    typedef G gamete_type;
    typedef S source_type;
    typedef PROP properties_type;

    typedef TIndividual< G, P, S, PROP > * pointer;
    typedef typename gamete_type::pointer  gamete_pointer;
    
    static const unsigned char PLOIDY = P;

    TIndividual();
    TIndividual( ClothoObject * co, ClothoObject * env );
    TIndividual( const TIndividual< G, P, S, PROP > & ind );

    void reset();

    static gamete_pointer reproduce( pointer ind );

    void initialize();
    void finalize();

    properties_type * getProperties();

    virtual ~TIndividual();
protected:
    properties_type m_props;
};

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

    ClothoObject::event_id_t n_eid = co->getNextEventID(2);
    ievent_t * ie = new ievent_t( ctime, ctime, id, id, n_eid++, gamete_type::EMPTY_GAMETE.copy());
    co->sendEvent( ie, id, ctime );

    ie = new ievent_t( ctime, ctime, id, id, n_eid, gamete_type::EMPTY_GAMETE.copy());
    co->sendEvent( ie, id, ctime );
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

#endif  // INDIVIDUAL3_HPP_
