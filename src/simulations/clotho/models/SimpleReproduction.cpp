#include "SimpleReproduction.h"

#include <iostream>

using std::cout;
using std::endl;

SimpleReproduction::SimpleReproduction( shared_ptr< iRNG > rng ) : m_rng(rng) {
    assert( m_rng );
}

//
// Using human reproduction as an example.
// Each parent inherits half its genome from
// a male and a female.  In other words, each
// genome has source material (gametes) from 
// two parent genomes.  The gametes which the
// current genome inherits are assumed to be
// unique.
//
// SimpleReproduction
// randomly choses from their current genome
// one of their gametes, and clones it.
// Then it sets the source of this new gamete
// to be the current genome.
//
gamete * SimpleReproduction::reproduce( genome * g ) {
    if( ! g ) return NULL;

    //cout << "Getting ";
    // randomly choose a parent gamete to clone
    gamete::gamete_source_t src = (gamete::gamete_source_t) m_rng->nextInt( g->getMaxSources() );
    gamete * tmp = g->getGamete( src )->clone();

    //cout << " of " << (int) g->getSource() << ": ";

    //tmp->print( cout );
    //cout << " -> ";
    // the cloned gamete should have the source information
    // of the current genome
    tmp->setSource( g->getSource() );
    //tmp->print(cout);
    //cout << "\n";

    return tmp;
}

SimpleReproduction::~SimpleReproduction() {}
