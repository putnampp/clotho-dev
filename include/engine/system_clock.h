#ifndef SYSTEM_CLOCK_H_
#define SYSTEM_CLOCK_H_

template < class CLOCK >
struct system_clock {
    typedef CLOCK vtime_t;

    static const vtime_t POSITIVE_INFINITY;
    static const vtime_t ZERO;

    template< class T >
    static vtime_t toVTime( const T & t );

};

#endif  // SYSTEM_CLOCK_H_
