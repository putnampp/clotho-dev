#ifndef LOWEST_BIT_H_
#define LOWEST_BIT_H_

class lowest_bit_256 {
public:
    struct _value {
        unsigned char next, bit_index, bit_shift_next;
        _value() : next(0), bit_index(0), bit_shift_next(0) {}
        _value( const _value & r ) : next( r.next ), bit_index(r.bit_index), bit_shift_next( r.bit_shift_next) {}
    };

    typedef _value value_type;

    lowest_bit_256();

    const value_type & operator[]( unsigned char val ) const;

    unsigned char next( unsigned char val ) const;
    unsigned char low_bit_index( unsigned char val ) const;
    unsigned char next_shift_offset( unsigned char val ) const;

    virtual ~lowest_bit_256();
protected:
    void initialize();

    value_type m_vals[256];
};

#endif  // LOWEST_BIT_H_
