#ifndef HUMAN_GENOME_H_
#define HUMAN_GENOME_H_

#include "../clotho.h"
#include "genome.h"

#include <vector>

using std::vector;

#include "human_zygote.h"

class HumanGenome : public genome {
public:
    HumanGenome( HumanZygote * hz0, HumanZygote * hz1 );
    HumanGenome( const HumanGenome & );

    genome * clone() const;

    inline zygote::zygote_source_t getSource() const {
        return ((m_sex == MALE) ? HumanZygote::FROM_FATHER : HumanZygote::FROM_MOTHER );
    }

    inline zygote::zygote_source_t getMaxSources() const {
        return 2;
    }

    Sex getSex() const;

    zygote * getZygote( zygote::zygote_source_t ) const;
    zygote * getZygote( Sex ) const;

    void setZygote( zygote * );
    void setZygote( HumanZygote * );

    virtual ~HumanGenome();
protected:
    HumanGenome();
    HumanZygote * m_mother, * m_father;
    Sex m_sex;
};

#endif  // HUMAN_GENOME_H_
