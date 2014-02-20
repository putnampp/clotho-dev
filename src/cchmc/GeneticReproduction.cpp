#include "GeneticReproduction.h"

GeneticReproduction::GeneticReproduction( GeneticMap::Ptr gm, recombination_model * recomb, mutation_model * mutate ) :
    m_genetic_map( gm ),
    m_recomb( recomb ),
    m_inherit( inherit ),
    m_mutate( mutate ),
    m_p0_tmp( gm->createLociAlleles() ),
    m_p1_tmp( gm->createLociAlleles() ),
    m_offspring_tmp( gm->createLociAlleles() )
{}

void GeneticReproduction::reproduce( AlleleGroupPtr p0, AlleleGroupPtr p1, AlleleGroupPtr offspring ) {
    assert( m_inherit );

    AlleleGroupPtr p0_recomb = p0;
    AlleleGroupPtr p1_recomb = p1;

    if( m_recomb ) {
        p0_recomb = m_p0_tmp;
        p1_recomb = m_p1_tmp;

        m_recomb->recombine( p0, p0_recomb );
        m_recomb->recombine( p1, p1_recomb );
    }

    m_inherit->inherit( p0_recomb, p1_recomb, offspring );

    if( m_mutate ) {
        m_mutate->mutate( offspring );
    }
}

GeneticReproduction::~GeneticReproduction() {
    deallocateAlleleGroup( m_p0_tmp );
    deallocateAlleleGroup( m_p1_tmp );
}
