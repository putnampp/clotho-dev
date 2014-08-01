#include "simulation_stats.h"
#include <cassert>

SimulationStats::SimulationStats() :
    m_nProcessedEvents(0),
    m_nPendingEvents(0) {
}

SimulationStats::~SimulationStats() {
    m_phase_lookup.clear();
    m_phase_times.clear();
}

void SimulationStats::startPhase( const phase_id_t & id ) {
    phase_lookup_t::iterator it = m_phase_lookup.find( id );
    phase_list_t::iterator pos = m_phase_times.end();
    if( it == m_phase_lookup.end() ) {
        // new phase
        pos = m_phase_times.insert( m_phase_times.end(), make_pair( id, make_pair( TIME(), TIME() ) ) );
        m_phase_lookup[ id ] = pos;
    } else {
        pos = it->second;
    }

    GetTime( pos->second.first );
}

void SimulationStats::stopPhase( const phase_id_t & id ) {
    TIME t;
    GetTime(t);

    phase_lookup_t::iterator it = m_phase_lookup.find( id );
    assert( it != m_phase_lookup.end() );

    it->second->second.second = t;
}

TIME SimulationStats::getPhaseStart( const phase_id_t & id ) const {
    phase_lookup_t::const_iterator it = m_phase_lookup.find( id );
    if( it == m_phase_lookup.end() ) {
        return TIME();
    }

    return it->second->second.first;
}

TIME SimulationStats::getPhaseStop( const phase_id_t & id ) const {
    phase_lookup_t::const_iterator it = m_phase_lookup.find( id );
    if( it == m_phase_lookup.end() ) {
        return TIME();
    }

    return it->second->second.second;
}

TIME SimulationStats::getPhaseLapse( const phase_id_t & id ) const {
    phase_lookup_t::const_iterator it = m_phase_lookup.find( id );
    if( it == m_phase_lookup.end() ) {
        return TIME();
    }

    TIME res;
    diff_TIME(res, it->second->second.second, it->second->second.first );

    return res;
}

void SimulationStats::printPhase( ostream & out, const phase_id_t & id ) const {
    phase_lookup_t::const_iterator it = m_phase_lookup.find( id );
    if( it == m_phase_lookup.end() ) {
        out << id << "\tNT\tNT\tNT";
    } else {
        TIME res;
        diff_TIME(res, it->second->second.second, it->second->second.first );

        out << id
            << "\t" << it->second->second.first
            << "\t" << it->second->second.second
            << "\t" << res;
    }
}

void SimulationStats::setProcessedEvents( size_t n ) {
    m_nProcessedEvents = n;
}

void SimulationStats::setPendingEvents( size_t n ) {
    m_nPendingEvents = n;
}

ostream & operator<<( ostream & out, const SimulationStats & ss ) {
    out << "Phase,Start,Stop,Lapse\n";
    for( SimulationStats::phase_list_t::const_iterator it = ss.m_phase_times.begin(); it != ss.m_phase_times.end(); it++ ) {
        TIME res;
        diff_TIME(res, it->second.second, it->second.first );

        out << it->first
            << "," << it->second.first
            << "," << it->second.second
            << "," << res
            << "\n";
    }

    out << ss.m_nProcessedEvents << " processed events\n";
    out << ss.m_nPendingEvents << " pending events\n";

    return out;
}
