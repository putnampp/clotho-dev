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

#include "RandomMatingModel.h"
#include <iostream>

#include <time.h>

#include <cmath>
#include <algorithm>

#include "../clothoobjects/events/ShellBirthEvent.h"

using std::cout;
using std::endl;
using std::random_shuffle;

RandomMatingModel::RandomMatingModel() : m_rng( gsl_rng_alloc( gsl_rng_taus ) ) {
    long seed = time(NULL);
    gsl_rng_set( m_rng, seed );
}

void RandomMatingModel::operator()( const ShellMaturityEvent * e, IndividualShell * ind ) {
    if( ind->getSex() == FEMALE ) {
        // females notifies environment when ready to mate
        //
        IntVTime date = dynamic_cast< const IntVTime & >( e->getReceiveTime() );
        IntVTime age = date - *ind->getBirthTime();

        unsigned int nOffspring = ind->getOffspringCount();

        // given age will she mate againt and produce offspring?
        if( readyToMate( age, nOffspring ) ) {
            Environment2 * env = ind->getEnvironment();
            Event * evt = new ShellMatingEvent( e->getReceiveTime(), date, ind, env, ind );
            env->receiveEvent( evt );
        } else if( age < 45 ) {
            // wait a bit and try again
            unsigned int rnd = gsl_rng_get( m_rng );

            IntVTime delay = date + (rnd % 3 ); // try again some time with in the next 3 years
            Event * evt = new ShellMaturityEvent( e->getReceiveTime(), delay, ind, ind );
            ind->receiveEvent( evt );
        }

    }
}

void RandomMatingModel::operator()( const ShellMatingEvent * e, Environment2 * env ) {
    int nMales = env->getMaleCount();

    // get a random male by index
    unsigned int rMale = gsl_rng_get( m_rng ) % nMales;

    IndividualShell * female = e->getFirstPartner();
    IndividualShell * male = env->getMaleAt( rMale );

    if( female && female->isAlive() ) {
        if( male && male->isAlive() ) {
            IndividualShell * offspring = env->nextAvailableIndividual();

            if( offspring ) {
                vector< genotype_t > genos;

                generateOffspringGenotype( female, male, genos );

                unsigned int rnd = gsl_rng_get( m_rng );
                sex_t s = ((rnd & 1 ) ? MALE : FEMALE);
                rnd >>= 1;

                IndividualProperties * ip = new IndividualProperties( female->getProperties(), male->getProperties(), s, genos );
                ip->m_dob = dynamic_cast< IntVTime * > (e->getReceiveTime().clone() );

                offspring->setProperties( ip );
                female->addOffspring();
                male->addOffspring();

                Event * eBorn = new ShellBirthEvent( e->getReceiveTime(), *ip->m_dob, offspring, offspring );
                offspring->receiveEvent( eBorn );

                IntVTime mTime = dynamic_cast< const IntVTime & >(e->getReceiveTime() ) + 1;
                Event * mE = new ShellMaturityEvent( e->getReceiveTime(), mTime, female, female );

                female->receiveEvent( mE );

                genos.clear();
            }
        }
    }
}

void RandomMatingModel::dump( ostream & out ) {

}

void RandomMatingModel::generateOffspringGenotype( IndividualShell * female, IndividualShell * male, vector< genotype_t > & genos ) {
    unsigned int nLoci = female->getEnvironmentLociCount();

    genos.reserve( nLoci );

    vector< allele_t > alleles(genotype_t::PLOIDY, (allele_t)ANCESTRAL_ALLELE );

    if( genotype_t::PLOIDY == 2 ) {
        unsigned int rnd = gsl_rng_get( m_rng );
        int j = 1, max_j = (sizeof( unsigned int ) << 3);   // max_j = 4 * 8 = 32
        for( unsigned int i = 0; i < nLoci; ++i ) {

            if( j >= max_j ) {
                rnd = gsl_rng_get( m_rng );
                j = 1;
            }

            // routine only works if ploidy == 2
            alleles[0] = female->alleleAt( i, (rnd & 1));
            rnd >>= 1;
            alleles[1] = male->alleleAt( i, (rnd & 1));
            rnd >>= 1;

            j += 2;

            genotype_t g(alleles);
            genos.push_back(g);
        }
    } else {
        // more general outline. Note that it assumes a parent allele
        // can serve as the variant source multiple times.
        unsigned int from_male = genotype_t::PLOIDY / 2;

        vector< ploidy_t > males;
        vector< ploidy_t > females;

        for( ploidy_t i = 0; i < genotype_t::PLOIDY; ++i ) {
            males.push_back( i );
            females.push_back( i );
        }

        for( unsigned int i = 0; i < nLoci; ++i ) {
            // shuffle the chromosomes for each locus?
            // treat each locus as being independent of one another
            random_shuffle( males.begin(), males.end() );
            random_shuffle( females.begin(), females.end() );
            for( unsigned int k = 0; k < from_male; ++k ) {
                alleles[ k ] = male->alleleAt( i, males[k] );
            }
            for( unsigned int k = from_male; k < genotype_t::PLOIDY; ++k ) {
                alleles[k] = female->alleleAt( i, females[k - from_male] );
            }
            genotype_t g( alleles );
            genos.push_back(g);
        }
    }
}

bool RandomMatingModel::readyToMate( IntVTime & age, unsigned int offspring ) {
    return (age < 45 && age >= 21 && offspring <= 3 );
}

RandomMatingModel::~RandomMatingModel() {
    gsl_rng_free( m_rng );
}
