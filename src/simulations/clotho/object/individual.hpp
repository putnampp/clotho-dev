#ifndef INDIVIDUAL_HPP_
#define INDIVIDUAL_HPP_

#include "../clotho.h"
#include "../clotho_object.h"
#include "../event/clotho_event.h"
#include "../models/life_cycle_model.h"

#include "individual_properties.h"
#include "reproduction.h"   // from libcchmc

#include <type_traits>

template < class LCM, class ENABLED = void >
class TIndividual;

template < class LCM >
class TIndividual< LCM, typename std::enable_if< std::is_base_of< life_cycle_model, LCM >::value >::type >
: public ClothoObject {
public:
    TIndividual( simulation_manager * manager, 
                        const system_id & env_id,
                        reproduction * repro ) :
        ClothoObject( manager ),
        m_env_id( env_id ),
        m_prop( new IndividualProperties() ),
        m_repro( repro )
    {
        ClothoObject::setSimulationManager( manager );
    }

    virtual void initialize( ) { assert(false); }

    virtual void perform_event( const event * ) { assert(false); }

    virtual void finalize() { assert(false); }

    virtual ~TIndividual() {
        if(m_prop) delete m_prop;
    }
protected:
    void handle_birth( const ClothoEvent * e ) { assert(false); }
    void handle_death( const ClothoEvent * e ) { assert(false); }
    void handle_inherit( const ClothoEvent * e ) { assert(false); }
    void handle_mate( const ClothoEvent * e ) { assert(false); }
    void handle_maturity( const ClothoEvent * e ) { assert(false); }

    system_id   m_env_id;
    IndividualProperties * m_prop;
    reproduction * m_repro;
};

#endif  // INDIVIDUAL_HPP_
