/*******************************************************************************
 * Copyright (c) 2013, Patrick P. Putnam (putnampp@gmail.com)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are those
 * of the authors and should not be interpreted as representing official policies,
 * either expressed or implied, of the FreeBSD Project.
 ******************************************************************************/

#include "KAlleleMutationModel.h"

#include <time.h>

KAlleleMutationModel::KAlleleMutationModel( double rate ) :
    m_rate( rate ),
    m_rng( gsl_rng_alloc(gsl_rng_taus) ),
    m_poisson( rate )
{
    long seed = time(NULL);
    gsl_rng_set( m_rng, seed );

    m_poisson.setRandomNumberGenerator( m_rng );
    m_uniform.setRandomNumberGenerator( m_rng );
}

KAlleleMutationModel::~KAlleleMutationModel() {
    gsl_rng_free( m_rng );
}

void KAlleleMutationModel::operator()( const BirthEvent * be, Individual * ind ) {

}

void KAlleleMutationModel::operator()( const ShellBirthEvent * sbe, IndividualShell * is ) {
    IndividualProperties * ip = is->getProperties();
    AlleleGroupPtr genos = ip->m_genos;
    if( genos->empty() ) return;

    if( ALLELE_COPIES == 2 ) {
        unsigned int nMut = m_poisson.nextVariate( m_rate * (genos->size() << 1) );
        while( nMut-- > 0 ) {
            unsigned long int locus_idx = m_uniform.nextVariate( (unsigned long int) genos->size());
            unsigned long int maxAllele = is->getEnvironment()->getGeneticMap()->getMaxAlleles( locus_idx );
            allele_t newAllele;
            if( m_uniform.nextBoolean() ) {
                do {
                    newAllele = m_uniform.nextVariate( maxAllele );
                } while ( newAllele == (*genos)[ locus_idx ][ 1 ] );
                (*genos)[locus_idx][1] = newAllele;
            } else {
                do {
                    newAllele = m_uniform.nextVariate( maxAllele );
                } while( newAllele == (*genos)[ locus_idx ][ 0 ] );
                (*genos)[locus_idx][0] = newAllele;
            }
        }
    }
}

void KAlleleMutationModel::dump( ostream & out ) {

}
