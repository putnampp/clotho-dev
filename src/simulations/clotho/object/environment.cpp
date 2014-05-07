#include "environment.h"

#include "individual.h"

#include "../event/birth_event.h"
#include "../event/maturity_event.h"
#include "../event/mate_event.h"
#include "../event/death_event.h"

#include <algorithm>

using std::swap;

//template <>
//void EventPerformer< Environment, ClothoEvent>::initialize() {
//    addHandler( BIRTH_EVENT_K, &Environment::handle_birth );
//    addHandler( DEATH_EVENT_K, &Environment::handle_death );
//}

//EventPerformer< Environment, ClothoEvent > Environment::m_evt_performer;

Environment::Environment( simulation_manager * manager ) :
//    SimulationObject< ClothoEventSet >( manager ),
    ClothoObject( manager ),
    m_genetic_map( new GeneticMap() ),
    m_selection_model( NULL ),
    m_reproduction_model( NULL ),
    m_nIndAlloc(0),
    m_nFounder(0)
{
    setSimulationManager( manager );
}

Environment::Environment( simulation_manager * manager, GeneticMap::Ptr gmap, selection_model * s, reproduction * r ) :
//    SimulationObject< ClothoEventSet >( manager ),
    ClothoObject( manager ),
    m_genetic_map( gmap ),
    m_selection_model( s ),
    m_reproduction_model( r ),
    m_nIndAlloc(0),
    m_nFounder(0)
{
    setSimulationManager( manager );
}

void Environment::setFounderSize( unsigned int s ) {
    m_nFounder = s;
}

void Environment::initialize() {

    for( unsigned int i = 0; i < m_nFounder; ++i ) {
        system_id id = getIndividual();
        object * t = m_sim_manager->getObject( id );

        t->initialize();
    }
}

void Environment::perform_event( const event * e ) {
    const ClothoEvent * evt = dynamic_cast< const ClothoEvent * >( e );

    if( evt ) {
//        m_evt_performer( this, evt );
        event_type_t e_id = evt->getEventType();
        if( e_id == BirthEvent::TYPE_ID ) {
            handle_birth( evt );
        } else if( e_id == MaturityEvent::TYPE_ID ) {
            handle_maturity( evt );
        } else if( e_id == DeathEvent::TYPE_ID ) {
            handle_death( evt );
        }
    }
}

void Environment::finalize() {
    m_males.clear();
    m_females.clear();

    cout << m_active_individuals.size() << " active individuals at the time of finalization" << endl;
    while( !m_active_individuals.empty() ) {
        object * tmp = m_sim_manager->getObject( m_active_individuals.begin()->first );
        m_active_individuals.erase( m_active_individuals.begin() );
        tmp->finalize();
        delete tmp;
    }

    cout << m_pending.size() << " individuals waiting to be born" << endl;
    while( !m_pending.empty() ) {
        object * tmp = m_sim_manager->getObject( *m_pending.begin() );
        m_pending.erase( m_pending.begin() );
        tmp->finalize();
        delete tmp;
    }

    cout << m_available_individuals.size() << " available individuals at the time of finalization" << endl;
    while( !m_available_individuals.empty() ) {
        object * tmp = m_sim_manager->getObject( m_available_individuals.front() );
        m_available_individuals.pop_front();
        tmp->finalize();
        delete tmp;
    }

    ClothoObject::finalize();

    cout << m_nIndAlloc << " allocated individuals by the environment" << endl;
}

system_id Environment::getIndividual() {
    system_id id(0);
    if( m_available_individuals.empty() ) {
        ++m_nIndAlloc;
        Individual * ind = new Individual( m_sim_manager, getSystemID(), m_reproduction_model );
        id = ind->getSystemID();
    } else {
        id = m_available_individuals.front();
        m_available_individuals.pop_front();
        assert( m_available_individuals.empty() || m_available_individuals.front() != id );
    }

    m_pending.insert( id );
    return id;
}

void Environment::handle_birth( const ClothoEvent * ce ) {
    // assume that the individual will always send a maturity event at time + 1
    const BirthEvent * be = static_cast< const BirthEvent * >( ce );

//    MaturityEvent * me = new MaturityEvent( getCurrentTime(), getCurrentTime() + 1, ce->getSender(), getSystemID(), getNextEventID(), be->getSex() );
    MaturityEvent * me = MaturityEvent::getOrCreate();
    me->init( getCurrentTime(), getCurrentTime() + 1, getSystemID(), getSystemID(), getNextEventID(), be->getSender(), be->getSex() );
    sendEvent( me );

    DeathEvent * de = DeathEvent::getOrCreate();
    de->init( getCurrentTime(), getCurrentTime() + 3, getSystemID(), ce->getSender(), getNextEventID() );
    sendEvent(de);

    m_pending.erase( be->getSender() );

    if( be->getSex() == FEMALE ) {
        m_active_individuals.insert( make_pair( be->getSender(), make_pair( &m_females, m_females.size())));
        m_females.push_back( be->getSender() );
    } else if( be->getSex() == MALE ) {
        m_active_individuals.insert( make_pair( be->getSender(), make_pair( &m_males, m_males.size())));
        m_males.push_back( be->getSender() );
        
    } else {
        assert(false);
    }
}

void Environment::handle_maturity( const ClothoEvent * ce ) {
    if( ! m_selection_model ) return;

    individual_group_t * igroup = m_active_individuals[ ce->getSender() ].first;
    system_id mate_id;
    if( igroup == &m_males ) {
        mate_id = m_selection_model->find_mate( ce->getSender(), m_females );
    } else {
        mate_id = m_selection_model->find_mate( ce->getSender(), m_males );
    }

    system_id offspring_id = getIndividual();

    MateEvent * me0 = MateEvent::getOrCreate();
    me0->init( getCurrentTime(), getCurrentTime() + 1, getSystemID(), ce->getSender(), getNextEventID(), offspring_id );
    MateEvent * me1 = MateEvent::getOrCreate();
    me1->init( getCurrentTime(), getCurrentTime() + 1, getSystemID(), mate_id, getNextEventID(), offspring_id );

    sendEvent( me0 );
    sendEvent( me1 );
}

void Environment::handle_death( const ClothoEvent * ce ) {
    lookup_iterator it = m_active_individuals.find( ce->getSender() );
    assert( it != m_active_individuals.end() );

    individual_group_t * igroup = it->second.first;
    size_t offset = it->second.second;
    if( offset != igroup->size() - 1 ) {
        // if the dead individual is not the last individual in the vector
        // swap the dead individual with the last individual in the vector
        // and update the previously last individual's offset to be the offset of
        // the dead individual
        swap( igroup->back(), igroup->at( offset ) );
        if( igroup->back() != ce->getSender() ) {
            cout << "Swap failed: " << ce->getSender() << " <> " << igroup->back() << "; " << igroup->size() << endl;
        }
        assert( igroup->back() == ce->getSender() );
        m_active_individuals[ igroup->at( offset ) ].second = offset;
    }

    it->second.first->pop_back();   // remove the dead individual
    m_active_individuals.erase(it); // individual is no longer active
    m_available_individuals.push_back( ce->getSender() );
}
/*
void Environment::addIndividual( const system_id & id ) {
//    cout << "Environment encountered birth event: " << id << " @ " << getCurrentTime() << endl;

    DeathEvent * de = new DeathEvent( getCurrentTime(), getCurrentTime() + 1, getSystemID(), id, getNextEventID() );
    sendEvent(de);
    m_active_individuals.insert( id );
}

void Environment::removeIndividual( const system_id & id ) {
//    cout << "Environment encountered death event: " << id << " @ " << getCurrentTime() << endl;

    m_active_individuals.erase( id );
    m_available_individuals.push_back( id );
}
*/

