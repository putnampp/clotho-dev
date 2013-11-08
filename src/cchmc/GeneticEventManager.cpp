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

#include "GeneticEventManager.h"
#include "GeneticEvent.h"
#include <cassert>

/*
 * 0000 - point substitution
 * 0001 - 2 point substitution
 * 0010 - 3 point substitution
 * 0011 - 4 point substitution
 * 0100 - insert allele   (assumes byte per allele)
 * 0101 - 2 allele insert (assumes byte per allele)
 * 0110 - 3 allele insert (assumes byte per allele)
 * 0111 - 4 allele insert (assumes byte per allele)
 * 1000 - deletion [start, end)
 * [1001, 1110] - index
 * 1111 - index
 */

enum EncodingFormat { efSUBSTITUTION1 = 0, efSUBSTITUTION2 = 1, efSUBSTITUTION3 = 2, efSUBSTITUTION4 = 3,
                      efINSERT1 = 4, efINSERT2 = 5, efINSERT3 = 6, efINSERT4 = 7,
                      efDELETION = 8, efINDEX = 15
                    };

inline ge_id_t encodeImmediate( const GeneticEvent * ge, EncodingFormat ef ) {
    assert( (ge->getStart() & POS_OVERFLOW) == 0);
    assert( 0 < ge->length() && ge->length() <= 4 );
    assert( efSUBSTITUTION1 <= ef && ef <= efINSERT4 );

    ge_id_t id;
    id.id = addFormat( ge->getStart(), ef );

    allele_t all = 0;
    int offset = 4;
    for( pos_t p = ge->getStart(); p < ge->getEnd(); ++p) {
        ge->allele( p, all );
        id.bytes[offset++] = all;
    }
    return id;
}

inline ge_id_t encodeDeletion(pos_t start, pos_t end ) {
    assert( (start & POS_OVERFLOW) == 0 );

    ge_id_t id;
    id.format_pos = start;
    id.data = end;
    id.format_pos = addFormat( id.format_pos, efDELETION);
}

inline ge_id_t encodeIndex( index_t idx ) {
    assert( (idx & INDEX_OVERFLOW) == 0 );

    ge_id_t id;
    id.id = addFormat( idx, efINDEX );

    return id;
}

inline void decodePosition( ge_id_t eID, pos_t & pos ) {
    pos = removeFormat( eID.lo );
}

inline EncodingFormat getFormat( ge_id_t id ) {
    return (EncodingFormat)( id.format_pos & FORMAT_MASK );
}

inline index_t getIndexFromID( ge_id_t eID ) {
    return removeFormat(eID.format_idx);
}

inline void decodeDeletion( ge_id_t eID, pos_t & start, pos_t & end ) {
    start = removeFormat(eID.format_pos);
    end = eID.data;
}

GeneticEventManager::GeneticEventManager() : m_events( new GeneticEvents() ) {}

GeneticEventManager::GeneticEventManager( const GeneticEventManager & gem ) : m_events( gem.m_events ) {}

GeneticEventManager::~GeneticEventManager() {
    if( m_events && !m_events->empty() ) {
        for( GeneticEvents::iterator it = m_events->begin(); it != m_events->end(); it++ ) {
            delete (*it);
        }
        m_events->clear();
    }
}

bool GeneticEventManager::decode_allele( ge_id_t eID, pos_t & pos, allele_t & all ) {
    bool bFound = false;
    int len = 0;
    switch( getFormat( eID ) ) {
    case efSUBSTITUTION4:
        ++len;
    case efSUBSTITUTION3:
        ++len;
    case efSUBSTITUTION2:
        ++len;
    case efSUBSTITUTION1:
        bFound = handleShortSubstitution( eID, ++len, pos, all );
        break;
    case efINSERT4:
        ++len;
    case efINSERT3:
        ++len;
    case efINSERT2:
        ++len;
    case efINSERT1:
        bFound = handleShortInsert( eID, ++len, pos, all );
        break;
    case efDELETION:
        bFound = handleDeletion( eID, pos, all );
        break;
    case efINDEX:
        bFound = handleIndex( eID, pos, all );
        break;
    default:
        assert(false);
        break;
    }

    return bFound;
}

ge_id_t GeneticEventManager::encode_event( const GeneticEvent * ge ) {
    ge_id_t id;

    if( ge->getType() == SUBSTITUTION ) {
        switch( ge->length() ) {
        case 0:
            assert( false );
            break;
        case 1:
            id = encodeImmediate( ge, efSUBSTITUTION1  );
            delete ge;
            break;
        case 2:
            id = encodeImmediate( ge, efSUBSTITUTION2  );
            delete ge;
            break;
        case 3:
            id = encodeImmediate( ge, efSUBSTITUTION3  );
            delete ge;
            break;
        case 4:
            id = encodeImmediate( ge, efSUBSTITUTION4  );
            delete ge;
            break;
        default:
            index_t idx = m_events->size();
            m_events->push_back( ge );

            id = encodeIndex( idx );
            break;
        }
    } else if( ge->getType() == DELETION ) {
        id = encodeDeletion( ge->getStart(), ge->getEnd() );
        delete ge;
    } else if( ge->getType() == INSERTION ) {
        switch( ge->length() ) {
        case 0:
            assert( false );
            break;
        case 1:
            id = encodeImmediate( ge, efINSERT1 );
            delete ge;
            break;
        case 2:
            id = encodeImmediate( ge, efINSERT2 );
            delete ge;
            break;
        case 3:
            id = encodeImmediate( ge, efINSERT3 );
            delete ge;
            break;
        case 4:
            id = encodeImmediate( ge, efINSERT4 );
            delete ge;
            break;
        default:
            index_t idx = m_events->size();
            m_events->push_back( ge );

            id = encodeIndex( idx );
            break;
        }
    } else {
        index_t idx = m_events->size();
        m_events->push_back( ge );

        id = encodeIndex( idx );
    }
    return id;
}

bool GeneticEventManager::handleIndex( ge_id_t eID, pos_t & pos, allele_t & all ) {
    index_t idx = removeFormat( eID.id );
    return m_events->at( idx )->allele( pos, all );
}

bool GeneticEventManager::handleShortSubstitution( ge_id_t eID, int len, pos_t & pos, allele_t & all ) {
    pos_t ePos;
    allele_t eAll;

    decodePosition( eID, ePos );

    if( ePos <= pos && pos < ePos + len ) {
        all = eID.bytes[ 4 + (pos - ePos) ];
        return true;
    }

    return false;
}

bool GeneticEventManager::handleDeletion( ge_id_t eID, pos_t & pos, allele_t & all ) {
    pos_t start, end;

    decodeDeletion( eID, start, end );

    if( pos >= start ) {
        pos += (end - start);
    }

    return false;
}

bool GeneticEventManager::handleShortInsert( ge_id_t eID, int len, pos_t & pos, allele_t & all ) {
    pos_t ePos;

    decodePosition( eID, ePos );

    if( ePos <= pos && pos < ePos + len ) {
        all = eID.bytes[4 + (pos - ePos)];
        return true;
    }

    return false;
}
