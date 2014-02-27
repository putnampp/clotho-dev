#ifndef INDIVIDUAL_PROPERTIES_H_
#define INDIVIDUAL_PROPERTIES_H_

#include "../clotho.h"

extern const system_id UNSET_ID;

class IndividualProperties {
public:
    IndividualProperties( );

    const system_id & getFather() const;
    const system_id & getMother() const;

    const SystemClock::vtime_t & getDOB() const;
    const SystemClock::vtime_t & getEOL() const;

    void setDOB( const SystemClock::vtime_t & t );
    void setEOL( const SystemClock::vtime_t & t );
    
    void setFather( const system_id & id);
    void setMother( const system_id & id);

    void inheritFrom( const system_id & id, Sex s, AlleleGroupPtr genos );

    virtual ~IndividualProperties();
protected:
    system_id mother, father;

    SystemClock::vtime_t dob, eol;
    Sex   sex;
    AlleleGroupPtr genotypes;

};

#endif  // INDIVIDUAL_PROPERTIES_H_
