#ifndef SIMULATION_STATS_H_
#define SIMULATION_STATS_H_

#include <iostream>
#include <list>
#include <unordered_map>
#include "time/timing.h"

using std::string;
using std::list;
using std::pair;
using std::make_pair;
using std::unordered_map;
using std::ostream;

using namespace timing;

class SimulationStats {
public:
    typedef string phase_id_t;
    typedef pair< TIME, TIME >  phase_period_t;
    typedef pair< phase_id_t, phase_period_t > phase_t;
    typedef list< phase_t >     phase_list_t;
    typedef unordered_map< phase_id_t, phase_list_t::iterator > phase_lookup_t;

    SimulationStats();

    void startPhase( const phase_id_t & id );
    void stopPhase( const phase_id_t & id );

    void printPhase( ostream &, const phase_id_t & id ) const;

    TIME getPhaseStart( const phase_id_t & id ) const;
    TIME getPhaseStop( const phase_id_t & id ) const;

    TIME getPhaseLapse( const phase_id_t & id ) const;

    void setProcessedEvents( size_t n );
    void setPendingEvents( size_t n );

    friend ostream & operator<<( ostream &, const SimulationStats & );

    virtual ~SimulationStats();
protected:
    phase_list_t m_phase_times;
    phase_lookup_t m_phase_lookup;

    size_t m_nProcessedEvents, m_nPendingEvents;
};

ostream & operator<<( ostream &, const SimulationStats & );

#endif  // SIMULATION_STATS_H_
