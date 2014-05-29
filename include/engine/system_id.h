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

    inline system_id() : object_id(0), manager_id(0) { }

//    inline system_id( system_key_t key ) : m_id(key) { }

//    inline system_id( manager_id_t man, object_id_t obj ) : m_id( obj, man ) { }
    inline system_id( manager_id_t man, object_id_t obj ) : object_id( obj ), manager_id(man) { }

//    inline system_id( const system_id & other ) : m_id( other.m_id ) { }
    inline system_id( const system_id & other ) : object_id( other.object_id ), manager_id(other.manager_id) { }

    inline system_key_t getKey() const {
        system_key_t id = manager_id;
        id <<= (sizeof(object_id_t) * 8);
        id += object_id;

        return id;
    }

    inline manager_id_t getManagerID() const {
        return manager_id;
    }

    inline object_id_t getObjectID() const {
        return object_id;
    }

    inline const system_id& operator=( const system_id & rhs ) {
        //m_id.key = rhs.m_id.key;
        object_id = rhs.object_id;
        manager_id = rhs.manager_id;
        return *this;
    }

    inline bool operator<( const system_id & rhs ) const {
//        return m_id.key < rhs.m_id.key;
        return ( manager_id < rhs.manager_id || (manager_id == rhs.manager_id && object_id < rhs.object_id ) );
    }

    inline bool operator==( const system_id & rhs ) const {
//        return m_id.key == rhs.m_id.key;
        return manager_id == rhs.manager_id && object_id == rhs.object_id;
    }

    inline bool operator!=( const system_id & rhs ) const {
//        return m_id.key != rhs.m_id.key;
        return manager_id != rhs.manager_id || object_id != rhs.object_id;
    }

    virtual ~system_id() {}
private:
//    union _system_id {
//        system_key_t    key;
//        struct c {
//            object_id_t     object_id;
//            manager_id_t    manager_id;
//            c( object_id_t oid, manager_id_t mid ) : object_id(oid), manager_id(mid) {}
//        } components;
//
//        _system_id( ) : key(0) {}
//        _system_id( system_key_t k ) : key( k ) {}
//        _system_id( object_id_t oid, manager_id_t mid) : components(oid, mid) {}
//        _system_id( const _system_id & id ) : key( id.key) {}
//    };
//
//    _system_id m_id;
    object_id_t object_id;
    manager_id_t manager_id;
};

static const system_id DefaultSystemID;

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
