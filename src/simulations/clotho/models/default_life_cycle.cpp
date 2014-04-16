#include "default_life_cycle.h"

#include "../event/birth_event.h"
#include "../event/death_event.h"
#include "../event/inherit_event.h"
#include "../event/maturity_event.h"
#include "../event/mate_event.h"
#include "../event/signal_mate_event.h"
#include "../event/selection_event.h"
#include "../event/environment_snapshot_event.h"

#include "../genomes/human_gamete.h"

#include <algorithm>

using std::swap;

//
// TIndividual
//

template <>
void TIndividual<default_life_cycle>::initialize() {
    InheritEvent * ie = new InheritEvent( getCurrentTime(), getCurrentTime(), this, this, getNextEventID(), FEMALE, new HumanGamete( HumanGamete::FROM_MOTHER, HumanGamete::X_TYPE, 10000 ) );
    sendEvent( ie );
    ie = new InheritEvent( getCurrentTime(), getCurrentTime(), this, this, getNextEventID(), MALE, new HumanGamete( HumanGamete::FROM_FATHER, ((getObjectID() % 2 == 0) ? HumanGamete::X_TYPE : HumanGamete::Y_TYPE), 10000) );
    sendEvent( ie );
}

template < >
void TIndividual<default_life_cycle>::perform_event( const event * e ) {
    const ClothoEvent * evt = dynamic_cast< const ClothoEvent * >( e );

    if( !evt ) return;

    if( evt->getEventType() == BIRTH_EVENT_K ) {
        handle_birth( evt );
    } else if( evt->getEventType() == DEATH_EVENT_K ) {
        handle_death( evt );
    } else if( evt->getEventType() == INHERIT_EVENT_K ) {
        handle_inherit( evt );
    } else if( evt->getEventType() == MATE_EVENT_K ) {
        handle_mate( evt );
    } else if( evt->getEventType() == MATURITY_EVENT_K ) {
        handle_maturity( evt );
    }

}

template <>
void TIndividual<default_life_cycle>::finalize() {
    ClothoObject::finalize();
}

template<>
void TIndividual<default_life_cycle>::handle_birth( const ClothoEvent * evt ) {
    // notify environment of birth
    //
    m_prop->setDOB( evt->getReceived() );

    BirthEvent * be = new BirthEvent( getCurrentTime(), evt->getReceived(), this->getSystemID(), m_env_id, getNextEventID(), m_prop->getSex() );
    sendEvent( be );
}

template <>
void TIndividual<default_life_cycle>::handle_death( const ClothoEvent * evt ) {
    // notify environment of death
    m_prop->setEOL( evt->getReceived() );

    DeathEvent * de = new DeathEvent( getCurrentTime(), evt->getReceived(), this->getSystemID(), m_env_id, getNextEventID());
    sendEvent( de );
}

template <>
void TIndividual<default_life_cycle>::handle_inherit( const ClothoEvent * evt ) {
    const InheritEvent * ie = static_cast< const InheritEvent * >( evt );

    if(m_prop->getEOL() != SystemClock::POSITIVE_INFINITY ) {
        m_prop->reset();
    }

    m_prop->inheritFrom( ie->getSender(), ie->getParentSex(), ie->getGamete() );

    if( m_prop->getFather() != UNSET_ID && m_prop->getMother() != UNSET_ID ) {
        event::vtime_t bday = evt->getReceived();
        bday = ((bday / LIFE_CYCLE_PADDING) + 1) * LIFE_CYCLE_PADDING;

        m_prop->setDOB( bday );
        BirthEvent * be = new BirthEvent( getCurrentTime(), bday, this->getSystemID(), m_env_id, getNextEventID(), m_prop->getSex());
        sendEvent( be );
    }
}

template <>
void TIndividual<default_life_cycle>::handle_mate( const ClothoEvent * evt ) {
    const MateEvent * me  = static_cast< const MateEvent * >( evt );

    assert( m_repro );

    gamete * z = m_repro->reproduce( m_prop->getGenome() );

    InheritEvent * ie = new InheritEvent( getCurrentTime(), getCurrentTime(), this->getSystemID(), me->getOffspringID(), getNextEventID(), m_prop->getSex(), z );
    sendEvent( ie );
}

template <>
void TIndividual<default_life_cycle>::handle_maturity( const ClothoEvent * evt ) {
    MaturityEvent * me = new MaturityEvent( getCurrentTime(), getCurrentTime(), this->getSystemID(), m_env_id, getNextEventID(), this->getSystemID(), m_prop->getSex());
    sendEvent( me );
}

//
// TENVIRONMENT
//

template <>
system_id TEnvironment<default_life_cycle>::getIndividual() {
    system_id id(0);
    if( m_available_individuals.empty() ) {
        ++m_nIndAlloc;
        TIndividual<default_life_cycle> * ind = new TIndividual<default_life_cycle>( m_sim_manager, getSystemID(), m_reproduction_model );
        id = ind->getSystemID();
    } else {
        id = m_available_individuals.front();
        m_available_individuals.pop_front();
        assert( m_available_individuals.empty() || m_available_individuals.front() != id );
    }

    m_pending.insert( id );
    return id;
}

template < >
void TEnvironment<default_life_cycle>::initialize() {

    for( unsigned int i = 0; i < m_nFounder; ++i ) {
        system_id id = getIndividual();
        object * t = m_sim_manager->getObject( id );

        t->initialize();
    }
}

template < >
void TEnvironment<default_life_cycle>::finalize() {
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

template < >
void TEnvironment<default_life_cycle>::handle_birth( const ClothoEvent * ce ) {
    // assume that the individual will always send a maturity event at time + 1
    const BirthEvent * be = static_cast< const BirthEvent * >( ce );

    MaturityEvent * me = new MaturityEvent( getCurrentTime(), getCurrentTime() + 1, getSystemID(), getSystemID(), getNextEventID(), be->getSender(), be->getSex() );
    sendEvent( me );

    DeathEvent * de = new DeathEvent( getCurrentTime(), getCurrentTime() + 3, getSystemID(), ce->getSender(), getNextEventID() );
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

template <>
void TEnvironment< default_life_cycle >::handle_maturity( const ClothoEvent * ce ) {
    if( ! m_selection_model ) return;

    const MaturityEvent *me = static_cast< const MaturityEvent * >( ce );

    individual_group_t * igroup = m_active_individuals[ me->getMatureObject() ].first;
    system_id mate_id;
    if( igroup == &m_males ) {
        mate_id = m_selection_model->find_mate( ce->getSender(), m_females );
    } else {
        mate_id = m_selection_model->find_mate( ce->getSender(), m_males );
    }

    system_id offspring_id = getIndividual();

    MateEvent * me0 = new MateEvent( getCurrentTime(), getCurrentTime() + 1, getSystemID(), me->getMatureObject(), getNextEventID(), offspring_id );
    MateEvent * me1 = new MateEvent( getCurrentTime(), getCurrentTime() + 1, getSystemID(), mate_id, getNextEventID(), offspring_id );

    sendEvent( me0 );
    sendEvent( me1 );
}

template <>
void TEnvironment<default_life_cycle>::handle_death( const ClothoEvent * ce ) {
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

template < >
void TEnvironment<default_life_cycle>::perform_event( const event * e ) {
    const ClothoEvent * evt = dynamic_cast< const ClothoEvent * >( e );

    if( !evt )  return;

    if( evt->getEventType() == BIRTH_EVENT_K ) {
        handle_birth( evt );
    } else if( evt->getEventType() == MATURITY_EVENT_K ) {
        handle_maturity( evt );
    } else if( evt->getEventType() == DEATH_EVENT_K ) {
        handle_death( evt );
    }
}

//
// TDistributedEnvironment
//
template<>
void TDistributedEnvironment<default_life_cycle>::process() {
    TEnvironment<default_life_cycle>::process();

    updateSnapShot();
    if( m_global_env != m_id ) {
        EnvironmentSnapshotEvent * ese = new EnvironmentSnapshotEvent( getCurrentTime(), getCurrentTime(), this->getSystemID(), m_global_env, getNextEventID(), m_snapshot);
        sendEvent( ese );
    }
}

template<>
void TDistributedEnvironment<default_life_cycle>::perform_event( const event * e) {
    const ClothoEvent * evt = dynamic_cast< const ClothoEvent * >( e );

    if( !evt ) return;

    if( evt->getEventType() == BIRTH_EVENT_K ) {
        handle_birth( evt );
    } else if( evt->getEventType() == MATURITY_EVENT_K ) {
        handle_maturity( evt );
    } else if( evt->getEventType() == DEATH_EVENT_K ) {
        handle_death( evt );
    } else if( evt->getEventType() == SIGNAL_MATE_EVENT_K ) {
        handle_signal_mate( evt );
    } else if( evt->getEventType() == ENVIRONMENT_SNAPSHOT_EVENT_K ) {
        handle_snapshot( evt );
    } else if( evt->getEventType() == SELECTION_EVENT_K ) {
        handle_selection( evt );
    } else {
        cout << "Unhandled event type: " << evt->getEventType() << endl;
        assert(false);
    }
}
template <>
void TDistributedEnvironment<default_life_cycle>::handle_birth( const ClothoEvent * ce ) {
    // assume that the individual will always send a maturity event at time + 1
    const BirthEvent * be = static_cast< const BirthEvent * >( ce );

    MaturityEvent * me = new MaturityEvent( getCurrentTime(), getCurrentTime() + 1, getSystemID(), getSystemID(), getNextEventID(), be->getSender(), be->getSex() );
    sendEvent( me );

    DeathEvent * de = new DeathEvent( getCurrentTime(), getCurrentTime() + 3, getSystemID(), ce->getSender(), getNextEventID() );
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

template <>
void TDistributedEnvironment<default_life_cycle>::handle_maturity( const ClothoEvent * ce ) {
    if(! m_selection_model ) {
        cout << "No Selection model" << endl;
        return;
    }

    const MaturityEvent * me = static_cast< const MaturityEvent *>( ce );

    system_id offspring_id = getIndividual();

    MateEvent * me0 = new MateEvent( getCurrentTime(), getCurrentTime() + 1, getSystemID(), me->getMatureObject(), getNextEventID(), offspring_id );
    sendEvent( me0 );

    SelectionEvent * se = new SelectionEvent( getCurrentTime(), getCurrentTime(), getSystemID(), m_global_env, getNextEventID(), me->getSex(), offspring_id );

    sendEvent( se );
}

template <>
void TDistributedEnvironment<default_life_cycle>::handle_snapshot( const ClothoEvent * ce ) {
    const EnvironmentSnapshotEvent * ese = static_cast< const EnvironmentSnapshotEvent * >( ce );

    // linear scan, but neighbor pool intended to be small (< 10)
    for( size_t i = 0; i < m_neighbors.size(); ++i ) {
        if( m_neighbors[i].first == ese->getSender() ) {
            m_snapshot -= m_neighbors[i].second;
            m_neighbors[i].second = ese->getSnapshot();
            m_snapshot += m_neighbors[i].second;
            break;
        }
    }

    if( m_global_env != m_id ) {
        EnvironmentSnapshotEvent * e2 = new EnvironmentSnapshotEvent( getCurrentTime(), getCurrentTime(), getSystemID(), m_global_env, getNextEventID(), m_snapshot );
        sendEvent( e2 );
    }

//    cout << getSystemID() << " -> " << m_snapshot << endl;
}

template<>
void TDistributedEnvironment<default_life_cycle>::handle_signal_mate( const ClothoEvent * ce ) {
    const SignalMateEvent * sme = static_cast< const SignalMateEvent * >( ce );
    system_id mate_id;

    if( sme->getSex() == FEMALE ) {
        mate_id = m_females[ sme->getIndex() % m_females.size() ];
    } else {
        mate_id = m_males[ sme->getIndex() % m_males.size() ];
    }

    MateEvent * me1 = new MateEvent( getCurrentTime(), getCurrentTime() + 1, getSystemID(), mate_id, getNextEventID(), sme->getOffspring() );
    sendEvent( me1 );
}

template <>
void TDistributedEnvironment<default_life_cycle>::handle_selection( const ClothoEvent * ce ) {
    const SelectionEvent * me = static_cast< const SelectionEvent * >( ce );
    if( getSystemID() != m_global_env ) {
        // this env is not considered a root node
        // thus its snapshot only reflects it and its
        // neighbors (children) individuals

        if( m_rng->nextUniform() <= m_view_threshold ) {
            // determined that the selection of the mate
            // should be passed along to a higher view
            // of the environment
            SelectionEvent * se = new SelectionEvent( getCurrentTime(), getCurrentTime(), getSystemID(), m_global_env, getNextEventID(), me->getSex(), me->getOffspring() );
            sendEvent( se );
            return;
        }
    }


    unsigned int idx = 0;
    if (me->getSex() == MALE ) {
        idx = m_selection_model->find_mate_index( me->getSender(), m_snapshot.female_count );
    } else {
        idx = m_selection_model->find_mate_index( me->getSender(), m_snapshot.male_count );
    }

    SignalMateEvent * sme = NULL;
    if( m_neighbors.size() > 0 ) {
        sme = new SignalMateEvent( getCurrentTime(), getCurrentTime(), getSystemID(), m_neighbors[ idx % m_neighbors.size() ].first, getNextEventID(), ((me->getSex() == MALE) ? FEMALE : MALE), idx, me->getOffspring() );
    } else {
        sme = new SignalMateEvent( getCurrentTime(), getCurrentTime(), getSystemID(), getSystemID(), getNextEventID(), ((me->getSex() == MALE) ? FEMALE : MALE), idx, me->getOffspring() );
    }

    sendEvent( sme );
}
