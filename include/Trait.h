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
#ifndef TRAIT_H_
#define TRAIT_H_

#include "common.h"
#include "ploidy.h"
#include "Configurable.h"
#include "Mutate.h"
#include "Inheritance.h"

#include <cassert>

struct iTrait {

/**
 *  return the ploidy of the trait
 */
    virtual ploidy_t    ploidy()    const = 0;

/**
 *  return the number of loci associated with the trait
 */
    virtual uint32_t    loci()      const = 0;

/**
 *  return the number of possible alleles
 */
    virtual uint32_t    alleles()   const = 0;
};

/*******************************************************************************
 * A Trait is some observed characteristic of an individual.
 *
 * A set of Traits is used to define a Phenotype.
 *
 * There are two types of traits: categorical or quantitative.
 *
 * Quantitative traits are represented by a measured value or quantity. For 
 * example, height is considered to be a quantitative value.
 *
 * Qualitative traits fall into a general set of values. For example, eye color
 * is a categorical trait, and is limited to a set of colors.
 *
 ******************************************************************************/
template < class V = Byte, unsigned char P = DIPLOID >
class Trait : public iTrait, 
    public Configurable, 
    virtual MutatableSequence< V, P>, 
    virtual InheritableSequence<V, P>,
    virtual Sequence<V,P> {
public:
    String getName() const;
    String getDescription() const;

    virtual void configure( std::istream & config );

    virtual ~Trait() {    }
private:
};

#endif  // TRAIT_H_
