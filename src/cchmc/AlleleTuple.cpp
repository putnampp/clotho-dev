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

#include "AlleleTuple.h"

#include <iostream>
#include <cstring>

using std::memcpy;
using std::cout;
using std::endl;

/*
ostream & operator<<( ostream & out, const array< allele_t, 2 >  & at ) {
    out << (unsigned short)at[0] << "|" << (unsigned short)at[1];
    return out;
}

AlleleGroupPtr allocateAlleleGroup( size_t size ) {
    AlleleGroupPtr agp = new AlleleGroup(size);
    return agp;
}

void resizeAlleleGroup( AlleleGroupPtr agp, size_t size ) {
    if( agp->size() < size ) {
        for( size_t s = agp->size(); s < size; ++s ) {
            agp->push_back( allele_tuple() );
        }
    }
}

AlleleGroupPtr cloneAlleleGroup( AlleleGroupPtr agp ) {
    AlleleGroupPtr clone = new AlleleGroup( *agp );
    return clone;
}*/

inline size_t computePaddedCapacity( size_t s ) {
    return s + ((s % ALLELES_PER_PWORD ) ? (ALLELES_PER_PWORD - (s % ALLELES_PER_PWORD) ) : 0);
}

AlleleGroupPtr allocateAlleleGroup( size_t s ) {
    AlleleGroupPtr tmp = new allele_t *[ ALLELE_COPIES ];

    if( s > 0 ) {
        size_t padded = computePaddedCapacity( s );
        tmp[ 0 ] = new allele_t[ ALLELE_COPIES * padded ];
        for( size_t i = 1; i < ALLELE_COPIES; ++i ) {
            tmp[i] = tmp[0] + i * padded;
        }
    } else {
        for( size_t i = 0; i < ALLELE_COPIES; ++i ) {
            tmp[ i ] = NULL;
        }
    }
    return tmp;
}

void resizeAlleleGroup( AlleleGroupPtr & agp, size_t size ) {
    if( size == 0 ) return;

    size_t oldSize = (agp[1] - agp[0]);

    if( oldSize < size ) {
        size_t padded = computePaddedCapacity( size );
        allele_t * tmp = new allele_t[ padded * ALLELE_COPIES ];

        if( oldSize > 0 ) {
            for( size_t i = 0; i < ALLELE_COPIES; ++i ) {
                memcpy( tmp + i * padded, agp[i], oldSize * sizeof(allele_t));
            }

            delete [] agp[0];
        }
        
        agp[0] = tmp;
        for( size_t i = 1; i < ALLELE_COPIES; ++i ) {
            agp[i] = agp[0] + i * padded;
        }
    }
}

AlleleGroupPtr cloneAlleleGroup( AlleleGroupPtr agp ) {
    size_t s = (agp[1] - agp[0]);

    AlleleGroupPtr clone = new allele_t *[ ALLELE_COPIES ];

    clone[0] = new allele_t[ s * ALLELE_COPIES ];
    for( size_t i = 1; i < ALLELE_COPIES; ++i ) {
        clone[i] = clone[0] + i * s;
    }

    memcpy( clone[0], agp[0], s * ALLELE_COPIES * sizeof(allele_t));

    return clone;
}

void copyAlleleGroup( AlleleGroupPtr dest, AlleleGroupPtr src ) {
    if( dest[0] == src[0] ) return;

    memcpy( dest[0], src[0], (src[1]-src[0]) * ALLELE_COPIES * sizeof(allele_t) );
}

void deallocateAlleleGroup( AlleleGroupPtr agp ) {
    delete [] agp[0];
    delete agp;
}
