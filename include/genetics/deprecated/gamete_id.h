#ifndef GAMETE_ID_H_
#define GAMETE_ID_H_

#include <type_traits>

template < class K, class S, class T, class E= void >
union _gamete_id;

template < class K, class S, class T >
union _gamete_id<K, S, T, typename std::enable_if< sizeof(K) >= sizeof(S) + sizeof(T) >::type > {
    typedef K gamete_key_t;
    typedef S gamete_source_t;
    typedef T gamete_type_t;

    K    key;
    struct z {
        gamete_source_t source;
        gamete_type_t   type;
        z( gamete_source_t s, gamete_type_t t ) : source(s), type(t) {}
    } id_component;

    _gamete_id( ) : key(0) {}
    _gamete_id( K k ) : key(k) {}
    _gamete_id( gamete_source_t s, gamete_type_t t ) : id_component( s, t) {}
    _gamete_id( const _gamete_id< K, S, T > & id ) : key( id.key ) {}
};

#endif  // GAMETE_ID_H_
