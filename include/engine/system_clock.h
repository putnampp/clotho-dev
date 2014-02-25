#ifndef SYSTEM_CLOCK_H_
#define SYSTEM_CLOCK_H_

template < class CLOCK >
struct system_clock {
    typedef CLOCK vtime_t;

    static vtime_t getPositiveInfinity() {
        return vtime_t(-1);
    }

    static vtime_t getZero() {
        return vtime_t(0);
    }

    template< class T >
    static vtime_t toVTime( const T & t );

};

#endif  // SYSTEM_CLOCK_H_
