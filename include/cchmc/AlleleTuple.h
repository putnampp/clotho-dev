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
#ifndef ALLELETUPLE_H_
#define ALLELETUPLE_H_

#include "config.hpp"
#include "ploidy.h"

#include <vector>

using std::vector;

#include <ostream>
#include <array>
using std::array;
using std::ostream;

#include <cstring>
using std::memcpy;

#define ALLELE_COPIES 2

//typedef array< allele_t, ALLELE_COPIES > allele_tuple;
//typedef vector< allele_tuple > AlleleGroup;
//typedef allele_t * AlleleGroup;

//typedef AlleleGroup * AlleleGroupPtr;
//typedef allele_t ** AlleleGroupPtr;
//

typedef allele_t * allele_tuple;
typedef allele_t ** AlleleGroup;
typedef AlleleGroup AlleleGroupPtr;

ostream & operator<<( ostream & out, const array< allele_t, 2 >  & at );

AlleleGroupPtr allocateAlleleGroup( size_t size );

void resizeAlleleGroup( AlleleGroupPtr & agp, size_t size );
void copyAlleleGroup( AlleleGroupPtr dest, AlleleGroupPtr src);

AlleleGroupPtr cloneAlleleGroup( AlleleGroupPtr agp );

void deallocateAlleleGroup( AlleleGroupPtr agp );

#endif  // ALLELETUPLE_H_
