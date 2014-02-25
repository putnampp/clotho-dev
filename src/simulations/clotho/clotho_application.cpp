#include "clotho_application.h"
//#include "object/individual.h"
#include "object/environment.h"

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

    Environment * env = new Environment( m_sim_manager );
    env->initialize();

    m_system_objs.push_back( env->getSystemID() );
    for( int i = 0; i < 10; ++i ) {
        system_id obj_id = env->getIndividual();
        object * obj = m_sim_manager->getObject( obj_id );

        obj->initialize();

        m_system_objs.push_back( obj_id );
    }

    cout << "ClothoApplication: Initialization Complete" << endl;
}

void ClothoApplication::finalize() {
    cout << "ClothoApplication: Finalizing" << endl;

    size_t nProcessedEvents = 0;
    while( !m_system_objs.empty() ) {
        system_id tmp = m_system_objs.back();
        m_system_objs.pop_back();
        
        object * tmp_obj = m_sim_manager->getObject( tmp );
        if( tmp_obj ) {
            cout << "ClothoApplication: finializing object " << tmp << endl;
            nProcessedEvents += tmp_obj->processedEventCount();
            tmp_obj->finalize();
        }
    }

    cout << "ClothoApplication: Events Processed = " << nProcessedEvents << endl;
    cout << "ClothoApplication: Finalization Complete" << endl;
}
