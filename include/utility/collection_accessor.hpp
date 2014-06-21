#ifndef COLLECTION_ACCESSOR_HPP_
#define COLLECTION_ACCESSOR_HPP_

template < class C, class K, class V >
struct collection_accessor {
    typedef C collection_t;
    typedef K key_t;
    typedef V result_t;

    inline result_t operator()( const collection_t & c, const key_t k ) {
        return c[k];
    }
};

#endif  // COLLECTION_ACCESSOR_HPP_
