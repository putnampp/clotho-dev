#ifndef _OBJECT_HPP_
#define _OBJECT_HPP_

#include "engine.h"

#include "system_object.h"
/*#include "event_interface.h"

#include "event_manager.hpp"
#include "event_processor.h"
#include "event_transceiver.h"*/

class object : public system_object
{
public:
    typedef SystemClock::vtime_t  vtime_t;

    inline const system_id & getSystemID() const {
        return m_id;
    }

    inline system_id::manager_id_t getManagerID() const {
        return m_id.getManagerID();
    }

    inline system_id::object_id_t getObjectID() const {
        return m_id.getObjectID();
    }
    virtual ~object() {}
protected:
    object( ) : m_id(0) {}
    object( system_id::manager_id_t man_id, system_id::object_id_t obj_id ) : m_id( man_id, obj_id ) {}

    object( const object & o ) : m_id( o.m_id) {}

    void setID( const system_id & id ) {
        m_id = id;
    }

    system_id   m_id;
};

#endif  // _OBJECT_HPP_
