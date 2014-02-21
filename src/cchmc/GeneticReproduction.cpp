#include "GeneticReproduction.h"

GeneticReproduction::GeneticReproduction( GeneticMap::Ptr gm, recombination_model * recomb, mutation_model * mutate ) :
    m_genetic_map( gm ),
    m_recomb( recomb ),
    m_inherit( inherit ),
    m_mutate( mutate ),
    m_p0_tmp( gm->createLociAlleles() ),
    m_offspring_tmp( gm->createLociAlleles() )
{}

void GeneticReproduction::reproduce( AlleleGroupPtr p0, AlleleGroupPtr offspring ) {
    assert( m_inherit );

    AlleleGroupPtr p0_recomb = p0;

    if( m_recomb ) {
        p0_recomb = m_p0_tmp;

        m_recomb->recombine( p0, p0_recomb );
    }

    if( m_mutate ) {
        m_mutate->mutate( offspring );
    }
}

GeneticReproduction::~GeneticReproduction() {
    deallocateAlleleGroup( m_p0_tmp );
    deallocateAlleleGroup( m_p1_tmp );
}
