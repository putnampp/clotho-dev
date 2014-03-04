#ifndef HUMAN_ZYGOTE_H_
#define HUMAN_ZYGOTE_H_

#include "../clotho.h"
#include "zygote.h"

#include <vector>

using std::vector;

class HumanZygote : public zygote {
public:
    static const zygote::zygote_source_t FROM_MOTHER;
    static const zygote::zygote_source_t FROM_FATHER;

    static const zygote::zygote_type_t X_TYPE;
    static const zygote::zygote_type_t Y_TYPE;

    HumanZygote( zygote::zygote_source_t s, zygote_type_t t, size_t );
    HumanZygote( const HumanZygote & z);
    
    zygote * clone() const;

    chromosome * getChromosome( chromosome::chrom_id_t );
    void setChromosome( chromosome * );

    virtual ~HumanZygote();
protected:
    size_t          m_nAlleles;
    unsigned char * m_alleles;
};


#endif  // HUMAN_ZYGOTE_H_
