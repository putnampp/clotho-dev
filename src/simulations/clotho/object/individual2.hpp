#ifndef INDIVIDUAL2_HPP_
#define INDIVIDUAL2_HPP_

#include "../clotho.h"
#include "../clotho_object.hpp"

#include "individual_initializer.hpp"
#include "individual_life_cycle.hpp"
#include "individual_finalizer.hpp"

#include "../clotho_event.h"

#include "../event/inherit_event.hpp"

#include "reproduction.hpp"   // from libcchmc

#include "individual_properties.hpp"

#include <type_traits>

template < class LCM, class IP, class RMODEL, class ENABLED = void >
class TIndividual;

template < class LCM, class IP, class RMODEL >
class TIndividual< LCM, ENV, IP, RMODEL, typename std::enable_if< std::is_base_of< life_cycle::life_cycle_model, LCM >::value >::type >
 {
public:
    typedef IP properties_t;
    typedef InheritEvent< typename properties_t::gamete_t > inherit_event_t;

    typedef RMODEL reproduction_model_t;

    typedef typename ClothoObject::simulation_manager_t simulation_manager_t;

    friend class initializer::IndividualInitializer;
    friend class finalizer::IndividualFinalizer;
    friend class life_cycle::IndividualLifeCycle<LCM>;

    friend reproduction_model_t;

    TIndividual( ) :
        m_clotho_object( NULL ),
        m_env_id(0),
        m_prop()
    { }

    TIndividual( ClothoObject * co, const system_id & env_id ) :
        m_clotho_object(co),
        m_prop()
    {}

    TIndividual( const TIndividual< LCM, IP, RMODEL> & ind ) :
        m_clotho_object( ind.m_clotho_object ),
        m_env_id( ind.m_env_id ),
        m_prop( ind.m_prop )
    {}

    virtual void initialize( ) {
        initializer::IndividualInitializer::init( this );
    }

    virtual void perform_event( const event_t * e) {
        life_cycle::IndividualLifeCycle<LCM>::handle_event( this, e );
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
    ClothoObject *  m_clotho_object;
    system_id       m_env_id;
    properties_t    m_prop;
};

#endif  // INDIVIDUAL2_HPP_
