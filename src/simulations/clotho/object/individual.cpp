#include "individual.h"

#include "../event/birth_event.h"
#include "../event/death_event.h"
#include "../event/inherit_event.h"
#include "../event/maturity_event.h"
#include "../event/mate_event.h"

static const SystemClock::vtime_t LIFE_CYCLE_PADDING = 100;

//#include "../genomes/human_gamete.h"

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
//    SimulationObject< ClothoEventSet >( manager ),
    ClothoObject( manager ),
    m_env_id( env_id ),
    m_prop( new IndividualProperties() ),
    m_repro( repro )
{
    setSimulationManager( manager );
}

void Individual::initialize() {
    //cout << "Initializing Individual: " << getSystemID() << endl;
//    BirthEvent * be = new BirthEvent( getCurrentTime(), getCurrentTime(), this, this, getNextEventID() );
//    sendEvent( be );
//
//    InheritEvent * ie = new InheritEvent( getCurrentTime(), getCurrentTime(), this, this, getNextEventID(), FEMALE, new HumanGamete( HumanGamete::FROM_MOTHER, HumanGamete::X_TYPE, 10000 ) );
//    sendEvent( ie );
//    ie = new InheritEvent( getCurrentTime(), getCurrentTime(), this, this, getNextEventID(), MALE, new HumanGamete( HumanGamete::FROM_FATHER, ((getObjectID() % 2 == 0) ? HumanGamete::X_TYPE : HumanGamete::Y_TYPE), 10000) );
//    sendEvent( ie );
}

void Individual::perform_event( const event * e ) {
    const ClothoEvent * evt = dynamic_cast< const ClothoEvent * >( e );

    if( evt ) {
//        m_evt_performer( this, evt );
//        cout <<"Individual-" << getSystemID() << "@" << getCurrentTime() << ": " << evt << endl;
        if( evt->getEventType() == BIRTH_EVENT_K ) {
            handle_birth( evt );
        } else if( evt->getEventType() == DEATH_EVENT_K ) {
            handle_death( evt );
        } else if( evt->getEventType() == INHERIT_EVENT_K ) {
            handle_inherit( evt );
        } else if( evt->getEventType() == MATE_EVENT_K ) {
            handle_mate( evt );
        } else if( evt->getEventType() == MATURITY_EVENT_K ) {
            handle_maturity( evt );
        }
    }
}

void Individual::finalize() {
    ClothoObject::finalize();
}

void Individual::handle_birth( const ClothoEvent * evt ) {
    // notify environment of birth
    //
    m_prop->setDOB( evt->getReceived() );

    BirthEvent * be = new BirthEvent( getCurrentTime(), evt->getReceived(), this->getSystemID(), m_env_id, getNextEventID(), m_prop->getSex() );
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

    if(m_prop->getEOL() != SystemClock::POSITIVE_INFINITY ) {
        m_prop->reset();
    }

    m_prop->inheritFrom( ie->getSender(), ie->getParentSex(), ie->getGamete() );

    if( m_prop->getFather() != UNSET_ID && m_prop->getMother() != UNSET_ID ) {
        event::vtime_t bday = evt->getReceived();
        bday = ((bday / LIFE_CYCLE_PADDING) + 1) * LIFE_CYCLE_PADDING;

        BirthEvent * be = new BirthEvent( getCurrentTime(), bday, this->getSystemID(), this->getSystemID(), getNextEventID(), m_prop->getSex() );
//        handle_birth( evt );
        sendEvent( be );
    }
}

void Individual::handle_mate( const ClothoEvent * evt ) {
    const MateEvent * me  = static_cast< const MateEvent * >( evt );

    assert( m_repro );

    gamete * z = m_repro->reproduce( m_prop->getGenome() );

    InheritEvent * ie = new InheritEvent( getCurrentTime(), getCurrentTime(), this->getSystemID(), me->getOffspringID(), getNextEventID(), m_prop->getSex(), z );
    sendEvent( ie );
}

void Individual::handle_maturity( const ClothoEvent * evt ) {
//    MaturityEvent * me = new MaturityEvent( getCurrentTime(), getCurrentTime(), this->getSystemID(), m_env_id, getNextEventID(), m_prop->getSex());
    MaturityEvent * me = new MaturityEvent( getCurrentTime(), getCurrentTime(), this->getSystemID(), m_env_id, getNextEventID(), this->getSystemID(), m_prop->getSex());
    sendEvent( me );
}
