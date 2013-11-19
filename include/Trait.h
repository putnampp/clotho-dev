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

#include "Genotype.h"
#include "Phenotype.h"

#include <functional>

class Trait : 
    virtual public MultilocusGenotyper,
    virtual public phenotyper {
public:

    const string & getName() const { return m_name; }

    virtual bool addIndexedGenotyper( size_t idx, LocusGenotyper * lg ) = 0;
    virtual size_t getLociCount() const = 0;

    virtual double genotype( const AlleleGroup * ag ) const = 0;
    virtual double phenotype( const AlleleGroup * ag, const environmental * env ) const = 0;

    virtual bool operator==( const Trait & rhs ) const {    return m_name == rhs.m_name; }

    virtual ~Trait() {}
protected:
    Trait( const char * n ) : m_name(n) {}

    Trait( const string & n ) : m_name( n ) {}

    string m_name;
};

typedef shared_ptr< Trait > TraitPtr;
typedef vector< TraitPtr > Traits;

#endif  // TRAIT_H_
