#ifndef _OBJECT_H_
#define _OBJECT_H_

#include "engine.h"

#include "system_object.h"
#include "event_interface.h"

#include "event_manager.h"
#include "event_processor.h"
#include "event_transceiver.h"

struct simulation_manager;

struct object :
        virtual public system_object,
        virtual public event_manager,
        virtual public event_processor,
        virtual public event_transceiver {
    typedef event::vtime_t  vtime_t;

//    virtual const system_id &       getSystemID() const = 0;
//    virtual system_id::object_id_t  getObjectID() const = 0;

//    virtual void setID( const system_id & id ) = 0;
//    virtual void initialize() = 0;
//    virtual void finalize() = 0;

    virtual void setSimulationManager( simulation_manager * sim ) = 0;

    /*
        virtual void process() = 0;
        virtual void perform_event( const event * ) = 0;

        virtual size_t pendingEventCount() = 0;
        virtual const event * getEvent() = 0;
        virtual const event * peekEvent() = 0;

        virtual void sendEvent( const event * evt ) = 0;
        virtual void receiveEvent( const event * evt ) = 0;
    */

    virtual void updateLocalTime( const vtime_t & t ) = 0;
    virtual const vtime_t & getCurrentTime() const = 0;

    virtual ~object() {}
};

#endif  // _OBJECT_H_
