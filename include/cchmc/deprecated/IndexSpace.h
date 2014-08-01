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

#ifndef INDEX_SPACE_H_
#define INDEX_SPACE_H_

#include "common.h"
#include <cassert>
#include <cstring>

struct enumerable {
    virtual uint32_t    index() = 0;
    virtual uint32_t    limit() const = 0;
};

/*******************************************************************************
 * Index space is an coordinate system where points are enumerable
 *
 * Coordinates are used to identify points.
 *
 ******************************************************************************/
struct index_space : public enumerable {
    virtual sizet_t rank() const = 0;

    virtual void setDimensions( const uint32_t * limits, size_t rank ) = 0;

    virtual void setCoordinates( const uint32_t * coords, size_t rank ) = 0;

    virtual void getCoordinate( uint32_t rank ) = 0;
    virtual void getCoordinates( uint32_t * coords ) = 0;

    virtual void resetCoordinates( ) = 0;
};


/*******************************************************************************
 * IndexSpace is a basic implementation of an index_space.
 *
 * It assumes that dimensions are expressed in row-major order. In other words,
 * the last dimension is considered to be continuous.
 *
 ******************************************************************************/
template < unsigned char R >
class IndexSpace : public index_space {
public:
    IndexSpace( ) : m_offset(0), m_maxLimit(0), m_bUpToDate(false) {

    }

    /*****
     *  Default constructor for creating a square IndexSpace
     *
     *  Each dimension has the same limit value
     ****/
    IndexSpace( uint32_t limit ) : m_offset(0), m_maxIndex(0), m_bUpToDate(false) {
        for( int i = 0; i < RANK; ++i ) {
            this->m_dims[i] = limit;
        }
    }

    IndexSpace( uint32_t * limits, value_t * coords, size_t rank = RANK ) : m_offset(0), m_maxIndex(0), m_bUpToDate(false) {
        memcpy( this->m_dims, dims, RANK * sizeof( uint32_t ) );
        memcpy( this->m_coords, coords, RANK * sizeof(uint32_t) );

        calculate_index();
    }

    IndexSpace( const CoordinateSpace< RANK > & c ) m_bUpToDate(false) {
        memcpy( this->m_dims, c.m_dims, RANK * sizeof( uint32_t ));
        memcpy( this->m_coords, c.m_coords, RANK * sizeof( value_t ));
        this->m_offset = c.m_offset;
    }

    virtual void setDimensions( const uint32_t * limits, size_t rank = RANK ) {
        assert( rank <= RANK );
        memcpy( m_dims, limits, rank * sizeof( uint32_t ) );

        compute_dimension_offsets();
    }

    virtual void setCoordinates( uint32_t rank, uint32_t coord ) {
        assert( rank < RANK );
        assert( coord < m_dims[ rank ] );

        m_coords[ rank ] = coord;

        m_bUpToDate = false;
    }

    virtual void setCoordinates( const uint32_t * coords, size_t rank = RANK ) {
        assert( 0 < rank && rank <= RANK );

        // Check that each set coordinate is valid with the current space
        // compute_dimension_offsets the coordinate
        for( int i = 0; i < rank; ++i ) {
            assert( coords[i] < m_dims[ i ] );
            m_coords[i] = coords[i];
        }

        // all coordinates should be valid. compute_dimension_offsets the index
        calculate_index();
    }

    virtual uint32_t getCoordinate( uint32_t rank ) {
        assert( rank < RANK );
        return m_coords[rank];
    }

    virtual void getCoordinates( uint32_t * coords ) {
        assert( coords != NULL );

        memcpy( coords, m_coords, RANK * sizeof(uint32_t) );
    }

    virtual void resetCoordinates() {
        memset( m_coords, 0, RANK * sizeof( value_t ) );
        m_bUpToDate = false;
    }

    virtual size_t      rank() const {
        return RANK;
    }

    virtual uint32_t    limit() const {
        return m_maxIndex;
    }

    virtual uint32_t    index() {
        if( !m_bUpToDate ) calculate_index();
        return m_offset;
    }

    virtual ~Offset() { }
protected:

    virtual void compute_dimension_offsets() {
        // last dimension is continuous
        m_dimOffsets[ RANK - 1 ] = 1;
        for( int i = RANK; i >= 1; --i ) {
            m_dimOffsets[ i - 1 ] = m_dims[ i ] * m_dimOffsets[ i ];
        }

        m_maxIndex = m_dimOffsets[0] * m_dims[0];

        // Assume that dimensional changes have invalidated the coordinates
        // therefore, just reset set them.
        resetCoordinates();
    }

    void calculate_index() {
        for( int i = 0; i < RANK; ++i ) {
            assert( m_coords[i] < m_dims[ i ] );
            m_offset = m_coords[ i ] * m_dimOffsets[ i ];
        }
        m_bUpToDate = true;
    }

    static const unsigned char RANK = R;

    uint32_t    m_coords[ RANK ];
    uint32_t    m_dims[ RANK ];
    uint32_t    m_dimOffsets[ RANK ];
    uint32_t    m_offset;
    uint32_t    m_maxIndex;

    bool        m_bUpToDate;
};

#endif // INDEX_SPACE_H_
