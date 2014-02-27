#include "human_zygote.h"

#include <cassert>

const zygote::zygote_source_t HumanZygote::FROM_MOTHER = 0;
const zygote::zygote_source_t HumanZygote::FROM_FATHER = 1;

const zygote::zygote_type_t HumanZygote::X_TYPE = 0;
const zygote::zygote_type_t HumanZygote::Y_TYPE = 1;

HumanZygote::HumanZygote( zygote::zygote_source_t s, zygote::zygote_type_t t) : zygote(s , t) {}

HumanZygote::HumanZygote( const HumanZygote & z ) : zygote( z ) {}

zygote * HumanZygote::clone() const {
    return new HumanZygote( *this );
}

chromosome * HumanZygote::getChromosome( chromosome::chrom_id_t id ) {
    return NULL;
}

void HumanZygote::setChromosome( chromosome * c ) {
    assert(false);
}

HumanZygote::~HumanZygote() {}
