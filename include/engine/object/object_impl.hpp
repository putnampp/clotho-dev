#ifndef OBJECT_COMMON_IMPL_HPP_
#define OBJECT_COMMON_IMPL_HPP_

#include "object.hpp"

//#include "simulation_manager.hpp"

#include <iostream>

using std::cout;
using std::endl;

class Object : public object {
public:
    inline const system_id & getSystemID() const {
        return m_id;
    }

    inline system_id::manager_id_t getManagerID() const {
        return m_id.getManagerID();
    }

    inline system_id::object_id_t getObjectID() const {
        return m_id.getObjectID();
    }

    virtual ~Object() { }

protected:
    Object( ) :
        m_id(0),
        m_local_time( SystemClock::ZERO ),
        m_next_eid( 0 )
    {}

    void setID( const system_id & id ) {
        m_id = id;
    }


    system_id   m_id;
};

#endif  // OBJECT_COMMON_IMPL_HPP_
