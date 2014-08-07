#include "utility/lowest_bit.h"

lowest_bit_256::lowest_bit_256() {
    initialize();
}

void lowest_bit_256::initialize() {
    block_type val = 255;
    while( val ) {
        block_type idx = 0, tmp = val;
        while( tmp ) {
            if( tmp & 1 ) {
                m_vals[ val ].bit_index = idx++;
                tmp >>= 1;
                break;
            } 

            tmp >>= 1;
            ++idx;
        }

        if( tmp ) {
            do {
                if( tmp & 1 ) {
                    m_vals[ val ].next = tmp;
                    m_vals[ val ].bit_shift_next = idx;
                    m_vals[ val ].next_ptr = &m_vals[tmp];
                    break;
                }
                tmp >>= 1;
                ++idx;
            } while( tmp );
        } else {
            m_vals[ val ].next = 0;
            m_vals[ val ].bit_shift_next = idx;
            m_vals[ val ].next_ptr = NULL;
        }
        --val;
    }
}

const lowest_bit_256::value_type & lowest_bit_256::operator[]( block_type val ) const {
    return m_vals[ val ];
}

unsigned char lowest_bit_256::next( block_type val ) const {
    return m_vals[ val ].next;
}

unsigned char lowest_bit_256::low_bit_index( block_type val ) const {
    return m_vals[ val ].bit_index;
}

unsigned char lowest_bit_256::next_shift_offset( block_type val ) const {
    return m_vals[ val ].bit_shift_next;
}

const lowest_bit_256::value_type * lowest_bit_256::begin() const {
    return m_vals;
}

lowest_bit_256::~lowest_bit_256() {}
