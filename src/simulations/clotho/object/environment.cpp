#include "environment.h"

#include "individual.h"

#include "../event/birth_event.h"

Environment::Environment( simulation_manager * manager ) :
    SimulationObject< ClothoEventSet >( manager )
{
    setSimulationManager( manager );
}

void Environment::initialize() {

}

void Environment::perform_event( const event * e ) {
    const ClothoEvent * evt = dynamic_cast< const ClothoEvent * >( e );

    if( evt ) {
        if( evt->getEventType() == BIRTH_EVENT_K ) {
            addIndividual( evt->getReceiver() );
        }
    }
}

void Environment::finalize() {

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

void Environment::addIndividual( const system_id & id ) {
    cout << "Environment encountered birth event" << endl;

    
}

void Environment::removeIndividual( const system_id & id ) {

}
