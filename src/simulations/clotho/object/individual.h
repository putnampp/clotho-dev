#ifndef INDIVIDUAL_H_
#define INDIVIDUAL_H_

#include "../clotho.h"
#include "engine/simulation_object.h"

#include "GeneticReproduction.h"

#include "individual_properties.h"

class Individual : public SimulationObject< ClothoEventSet > {
public:
    Individual( simulation_manager * manager,
                GeneticReproduction * repro ) :
        SimulationObject( manager ),
        m_prop( new IndividualProperties() ),
        m_repro( repro )
    {
        setSimulationManager( manager );
    }

    virtual void perform_event( const event * );

    virtual ~Individual() {}
protected:
    IndividualProperties * m_prop;
    GeneticReproduction * m_repro;
};

#endif  // INDIVIDUAL_H_
