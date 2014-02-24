#ifndef EVENT_SET_H_
#define EVENT_SET_H_

template < class OBJ >
struct event_set {
    typedef std::list< const event * >  event_set_t;
};

#endif  // EVENT_SET_H_
