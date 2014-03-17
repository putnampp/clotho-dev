#include "clotho_application.h"
#include "object/environment.h"

#include "models/SimpleReproduction.h"
#include "models/simple_random_selection.h"

#include <cassert>

//template <>
//void ConfigurationManager< ClothoApplication >::initialize() {
//    cout << "Parsing: " << m_app->m_config_file << endl;
//}

ClothoApplication::ClothoApplication( const string & config, shared_ptr< iRNG > rng) :
    m_config_file( config ),
    m_sim_manager( NULL ),
    m_genetic_map( new GeneticMap() ),
    m_rng( rng ),
    m_selection_model( new SimpleRandomSelection( m_rng ) ),
    m_reproduction_model( new SimpleReproduction() ),
    m_nFounder(10000)
{}

ClothoApplication::~ClothoApplication() {
    if( m_selection_model ) delete m_selection_model;
    if( m_reproduction_model ) delete m_reproduction_model;
}

void ClothoApplication::setSimulationManager( simulation_manager * manager ) {
    m_sim_manager = manager;
}

void ClothoApplication::setFounderSize( unsigned int s ) {
    m_nFounder = s;
}

void ClothoApplication::initialize() {
    assert( m_sim_manager != NULL );

    Environment * env = new Environment( m_sim_manager, m_genetic_map, m_selection_model, m_reproduction_model );
    env->setFounderSize( m_nFounder );
    env->initialize();

    m_system_objs.push_back( env->getSystemID() );
}

void ClothoApplication::finalize() {
    while( !m_system_objs.empty() ) {
        system_id tmp = m_system_objs.back();
        m_system_objs.pop_back();
        
        object * tmp_obj = m_sim_manager->getObject( tmp );
        if( tmp_obj ) {
            tmp_obj->finalize();
            delete tmp_obj;
        }
    }
}
