#include "individual.h"

#include "../event/birth_event.h"
#include "../event/death_event.h"
#include "../event/inherit_event.h"
#include "../event/mate_event.h"

/*
template <>
struct IDer< ClothoEvent > {
    typedef event_type_t id_type_t;
    inline id_type_t operator()( const ClothoEvent & ce ) const {
        return ce.getEventType();
    }

    inline id_type_t operator()( const ClothoEvent * ce ) const {
        return ce->getEventType();
    }
};
*/

template <>
void EventPerformer< Individual, ClothoEvent >::initialize() {
    addHandler( BIRTH_EVENT_K, &Individual::handle_birth );
    addHandler( DEATH_EVENT_K, &Individual::handle_death );
}

EventPerformer< Individual, ClothoEvent > Individual::m_evt_performer;

Individual::Individual( simulation_manager * manager, 
                        const system_id & env_id,
                        GeneticReproduction * repro ) :
    SimulationObject< ClothoEventSet >( manager ),
    m_env_id( env_id ),
    m_prop( new IndividualProperties() ),
    m_repro( repro )
{
    setSimulationManager( manager );
}

void Individual::initialize() {
//    cout << "Individual: initializing ... " << endl;
    BirthEvent * be = new BirthEvent( getCurrentTime(), getCurrentTime(), this, this, getNextEventID() );

    sendEvent( be );
}

void Individual::perform_event( const event * e ) {
    const ClothoEvent * evt = dynamic_cast< const ClothoEvent * >( e );

    if( evt ) {
/*        if( evt->getEventType() == BIRTH_EVENT_K ) {
            // notify environment of birth
            BirthEvent * be = new BirthEvent( getCurrentTime(), evt->getReceived(), this->getSystemID(), m_env_id, getNextEventID() );
            sendEvent( be );
        } else if( evt->getEventType() == DEATH_EVENT_K ) {
            // notify environment of death
            DeathEvent * de = new DeathEvent( getCurrentTime(), evt->getReceived(), this->getSystemID(), m_env_id, getNextEventID());
            sendEvent( de );
        } else if( evt->getEventType() == MATE_EVENT_K ) {

        }
*/
        m_evt_performer( this, evt );
    }
}

void Individual::finalize() {
    SimulationObject< ClothoEventSet >::finalize();
}

void Individual::handle_birth( const ClothoEvent * evt ) {
    // notify environment of birth
    BirthEvent * be = new BirthEvent( getCurrentTime(), evt->getReceived(), this->getSystemID(), m_env_id, getNextEventID() );
    sendEvent( be );
}

void Individual::handle_death( const ClothoEvent * evt ) {
    // notify environment of death
    DeathEvent * de = new DeathEvent( getCurrentTime(), evt->getReceived(), this->getSystemID(), m_env_id, getNextEventID());
    sendEvent( de );
}
