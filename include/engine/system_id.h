#ifndef SYSTEMID_H_
#define SYSTEMID_H_

#include <iostream>
#include <functional>

using std::ostream;

class system_id {
public:
    typedef unsigned int manager_id_t;
    typedef unsigned int object_id_t;
    typedef unsigned long system_key_t;

    inline system_id() { 
        m_id.key = 0; 
    }

    inline system_id( system_key_t key ) { 
        m_id.key = key; 
    }

    inline system_id( manager_id_t man, object_id_t obj ) {
        m_id.object_id = obj;
        m_id.manager_id = man;
    }

    inline system_id( const system_id & other ) {
        m_id.key = other.m_id.key;
    }

    inline system_key_t getKey() const {
        return m_id.key;
    }

    inline manager_id_t getManagerID() const {
        return m_id.manager_id;
    }

    inline object_id_t getObjectID() const {
        return m_id.object_id;
    }

    inline const system_id& operator=( const system_id & rhs ) {
        m_id.key = rhs.m_id.key;
        return *this;
    }

    inline bool operator<( const system_id & rhs ) const {
        return m_id.key < rhs.m_id.key;
    }

    inline bool operator==( const system_id & rhs ) const {
        return m_id.key == rhs.m_id.key;
    }

    virtual ~system_id() {}
private:
    union _system_id {
        system_key_t    key;
        struct {
            object_id_t     object_id;
            manager_id_t    manager_id;
        };
    };

    _system_id m_id;
};

inline ostream & operator<<( ostream & out, const system_id & id ) {
    out << "[" << id.getManagerID() << "," << id.getObjectID() << "]";
    return out;
}

namespace std {

template <>
struct hash< system_id > {
    typedef system_id   argument_type;
    typedef size_t      value_type;

    hash< system_id::system_key_t > key_hasher;
    value_type operator() ( argument_type const & id ) const {
        return key_hasher( id.getKey() );
    }
};

}

#endif  // SYSTEMID_H_
