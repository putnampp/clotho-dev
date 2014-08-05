#include "utility/lowest_bit.h"

lowest_bit_256::lowest_bit_256() {
    initialize();
}

void lowest_bit_256::initialize() {
    unsigned char val = 255;
    while( val ) {
        unsigned char idx = 0, tmp = val;
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
                    break;
                }
                tmp >>= 1;
                ++idx;
            } while( tmp );
        } else {
            m_vals[ val ].next = 0;
            m_vals[ val ].bit_shift_next = idx;
        }
        --val;
    }
}

const lowest_bit_256::value_type & lowest_bit_256::operator[]( unsigned char val ) const {
    return m_vals[ val ];
}

unsigned char lowest_bit_256::next( unsigned char val ) const {
    return m_vals[ val ].next;
}

unsigned char lowest_bit_256::low_bit_index( unsigned char val ) const {
    return m_vals[ val ].bit_index;
}

unsigned char lowest_bit_256::next_shift_offset( unsigned char val ) const {
    return m_vals[ val ].bit_shift_next;
}

lowest_bit_256::~lowest_bit_256() {}
