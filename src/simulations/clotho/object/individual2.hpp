#ifndef INDIVIDUAL2_HPP_
#define INDIVIDUAL2_HPP_

#include "../clotho.h"
#include "../clotho_object.hpp"

#include "individual_initializer.hpp"
#include "individual_life_cycle.hpp"
#include "individual_finalizer.hpp"

#include "../clotho_event.h"
//#include "../models/life_cycle_model.h"

#include "../event/inherit_event.hpp"

//#include "individual_properties.h"
#include "reproduction.hpp"   // from libcchmc

#include "individual_properties.hpp"

#include <type_traits>

template < class LCM, class IP, class RMODEL, class ENABLED = void >
class TIndividual;

template < class LCM, class IP, class RMODEL >
class TIndividual< LCM, IP, RMODEL, typename std::enable_if< std::is_base_of< life_cycle::life_cycle_model, LCM >::value >::type >
: public ClothoObject {
public:
    typedef IP properties_t;
    typedef InheritEvent< typename properties_t::gamete_t > inherit_event_t;

    typedef RMODEL reproduction_model_t;
//    typedef typename RMODEL::parameter_t   reproduction_parameter_t;
//
    typedef typename ClothoObject::simulation_manager_t simulation_manager_t;

    friend class initializer::IndividualInitializer;
    friend class finalizer::IndividualFinalizer;
    friend class life_cycle::IndividualLifeCycle<LCM>;

    friend reproduction_model_t;

    TIndividual( ) :
        ClothoObject( NULL ),
        m_env_id(0),
        m_prop()
    {    }

    TIndividual( simulation_manager_t * manager, 
                        const system_id & env_id/*,
                        reproduction_parameter_t * repro = NULL*/ ) :
        ClothoObject( manager ),
        m_env_id( env_id ),
        m_prop( )/*,
        m_repro( repro )*/
    {
        ClothoObject::setSimulationManager( manager );
    }

    TIndividual( const TIndividual< LCM, IP, RMODEL> & ind ) :
        ClothoObject( ind ),
        m_env_id( ind.m_env_id ),
        m_prop( ind.m_prop )
    {}

    virtual void initialize( ) {
        initializer::IndividualInitializer::init( this );
    }
/*
    reproduction_parameter_t * reproduction_parameter() {
        return this->m_repro;
    }
*/
    virtual void perform_event( const event_t * e) {
        life_cycle::IndividualLifeCycle<LCM>::handle_event( this, e );
    }

    virtual void finalize() {
        finalizer::IndividualFinalizer::finalize( this );
        ClothoObject::finalize();
    }

    virtual void reset() {
        m_prop.reset();
    }

    properties_t * getProperties() {
        return &m_prop;
    }

    virtual ~TIndividual() {
    }
protected:
    //void handle_birth( const ClothoEvent * e ) { assert(false); }
    //void handle_death( const ClothoEvent * e ) { assert(false); }
    //void handle_inherit( const ClothoEvent * e ) { assert(false); }
    //void handle_mate( const ClothoEvent * e ) { assert(false); }
    //void handle_maturity( const ClothoEvent * e ) { assert(false); }

    system_id    m_env_id;
    properties_t m_prop;
//    reproduction_parameter_t * m_repro;
};

#endif  // INDIVIDUAL2_HPP_
