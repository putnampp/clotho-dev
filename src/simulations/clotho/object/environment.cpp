#include "environment.h"

#include "individual.h"

#include "../event/birth_event.h"
#include "../event/death_event.h"

template <>
void EventPerformer< Environment, ClothoEvent>::initialize() {
    addHandler( BIRTH_EVENT_K, &Environment::handle_birth );
    addHandler( DEATH_EVENT_K, &Environment::handle_death );
}

EventPerformer< Environment, ClothoEvent > Environment::m_evt_performer;

Environment::Environment( simulation_manager * manager ) :
    SimulationObject< ClothoEventSet >( manager ),
    m_genetic_map( new GeneticMap() )
{
    setSimulationManager( manager );
}

Environment::Environment( simulation_manager * manager, GeneticMap::Ptr gmap ) :
    SimulationObject< ClothoEventSet >( manager ),
    m_genetic_map( gmap )
{
    setSimulationManager( manager );
}

void Environment::initialize() {

    for( int i = 0; i < 10; ++i ) {
        system_id id = getIndividual();
        object * t = m_sim_manager->getObject( id );

        t->initialize();
    }
}

void Environment::perform_event( const event * e ) {
    const ClothoEvent * evt = dynamic_cast< const ClothoEvent * >( e );

    if( evt ) {
//        if( evt->getEventType() == BIRTH_EVENT_K ) {
//            addIndividual( evt->getSender() );
//        } else if( evt->getEventType() == DEATH_EVENT_K ) {
//            removeIndividual( evt->getSender() );
//        }
        m_evt_performer( this, evt );
    }
}

void Environment::finalize() {
    for( unordered_set< system_id >::iterator it = m_active_individuals.begin(); it != m_active_individuals.end(); ) {
        object * tmp = m_sim_manager->getObject( *it++ );
        tmp->finalize();
        
        delete tmp;
    }

    for( list< system_id>::iterator it = m_available_individuals.begin(); it != m_available_individuals.end(); ) {
        object * tmp = m_sim_manager->getObject( *it++ );
        tmp->finalize();

        delete tmp;
    }

    SimulationObject< ClothoEventSet >::finalize();
}

system_id Environment::getIndividual() {
    system_id id(0);
    if( m_available_individuals.empty() ) {
        Individual * ind = new Individual( m_sim_manager, getSystemID(), NULL );
        id = ind->getSystemID();
    } else {
        id = m_available_individuals.front();
        m_available_individuals.pop_front();
    }

    return id;
}

void Environment::handle_birth( const ClothoEvent * ce ) {
//    addIndividual( evt->getSender() );
    DeathEvent * de = new DeathEvent( getCurrentTime(), getCurrentTime() + 1, getSystemID(), ce->getSender(), getNextEventID() );
    sendEvent(de);
    m_active_individuals.insert( ce->getSender() );
}

void Environment::handle_death( const ClothoEvent * ce ) {
//    removeIndividual( evt->getSender() );
    m_active_individuals.erase( ce->getSender() );
    m_available_individuals.push_back( ce->getSender() );
}
/*
void Environment::addIndividual( const system_id & id ) {
//    cout << "Environment encountered birth event: " << id << " @ " << getCurrentTime() << endl;

    DeathEvent * de = new DeathEvent( getCurrentTime(), getCurrentTime() + 1, getSystemID(), id, getNextEventID() );
    sendEvent(de);
    m_active_individuals.insert( id );
}

void Environment::removeIndividual( const system_id & id ) {
//    cout << "Environment encountered death event: " << id << " @ " << getCurrentTime() << endl;

    m_active_individuals.erase( id );
    m_available_individuals.push_back( id );
}
*/

