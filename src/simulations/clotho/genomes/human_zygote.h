#ifndef HUMAN_ZYGOTE_H_
#define HUMAN_ZYGOTE_H_

#include "../clotho.h"
#include "zygote.h"

class HumanZygote : public zygote {
public:
    static const zygote::zygote_source_t FROM_MOTHER;
    static const zygote::zygote_source_t FROM_FATHER;

    static const zygote::zygote_type_t X_TYPE;
    static const zygote::zygote_type_t Y_TYPE;

    HumanZygote( zygote::zygote_source_t s, zygote_type_t t );
    HumanZygote( const HumanZygote & z);
    
    zygote * clone() const;

    chromosome * getChromosome( chromosome::chrom_id_t );
    void setChromosome( chromosome * );

    virtual ~HumanZygote();
};


#endif  // HUMAN_ZYGOTE_H_