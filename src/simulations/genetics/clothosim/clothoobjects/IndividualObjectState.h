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
#ifndef INDIVIDUALOBJETSTATE_H_
#define INDIVIDUALOBJETSTATE_H_

#include "warped/warped.h"
#include "warped/State.h"

#include "common.h"
#include "Genotype.h"
#include <vector>

using std::vector;

enum Sex { FEMALE, MALE, UNK_SEX };
//enum Genotype { HOMOZYGOUS_REF, HETEROZYGOUS, HOMOZYGOUS_ALT };

typedef Sex     sex_t;
typedef size_t  variant_index_t;
typedef VTime   age_t;


//typedef Genotype< 2 > genotype_t;
typedef double  phenotype_t;

static const int MAX_VARIANTS = 100;
static const ploidy_t DEFAULT_PLOID = 2;

class IndividualObjectState : public State {
public:
    IndividualObjectState(const VTime & t, sex_t s = UNK_SEX) :
        m_dob( t.clone() ),
        m_sex( UNK_SEX ),
//        m_genotypes( new vector< genotype_t > ),
        m_phenotype(0.0) {
//        m_genotypes->reserve( MAX_VARIANTS );
    }

    void    copyState( const State * toCopy ) {
        ASSERT( toCopy );
        const IndividualObjectState * c = static_cast< const IndividualObjectState * >(toCopy);

        m_dob = c->m_dob->clone();
        m_sex = c->m_sex;

//        m_genotypes->insert(m_genotypes->begin(), c->m_genotypes->begin(), c->m_genotypes->end() );
        m_phenotype = c->m_phenotype;
    }

    unsigned int getStateSize() const {
        return sizeof(IndividualObjectState);
    }

    sex_t   getSex() const { return m_sex; }
    void    setSex( sex_t s) { m_sex = s; }

    const age_t &   getDOB() const { return *m_dob; }
    void            setDOB( const VTime & t ) {
        m_dob = t.clone();
    }

//    const genotype_t & getGenotypeAt( variant_index_t idx ) const {
//        return m_genotypes->at( idx );
//    }

    phenotype_t getPhenotype() const { return m_phenotype; }

    virtual ~IndividualObjectState() {
        delete m_dob;
//        delete m_genotypes;
    }
private:
    age_t *         m_dob;
    sex_t           m_sex;
//    vector< genotype_t > * m_genotypes;
    phenotype_t     m_phenotype;
};

#endif  // INDIVIDUALOBJETSTATE_H_
