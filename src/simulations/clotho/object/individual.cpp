#include "individual.h"

#include "../event/birth_event.h"
#include "../event/death_event.h"
#include "../event/inherit_event.h"
#include "../event/mate_event.h"

template <>
void EventPerformer< Individual, ClothoEvent >::initialize() {
    addHandler( BIRTH_EVENT_K, &Individual::handle_birth );
    addHandler( DEATH_EVENT_K, &Individual::handle_death );
    addHandler( INHERIT_EVENT_K, &Individual::handle_inherit );
    addHandler( MATE_EVENT_K, &Individual::handle_mate );
}

EventPerformer< Individual, ClothoEvent > Individual::m_evt_performer;

Individual::Individual( simulation_manager * manager, 
                        const system_id & env_id,
                        reproduction * repro ) :
    SimulationObject< ClothoEventSet >( manager ),
    m_env_id( env_id ),
    m_prop( new IndividualProperties() ),
    m_repro( repro )
{
    setSimulationManager( manager );
}

void Individual::initialize() {
    BirthEvent * be = new BirthEvent( getCurrentTime(), getCurrentTime(), this, this, getNextEventID() );

    sendEvent( be );
}

void Individual::perform_event( const event * e ) {
    const ClothoEvent * evt = dynamic_cast< const ClothoEvent * >( e );

    if( evt ) {
//        m_evt_performer( this, evt );
        if( evt->getEventType() == BIRTH_EVENT_K ) {
            handle_birth( evt );
        } else if( evt->getEventType() == DEATH_EVENT_K ) {
            handle_death( evt );
        } else if( evt->getEventType() == INHERIT_EVENT_K ) {
            handle_inherit( evt );
        } else if( evt->getEventType() == MATE_EVENT_K ) {
            handle_mate( evt );
        }
    }
}

void Individual::finalize() {
    SimulationObject< ClothoEventSet >::finalize();
}

void Individual::handle_birth( const ClothoEvent * evt ) {
    // notify environment of birth
    //
    m_prop->setDOB( evt->getReceived() );

    BirthEvent * be = new BirthEvent( getCurrentTime(), evt->getReceived(), this->getSystemID(), m_env_id, getNextEventID() );
    sendEvent( be );
}

void Individual::handle_death( const ClothoEvent * evt ) {
    // notify environment of death
    m_prop->setEOL( evt->getReceived() );

    DeathEvent * de = new DeathEvent( getCurrentTime(), evt->getReceived(), this->getSystemID(), m_env_id, getNextEventID());
    sendEvent( de );
}

void Individual::handle_inherit( const ClothoEvent * evt ) {
    const InheritEvent * ie = static_cast< const InheritEvent * >( evt );

    m_prop->inheritFrom( ie->getSender(), ie->getParentSex(), ie->getZygote() );

    if( m_prop->getFather() != UNSET_ID && m_prop->getMother() != UNSET_ID ) {
        handle_birth( evt );
    }
}

void Individual::handle_mate( const ClothoEvent * evt ) {
    const MateEvent * me  = static_cast< const MateEvent * >( evt );

    assert( m_repro );
    

    zygote * z = m_repro->reproduce( m_prop->getGenome() );

    InheritEvent * ie = new InheritEvent( getCurrentTime(), getCurrentTime(), this->getSystemID(), me->getOffspringID(), getNextEventID(), m_prop->getSex(), z );
    sendEvent( ie );
}
