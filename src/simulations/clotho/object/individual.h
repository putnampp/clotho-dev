#ifndef INDIVIDUAL_H_
#define INDIVIDUAL_H_

#include "../clotho.h"
#include "engine/simulation_object.h"

#include "GeneticReproduction.h"

#include "individual_properties.h"

class Individual : public SimulationObject< ClothoEventSet > {
public:
    Individual( simulation_manager * manager,
                const system_id & env_id,
                GeneticReproduction * repro );

    virtual void initialize( );

    virtual void perform_event( const event * );

    virtual void finalize();

    virtual ~Individual() {}
protected:
    system_id   m_env_id;
    IndividualProperties * m_prop;
    GeneticReproduction * m_repro;
};

#endif  // INDIVIDUAL_H_
