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

#ifndef DIMENSION_H_
#define DIMENSION_H_

#include "common.h"
#include <cassert>


/*******************************************************************************
 * Dimensions is a generic fixed-length vector of coordinates of a specified
 * type.
 *
 * D - specifies the desired type
 * N - the number of dimensions (overflow occurs at 255)
 ******************************************************************************/
template < class D, unsigned char N >
class Dimensions {
public:
    enum { size = N };
    typedef D   type;

    Dimensions() {}

    virtual type &   operator[]( uint32_t idx ) {
        assert( idx < size );
        return m_dimensions[ size ];
    }

    virtual ~Dimensions() {}
protected:
    type    m_dimensions[ size ];
    struct  m_coordinates;
};

/*******************************************************************************
 * Dimensions< D > is 3D partial specialization of the Dimensions object.
 *
 * Partial specialization provides an implementation of the coordinates
 * structure. Thus, inheriting objects can access the dimensions using
 * either m_coordinates.(x, y, z) or m_dimensions[0,1,2].
 ******************************************************************************/
template < class D >
class Dimensions < D, 3 > {
public:
    enum { size = 3 };
    typedef D   type;

    Dimensions() {}

    virtual type & operator[]( uint32_t idx ) {
        assert( idx < size );
        return m_coordinates[ idx ];
    }

    virtual ~Dimensions() {}
protected:
    union {
        type    m_dimensions[ size ];
        struct  {
            type    x, y, z;
        } m_coordinates;
    };
};

#endif  // DIMENSION_H_
