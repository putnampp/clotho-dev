#ifndef ENVIRONMENT_SNAPSHOT_H_
#define ENVIRONMENT_SNAPSHOT_H_

#include <ostream>

struct EnvironmentSnapshot {
    size_t male_count, female_count;
    size_t pending_count;
    size_t available_count;

    EnvironmentSnapshot() :
        male_count(0), 
        female_count(0), 
        pending_count(0), 
        available_count(0)
    {}

    EnvironmentSnapshot( size_t nMales, size_t nFemales, size_t nPending, size_t nAvail ) :
        male_count(nMales), 
        female_count(nFemales), 
        pending_count(nPending), 
        available_count(nAvail)
    {}

    EnvironmentSnapshot( const EnvironmentSnapshot & es ) :
        male_count(es.male_count), 
        female_count(es.female_count), 
        pending_count(es.pending_count), 
        available_count(es.available_count)
    {}

    EnvironmentSnapshot & operator=( const EnvironmentSnapshot & es ) {
        male_count = es.male_count;
        female_count = es.female_count;
        pending_count = es.pending_count;
        available_count = es.available_count;
        return *this;
    }

    EnvironmentSnapshot & operator-=( const EnvironmentSnapshot & es ) {
        male_count -= es.male_count;
        female_count -= es.female_count;
        pending_count -= es.pending_count;
        available_count -= es.available_count;
        return *this;
    }

    EnvironmentSnapshot & operator+=( const EnvironmentSnapshot & es ) {
        male_count += es.male_count;
        female_count += es.female_count;
        pending_count += es.pending_count;
        available_count += es.available_count;
        return *this;
    }

    friend std::ostream & operator<<( std::ostream & o, const EnvironmentSnapshot & es );
};

inline std::ostream & operator<<( std::ostream & o, const EnvironmentSnapshot & es ) {
    o << "{Males=" << es.male_count << "; Females=" << es.female_count << "; Pending=" << es.pending_count << "; Available=" << es.available_count << "}";
    return o;
}

#endif  // ENVIRONMENT_SNAPSHOT_H_
