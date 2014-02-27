#ifndef INDIVIDUAL_H_
#define INDIVIDUAL_H_

#include "../clotho.h"
#include "engine/simulation_object.h"
#include "../event/clotho_event.h"
#include "../event_performer.h"

#include "GeneticReproduction.h"

#include "individual_properties.h"

class Individual : public SimulationObject< ClothoEventSet > {
public:
    friend class EventPerformer< Individual, ClothoEvent >;

    Individual( simulation_manager * manager,
                const system_id & env_id,
                GeneticReproduction * repro );

    virtual void initialize( );

    virtual void perform_event( const event * );

    virtual void finalize();

    virtual ~Individual() {
        if(m_prop) delete m_prop;
    }
protected:
    void handle_birth( const ClothoEvent * e );
    void handle_death( const ClothoEvent * e );

    system_id   m_env_id;
    IndividualProperties * m_prop;
    GeneticReproduction * m_repro;

    static EventPerformer< Individual, ClothoEvent > m_evt_performer;
};

#endif  // INDIVIDUAL_H_
