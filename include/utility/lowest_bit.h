#ifndef LOWEST_BIT_H_
#define LOWEST_BIT_H_

#include <cstdlib>

class lowest_bit_256 {
public:
    struct _value {
        unsigned char next, bit_index, bit_shift_next;
        _value * next_ptr;
        _value() : next(0), bit_index(0), bit_shift_next(0), next_ptr(NULL) {}
        _value( const _value & r ) : next( r.next ), bit_index(r.bit_index), bit_shift_next( r.bit_shift_next), next_ptr( r.next_ptr) {}
    };

    typedef _value value_type;
    typedef unsigned char block_type;

    static const unsigned int block_width = 8;

    lowest_bit_256();

    const value_type & operator[]( block_type val ) const;

    unsigned char next( block_type val ) const;
    unsigned char low_bit_index( block_type val ) const;
    unsigned char next_shift_offset( block_type val ) const;

    const value_type * begin() const;

    virtual ~lowest_bit_256();
protected:
    void initialize();

    value_type m_vals[256];
};

#endif  // LOWEST_BIT_H_
