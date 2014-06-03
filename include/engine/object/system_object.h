#ifndef SYSTEM_OBJECT_H_
#define SYSTEM_OBJECT_H_

#include "../system_id.h"

struct system_object {
    virtual const system_id &       getSystemID() const = 0;
    virtual system_id::manager_id_t getManagerID() const = 0;
    virtual system_id::object_id_t  getObjectID() const = 0;
//    virtual void setID( const system_id & id ) = 0;

    virtual void initialize() = 0;
    virtual void finalize() = 0;
};


#endif  // SYSTEM_OBJECT_H_
