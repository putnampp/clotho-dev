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
#ifndef CLOTHOGENETICMAP_H_
#define CLOTHOGENETICMAP_H_

#include "GeneticMap.h"

#include <vector>

using std::vector;
using std::pair;

typedef pair< size_t, allelic_dominance * > locus_dominance_t;
#define VECTOR_TRAIT_LOCUS
#ifdef VECTOR_TRAIT_LOCUS
typedef vector< vector< locus_dominance_t *> * > trait_locus_map_t;
typedef vector< locus_dominance_t * > * trait_locus_group_t;
#else

// added for consideration later.
// this approach may be less efficient as multimap::equal_range
// is a logarithmic complexity, whereas by index lookup in vector is constant
#include <map>
using std::multimap;
typedef multimap< size_t, locus_dominance_t > trait_locus_map_t;
typedef pair< trait_locus_map_t::iterator, trait_locus_map_t::iterator > trait_locus_group_t;
#endif

class ClothoGeneticMap : public GeneticMap {
public:
    ClothoGeneticMap();
    void add( Locus::Ptr l );
    void add( const shared_ptr< Trait > p );

    size_t getLociCount() const;
    size_t getTraitCount() const;

    trait_locus_group_t getLociForTrait( size_t trait_idx );
    trait_locus_group_t getLociForTrait( const string & trait_name );

    virtual ~ClothoGeneticMap();
protected:
    Loci *    m_loci;
    Traits *    m_traits;
    trait_locus_map_t * m_trait_loci_mapping;
};

#endif  // CLOTHOGENETICMAP_H_
