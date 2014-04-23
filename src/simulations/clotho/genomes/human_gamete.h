#ifndef HUMAN_ZYGOTE_H_
#define HUMAN_ZYGOTE_H_

#include "../clotho.h"
#include "gamete.h"

#include <vector>

using std::vector;

class HumanGamete : public gamete {
public:
    static const gamete::gamete_source_t FROM_MOTHER;
    static const gamete::gamete_source_t FROM_FATHER;

    static const gamete::gamete_type_t X_TYPE;
    static const gamete::gamete_type_t Y_TYPE;

    HumanGamete( gamete::gamete_source_t s, gamete_type_t t, size_t );
    HumanGamete( const HumanGamete & z);
    
    gamete * clone() const;

//    chromosome * getChromosome( chromosome::chrom_id_t );
//    void setChromosome( chromosome * );

    virtual void print( std::ostream & o );

    virtual ~HumanGamete();
protected:
    size_t          m_nAlleles;
    unsigned char * m_alleles;
};


#endif  // HUMAN_ZYGOTE_H_
