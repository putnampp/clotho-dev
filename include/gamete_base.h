#ifndef GAMETE_BASE_H_
#define GAMETE_BASE_H_

#include "gamete_id.h"

class gamete_base {
public:
    typedef unsigned short gamete_key_t;
    typedef unsigned char gamete_source_t;
    typedef unsigned char gamete_type_t;

    typedef _gamete_id< gamete_key_t, gamete_source_t, gamete_type_t > gamete_id_t;
    
    gamete_base( ) : m_id(0) {}
    gamete_base( gamete_source_t s, gamete_type_t t ) : m_id(s, t ) {}
    gamete_base( const gamete_id_t & cid ) : m_id( cid ) {}

    inline gamete_type_t getType() const {
        return m_id.id_component.type;
    }

    inline gamete_source_t getSource() const {
        return m_id.id_component.source;
    }

    void setSource( gamete_source_t id ) {
        m_id.id_component.source = id;
    }

    void setType( gamete_type_t id ) {
        m_id.id_component.type = id;
    }

    virtual ~gamete_base() {}

protected:
    gamete_id_t m_id;
};

#endif  // GAMETE_BASE_H_
