#include "distributed_environment.h"

#include "../event/birth_event.h"
#include "../event/death_event.h"
#include "../event/mate_event.h"
#include "../event/environment_snapshot_event.h"
#include "../event/signal_mate_event.h"
#include "../event/selection_event.h"
#include "../event/maturity_event.h"

DistributedEnvironment::DistributedEnvironment( simulation_manager * manager, const system_id & global_env ) :
    Environment( manager ),
    m_global_env( global_env ),
    m_snapshot(),
    m_view_threshold( 0.5 ) {
    setSimulationManager( manager );
    if( m_global_env == DefaultSystemID ) {
        m_global_env = getSystemID();
        m_neighbors.push_back( make_pair( getSystemID(), m_snapshot ) );  // root is its own neighbor
    }
}

DistributedEnvironment::DistributedEnvironment( simulation_manager * manager, const system_id & global_env, GeneticMap::Ptr gmap, selection_model * s, reproduction * r, shared_ptr< iRNG > rng, double threshold ) :
    Environment( manager, gmap, s, r ),
    m_global_env( global_env ),
    m_snapshot(),
    m_rng( rng->clone() ),
    m_view_threshold( threshold ) {
    setSimulationManager( manager );
    if( m_global_env == DefaultSystemID ) {
        m_global_env = getSystemID();
        m_neighbors.push_back( make_pair( getSystemID(), m_snapshot ) );  // root is its own neighbor
    }
}


void DistributedEnvironment::process() {
    Environment::process();

    updateSnapShot();

    if( m_global_env != m_id ) {
        EnvironmentSnapshotEvent * ese = new EnvironmentSnapshotEvent( getCurrentTime(), getCurrentTime(), this->getSystemID(), m_global_env, getNextEventID(), m_snapshot);
        sendEvent( ese );
    }
}

void DistributedEnvironment::perform_event( const event * e) {
    const ClothoEvent * evt = dynamic_cast< const ClothoEvent * >( e );

    if( evt ) {
//        cout << "DistEnv-" << getSystemID() << "@" << getCurrentTime() << ": " << evt << endl;
        event_type_t e_id = evt->getEventType();
        if( e_id == BirthEvent::TYPE_ID ) {
            handle_birth( evt );
        } else if( e_id == MaturityEvent::TYPE_ID ) {
            handle_maturity( evt );
        } else if( e_id == DeathEvent::TYPE_ID ) {
            handle_death( evt );
        } else if( e_id == SignalMateEvent::TYPE_ID) {
            handle_signal_mate( evt );
        } else if( e_id == EnvironmentSnapshotEvent::TYPE_ID ) {
            handle_snapshot( evt );
        } else if( e_id == SelectionEvent::TYPE_ID ) {
            handle_selection( evt );
        } else {
            assert(false);
        }
    }
}

void DistributedEnvironment::updateSnapShot() {
    m_snapshot.male_count = m_males.size();
    m_snapshot.female_count = m_females.size();
    m_snapshot.pending_count = m_pending.size();
    m_snapshot.available_count = m_available_individuals.size();
}

void DistributedEnvironment::addNeighborEnvironment( const system_id & env_id ) {
    bool bDuplicate = false;
    for( neighborhood_t::iterator it = m_neighbors.begin(); it != m_neighbors.end(); it++ ) {
        if( (*it).first == env_id ) {
            bDuplicate = true;
            break;
        }
    }

    if( !bDuplicate ) {
        m_neighbors.push_back( make_pair( env_id, EnvironmentSnapshot() ) );

        cout << m_id << " -> " << env_id << endl;
    }
}

void DistributedEnvironment::handle_birth( const ClothoEvent * ce ) {
    // assume that the individual will always send a maturity event at time + 1
    const BirthEvent * be = static_cast< const BirthEvent * >( ce );

//    MaturityEvent * me = new MaturityEvent( getCurrentTime(), getCurrentTime() + 1, getSystemID(), ce->getSender(), getNextEventID() , be->getSex() );
    MaturityEvent * me = MaturityEvent::getOrCreate();
    me->init( getCurrentTime(), getCurrentTime() + 1, getSystemID(), getSystemID(), getNextEventID() , be->getSender(), be->getSex() );
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

void DistributedEnvironment::handle_maturity( const ClothoEvent * ce ) {
    if(! m_selection_model ) {
        cout << "No Selection model" << endl;
        return;
    }

    const MaturityEvent * me = static_cast< const MaturityEvent *>( ce );

    system_id offspring_id = getIndividual();

    MateEvent * me0 = MateEvent::getOrCreate();
    me0->init( getCurrentTime(), getCurrentTime() + 1, getSystemID(), ce->getSender(), getNextEventID(), offspring_id );
    sendEvent( me0 );

    SelectionEvent * se = new SelectionEvent( getCurrentTime(), getCurrentTime(), getSystemID(), m_global_env, getNextEventID(), me->getSex(), offspring_id );

    sendEvent( se );
}

void DistributedEnvironment::handle_snapshot( const ClothoEvent * ce ) {
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
}

void DistributedEnvironment::handle_signal_mate( const ClothoEvent * ce ) {
    const SignalMateEvent * sme = static_cast< const SignalMateEvent * >( ce );
    system_id mate_id;

    if( sme->getSex() == FEMALE ) {
        mate_id = m_females[ sme->getIndex() % m_females.size() ];
    } else {
        mate_id = m_males[ sme->getIndex() % m_males.size() ];
    }

    MateEvent * me1 = MateEvent::getOrCreate();
    me1->init( getCurrentTime(), getCurrentTime() + 1, getSystemID(), mate_id, getNextEventID(), sme->getOffspring() );
    sendEvent( me1 );
}

//void DistributedEnvironment::determineMate( const MaturityEvent * me ) {
void DistributedEnvironment::handle_selection( const ClothoEvent * ce ) {
    const SelectionEvent * me = static_cast< const SelectionEvent * >( ce );
    if( getSystemID() != m_global_env ) {
        // this env is not considered a root node
        // thus its snapshot only reflects it and its
        // neighbors individuals

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

//    system_id offspring_id = getIndividual();

//    MateEvent * me0 = new MateEvent( getCurrentTime(), getCurrentTime() + 1, getSystemID(), me->getSender(), getNextEventID(), offspring_id );
//    sendEvent( me0 );

    SignalMateEvent * sme = NULL;
    if( m_neighbors.size() > 0 ) {
        sme = new SignalMateEvent( getCurrentTime(), getCurrentTime(), getSystemID(), m_neighbors[ idx % m_neighbors.size() ].first, getNextEventID(), ((me->getSex() == MALE) ? FEMALE : MALE), idx, me->getOffspring() );
    } else {
        sme = new SignalMateEvent( getCurrentTime(), getCurrentTime(), getSystemID(), getSystemID(), getNextEventID(), ((me->getSex() == MALE) ? FEMALE : MALE), idx, me->getOffspring() );
    }

    sendEvent( sme );
}
