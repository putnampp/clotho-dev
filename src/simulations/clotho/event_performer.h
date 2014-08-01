#ifndef EVENT_PERFORMER_H_
#define EVENT_PERFORMER_H_

#include <cstring>
#include <unordered_map>

#include <iostream>

#include "ider.h"

using std::cout;
using std::endl;

using std::unordered_map;

template < class O, class E, class ID = IDer< E > >
class EventPerformer {
public:
    typedef void (O::*performer_t)( const E * );
    EventPerformer() :
        m_performer_map() {
        initialize();
    }

    void addHandler( const typename ID::id_type_t & id, performer_t p ) {
        m_performer_map.insert( std::make_pair( id, p ) );
    }

    void operator()( O * o, const E * e ) {
        if( o == NULL || e == NULL ) return;

        performer_t p = m_performer_map[ m_ider( e ) ];
        (o->*p)( e );
    }

    virtual ~EventPerformer() {}
protected:
    void initialize();

    unordered_map< typename ID::id_type_t, performer_t > m_performer_map;
    ID  m_ider;
};

#endif  // EVENT_PERFORMER_H_
