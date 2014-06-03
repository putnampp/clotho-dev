#ifndef OBJECT_HANDLER_H_
#define OBJECT_HANDLER_H_

template < class O >
struct object_handler {
    typedef void (O::*handler_t)();
};

#endif  // OBJECT_HANDLER_H_
