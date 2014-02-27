#ifndef ZYGOTE_H_
#define ZYGOTE_H_

#include "chromosome.h"

/*
 * A zygote is considered to be an indexed set of chromosomes
 */
class zygote {
public:
    typedef unsigned short zygote_key_t;
    typedef unsigned char zygote_source_t;
    typedef unsigned char zygote_type_t;

    zygote( ) : m_id(0) {}
    zygote( zygote_source_t s, zygote_type_t t ) : m_id(s, t ) {}
    zygote( const zygote & z ) : m_id( z.m_id ) {}
    
    virtual zygote * clone() const = 0;

    inline zygote_type_t getType() const {
        return m_id.id_component.type;
    }

    inline zygote_source_t getSource() const {
        return m_id.id_component.source;
    }

    void setSource( zygote_source_t id ) {
        m_id.id_component.source = id;
    }

    void setType( zygote_type_t id ) {
        m_id.id_component.type = id;
    }

    virtual chromosome * getChromosome( chromosome::chrom_id_t ) = 0;
    virtual void setChromosome( chromosome * ) = 0;

    virtual ~zygote() {}
protected:
    union _zygote_id {
        zygote_key_t    key;
        struct z {
            zygote_source_t source;
            zygote_type_t   type;
            z( zygote_source_t s, zygote_type_t t ) : source(s), type(t) {}
        } id_component;

        _zygote_id( ) : key(0) {}
        _zygote_id( zygote_key_t k ) : key(k) {}
        _zygote_id( zygote_source_t s, zygote_type_t t ) : id_component( s, t) {}
        _zygote_id( const _zygote_id & id ) : key( id.key ) {}
    };

    _zygote_id m_id;
};

#endif  // ZYGOTE_H_
