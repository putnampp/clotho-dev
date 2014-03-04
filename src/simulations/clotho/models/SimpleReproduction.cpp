#include "SimpleReproduction.h"

SimpleReproduction::SimpleReproduction() : m_source(0) {}

zygote * SimpleReproduction::reproduce( genome * g ) {
    if( ! g ) return NULL;

    if( m_source >= g->getMaxSources() ) {
        m_source = 0;
    }

    zygote * tmp = g->getZygote( m_source++ )->clone();
    tmp->setSource( g->getSource() );

    return tmp;
}

SimpleReproduction::~SimpleReproduction() {}
