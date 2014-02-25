#include "clotho_application.h"

#include <cassert>

//template <>
//void ConfigurationManager< ClothoApplication >::initialize() {
//    cout << "Parsing: " << m_app->m_config_file << endl;
//}

ClothoApplication::ClothoApplication( const string & config ) :
    m_config_file( config ),
//    m_config( this ),
    m_sim_manager( NULL ),
    m_genetic_map( new GeneticMap() )
{}

void ClothoApplication::setSimulationManager( simulation_manager * manager ) {
    m_sim_manager = manager;
}

void ClothoApplication::initialize() {
    assert( m_sim_manager != NULL );

    cout << "ClothoApplication: Initializing" << endl;

//    m_config.initialize();

    cout << "ClothoApplication: Initialization Complete" << endl;
}

void ClothoApplication::finalize() {
    cout << "ClothoApplication: Finalizing" << endl;

    cout << "ClothoApplication: Finalization Complete" << endl;
}
