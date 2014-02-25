#ifndef CLOTHO_APPLICATION_H_
#define CLOTHO_APPLICATION_H_

#include "clotho.h"
#include "engine/application.h"
#include "engine/simulation_manager.h"
//#include "configuration_manager.h"
//
#include "GeneticMap.h"

class ClothoApplication : public application {
public:
//    friend class ConfigurationManager< ClothoApplication >;
    ClothoApplication( const string & config );

    void setSimulationManager( simulation_manager * manager );

    void initialize();

    void finalize();

    virtual ~ClothoApplication() {}
protected:
    string  m_config_file;
//    ConfigurationManager< ClothoApplication > m_config;
    simulation_manager * m_sim_manager;

    GeneticMap::Ptr     m_genetic_map;
};

#endif  // CLOTHO_APPLICATION_H_
