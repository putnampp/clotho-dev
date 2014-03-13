#ifndef CLOTHO_APPLICATION_H_
#define CLOTHO_APPLICATION_H_

#include "clotho.h"
#include "engine/application.h"
#include "engine/simulation_manager.h"
//#include "configuration_manager.h"
//
#include "GeneticMap.h"

#include "rng/rng.hpp"
#include "models/selection_model.h"
#include "reproduction.h"

#include <vector>

using std::vector;

class ClothoApplication : public application {
public:
//    friend class ConfigurationManager< ClothoApplication >;
    ClothoApplication( const string & config, shared_ptr< iRNG > );

    void setSimulationManager( simulation_manager * manager );

    void initialize();

    void finalize();

    virtual ~ClothoApplication();
protected:
    string  m_config_file;
//    ConfigurationManager< ClothoApplication > m_config;
    simulation_manager * m_sim_manager;

    GeneticMap::Ptr     m_genetic_map;
    shared_ptr< iRNG >  m_rng;

    vector< system_id > m_system_objs;
    selection_model *   m_selection_model;
    reproduction *      m_reproduction_model;
};

#endif  // CLOTHO_APPLICATION_H_
