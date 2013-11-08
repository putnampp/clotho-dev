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

#ifndef GENETICEVENTMANAGER_H_
#define GENETICEVENTMANAGER_H_

#include "common.h"
#include <vector>

using std::vector;

class GeneticEvent;

typedef unsigned long long index_t;

union long_id {
    unsigned long long id;
    struct {
        index_t format_idx;
    };
    struct {
        unsigned int lo, hi;
    };
    struct {
        unsigned int format_pos, data;
    };
    struct {
        unsigned short lo_lo, lo_hi, hi_lo, hi_hi;
    };
    unsigned char bytes[ sizeof( index_t ) ];
};

typedef long_id ge_id_t;

#define FORMAT_BITS     4
#define FORMAT_MASK        0x000000000000000F
#define INDEX_OVERFLOW     0xF000000000000000
#define POS_OVERFLOW       0xF0000000

#define removeFormat( id ) (id >> FORMAT_BITS)
#define addFormat(id, format) ((id << FORMAT_BITS) | format)

class EventAlignment;

class GeneticEventManager {
public:
    typedef vector< const GeneticEvent * > GeneticEvents;

    GeneticEventManager();
    GeneticEventManager( const GeneticEventManager & gem);

    bool decode_allele(ge_id_t e, pos_t & pos, allele_t & all );

    bool    decode_event( ge_id_t e, GeneticEvent * ge );
    ge_id_t encode_event( const GeneticEvent * e );

    EventAlignment alignGeneticEvents( GeneticEvent * ge, ge_id_t id );

    virtual ~GeneticEventManager();
protected:
    inline bool handleIndex( ge_id_t eID, pos_t & pos, allele_t & all );

    bool handleShortSubstitution( ge_id_t eID, int len, pos_t & pos, allele_t & all );
    bool handleDeletion( ge_id_t eID, pos_t & pos, allele_t & all );
    bool handleShortInsert( ge_id_t eID, int len, pos_t & pos, allele_t & all );

    shared_ptr< GeneticEvents >    m_events;
};

#endif  // GENETICEVENTMANAGER_H_
