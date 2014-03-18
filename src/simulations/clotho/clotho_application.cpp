#include "clotho_application.h"
#include "object/environment.h"
#include "object/distributed_environment.h"

#include "models/SimpleReproduction.h"
#include "models/simple_random_selection.h"

#include <cassert>

//template <>
//void ConfigurationManager< ClothoApplication >::initialize() {
//    cout << "Parsing: " << m_app->m_config_file << endl;
//}

ClothoApplication::ClothoApplication( const string & config, shared_ptr< iRNG > rng, unsigned int nEnvs) :
    m_config_file( config ),
    m_sim_manager( NULL ),
    m_nEnvironments( nEnvs ),
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

    if( m_nEnvironments == 1 ) {
        setupEnvironment();
    } else if( m_nEnvironments > 0 ) {
        setupDistributedEnvironment();
    } else {
        // 0 environments cannot occur
        assert( false );
    }
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

void ClothoApplication::setupEnvironment() {
    cout << "Unpartitioned environment" << endl;
    Environment * env = new Environment( m_sim_manager, m_genetic_map, m_selection_model, m_reproduction_model );
    env->setFounderSize( m_nFounder );
    env->initialize();
    m_system_objs.push_back( env->getSystemID() );
}

void ClothoApplication::setupDistributedEnvironment() {
    cout << "Partitioning environment into " << m_nEnvironments << " parts" << endl;
    unsigned int nSize = m_nFounder / m_nEnvironments;

    DistributedEnvironment * root_env = new DistributedEnvironment( m_sim_manager, DefaultSystemID, m_genetic_map, m_selection_model, m_reproduction_model );
    root_env->setFounderSize( m_nFounder - ((m_nEnvironments - 1) * nSize ) );
    root_env->initialize();
    m_system_objs.push_back( root_env->getSystemID() );

    for( unsigned int i = 1; i < m_nEnvironments; ++i ) {
        DistributedEnvironment * denv = new DistributedEnvironment( m_sim_manager, root_env->getSystemID(), m_genetic_map, m_selection_model, m_reproduction_model );
        denv->setFounderSize( nSize );
        denv->initialize();

        m_system_objs.push_back( denv->getSystemID() );
        root_env->addNeighborEnvironment( denv->getSystemID() );
    }
}
