#include "individual.h"

#include "../event/birth_event.h"

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
    cout << "Individual: initializing ... " << endl;
    BirthEvent * be = new BirthEvent( getCurrentTime(), getCurrentTime(), this, this, getNextEventID() );

    sendEvent( be );
}

void Individual::perform_event( const event * e ) {
    cout << getSystemID() << " performing event" << endl;
    const ClothoEvent * evt = dynamic_cast< const ClothoEvent * >( e );

    if( evt ) {
        cout << getCurrentTime() << " - " <<  getSystemID() << ": " << evt->getEventType() << endl;

        BirthEvent * be = new BirthEvent( getCurrentTime(), evt->getReceived(), this->getSystemID(), m_env_id, getNextEventID() );
        sendEvent( be );
    }
}

void Individual::finalize() {
    SimulationObject< ClothoEventSet >::finalize();
}
