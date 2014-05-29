#ifndef INDIVIDUAL2_HPP_
#define INDIVIDUAL2_HPP_

#include "../clotho.h"
#include "../clotho_object.hpp"

#include "individual_initializer.hpp"
#include "individual_life_cycle.hpp"
#include "individual_finalizer.hpp"

#include "../clotho_event.h"

//#include "../event/inherit_event.hpp"
#include "../event/inherit_event.2.h"

#include "reproduction.hpp"   // from libcchmc

#include "individual_properties.hpp"

#include <type_traits>

#include "../app_object.hpp"

template < class LCM, class IP, class RMODEL, class ENABLED = void >
class TIndividual;

template < class LCM, class IP, class RMODEL >
class TIndividual< LCM, IP, RMODEL, typename std::enable_if< std::is_base_of< life_cycle::life_cycle_model, LCM >::value >::type >
    :   public AppObject
 {
public:
    typedef IP properties_t;
//    typedef InheritEvent< typename properties_t::gamete_t > inherit_event_t;
    typedef typename properties_t::gamete_t gamete_t;
    typedef InheritEvent inherit_event_t;

    typedef RMODEL reproduction_model_t;

    typedef typename ClothoObject::simulation_manager_t simulation_manager_t;
    typedef ClothoObject::event_t event_t;

    typedef life_cycle::IndividualLifeCycle<LCM, TIndividual<LCM, IP, RMODEL>, ClothoEvent> life_cycle_t;

    friend class initializer::IndividualInitializer;
    friend class finalizer::IndividualFinalizer;
    friend life_cycle_t;

    friend reproduction_model_t;

    TIndividual( ) :
        AppObject(),
        m_env(NULL),
        m_prop()
    { }

    TIndividual( ClothoObject * co, ClothoObject * env ) :
        AppObject( co ),
        m_env(env),
        m_prop()
    {
        if( m_clotho_object != NULL ) m_clotho_object->updatePerformer(this);
    }

    TIndividual( const TIndividual< LCM, IP, RMODEL> & ind ) :
        AppObject( ind ),
        m_env( ind.m_env ),
        m_prop( ind.m_prop )
    {
        if( m_clotho_object != NULL ) m_clotho_object->updatePerformer(this);
    }

    virtual void initialize( ) {
        initializer::IndividualInitializer::init( this );
    }

    virtual void perform_event( /*const*/ event_t * e) {
        life_cycle_t::handle_event( this, e );
    }

    virtual void finalize() {
        finalizer::IndividualFinalizer::finalize( this );
        m_clotho_object->finalize();
    }

    virtual void reset() {
        m_prop.reset();
    }

    properties_t * getProperties() {
        return &m_prop;
    }

    virtual ~TIndividual() { }
protected:
//    ClothoObject *  m_clotho_object;
//    system_id       m_env_id;
    ClothoObject *  m_env;
    properties_t    m_prop;
};

#endif  // INDIVIDUAL2_HPP_
