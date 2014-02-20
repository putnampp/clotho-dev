#ifndef _EVENT_INTERFACE_H_
#define _EVENT_INTERFACE_H_

#include "system_id.h"
//#include "orderable.h"

//template < class CLOCK >
//class event : virtual public orderable< CLOCK > {
//public:
//    virtual void execute() = 0;
//    virtual bool operator<( const orderable< CLOCK > & rhs ) const = 0;
//    virtual ~event() {}
//protected:
//    event( const CLOCK time ) : orderable( time ) {}
//};
//

struct event {
    typedef unsigned int event_id_t;

    virtual event_id_t          getEventID()    const = 0;
    virtual const system_id &   getSender()     const = 0;
    virtual const system_id &   getReceiver()   const = 0;

    virtual void execute() = 0;

    virtual bool operator<( const event * rhs ) const = 0;
    virtual bool operator==( const event * rhs ) const = 0;
};

#endif  // _EVENT_INTERFACE_H_
