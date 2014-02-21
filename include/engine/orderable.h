#ifndef ORDERABLE_H_
#define ORDERABLE_H_

template < class CLOCK >
class orderable {
public:
    typedef CLOCK   logical_clock_t;
    CLOCK getTime() const { return m_time; }
    virtual bool operator<( const orderable< CLOCK > & rhs ) const {
        return m_time < rhs.m_time;
    }
    virtual ~orderable() {}
protected:
    orderable( const CLOCK time ) : m_time(time) {}
    logical_clock_t   m_time;
};

#endif  // ORDERABLE_H_
