#include "individual_properties.h"

const system_id UNSET_ID(-1);

IndividualProperties::IndividualProperties( ) :
    mother(UNSET_ID),
    father(UNSET_ID),
    dob( SystemClock::getPositiveInfinity() ),
    eol( SystemClock::getPositiveInfinity() ),
    sex( UNASSIGNED ),
    m_genome( NULL )
{}

const system_id & IndividualProperties::getFather() const {
    return father;
}

const system_id & IndividualProperties::getMother() const {
    return mother;
}

const SystemClock::vtime_t & IndividualProperties::getDOB() const {
    return dob;
}

const SystemClock::vtime_t & IndividualProperties::getEOL() const {
    return eol;
}

genome * IndividualProperties::getGenome() const {
    return m_genome;
}

Sex IndividualProperties::getSex() const {
    return sex;
}

void IndividualProperties::setFather( const system_id & id ) {
    father = id;
}

void IndividualProperties::setMother( const system_id & id ) {
    mother = id;
}

void IndividualProperties::setDOB( const SystemClock::vtime_t & t ) {
    dob = t;
}

void IndividualProperties::setEOL( const SystemClock::vtime_t & t ) {
    eol = t;
}

void IndividualProperties::inheritFrom( const system_id & id, Sex s, zygote * g ){
    if( s == MALE ) {
        setFather( id );
        // father's allelegroup determines sex?
        m_genome->father = g;
        determineSex();
    } else if ( s == FEMALE ) {
        setMother( id );
        m_genome->mother = g;
    }
}

void IndividualProperties::determineSex() {

}

IndividualProperties::~IndividualProperties() {

}
