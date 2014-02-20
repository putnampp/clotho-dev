#ifndef _OBJECT_H_
#define _OBJECT_H_

#include "system_id.h"

struct object {
    virtual const system_id &       getSystemID() const = 0;
    virtual system_id::object_id_t  getObjectID() const = 0;

    virtual void setID( const system_id & id ) = 0;

    virtual void setSimulationManager( simulation_manager * sim ) = 0;

    virtual void sendEvent( event_interface * evt ) = 0;
    virtual void receiveEvent( event_interface * evt ) = 0;
};

#endif  // _OBJECT_H_
