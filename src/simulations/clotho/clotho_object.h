#ifndef CLOTHO_OBJECT_H_
#define CLOTHO_OBJECT_H_

#include "clotho.h"

class ClothoObject : public SimulationObject< ClothoEventSet > {
public:
    ClothoObject( ) : SimulationObject< ClothoEventSet >() {}
    ClothoObject( simulation_manager * manager ) : SimulationObject< ClothoEventSet >( manager ) {}

    virtual void initialize() {
        SimulationObject< ClothoEventSet >::initialize();
    }

    virtual void finalize() {
        SimulationObject< ClothoEventSet >::finalize();
    }

    virtual ~ClothoObject() {}
};

#endif  // CLOTHO_OBJECT_H_
