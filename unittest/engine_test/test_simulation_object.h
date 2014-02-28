#ifndef TEST_SIMULATION_OBJECT_H_
#define TEST_SIMULATION_OBJECT_H_

#include "simulation_object.h"
#include <list>
#include <iostream>

#include "engine/FSBAllocator.hh"

class MyTestSO : public SimulationObject< std::list< const event *, FSBAllocator< const event * >  > > {
public:
    MyTestSO( ) : 
        SimulationObject< std::list< const event *, FSBAllocator< const event * > > >( ),
        m_process_count(0) 
    {}

    MyTestSO( simulation_manager * manager ) : 
        SimulationObject< std::list< const event *, FSBAllocator< const event * > > >( manager ),
        m_process_count(0)
    {
        setSimulationManager( manager );
    }

    virtual void process() {
        ++m_process_count;
        std::cout << "Process called: " << std::endl;
    }

    virtual ~MyTestSO() {}

protected:
    unsigned int m_process_count;
};


#endif  // TEST_SIMULATION_OBJECT_H_
