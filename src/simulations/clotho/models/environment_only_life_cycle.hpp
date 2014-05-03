#ifndef ENVIRONMENT_ONLY_LIFE_CYCLE_HPP_
#define ENVIRONMENT_ONLY_LIFE_CYCLE_HPP_

#include "../clotho.h"

#include "../object/environment_life_cycle.hpp"
#include "../object/individual_life_cycle.hpp"

#include <iostream>

#include "../clotho_event.h"
#include "../event/birth_event.h"
#include "../event/death_event.h"
#include "../event/inherit_event.hpp"
#include "../event/maturity_event.h"
#include "../event/mate_select_event.h"
#include "../event/mate_event.h"
#include "../event/signal_mate_event.h"
#include "../event/selection_event.h"
#include "../event/environment_snapshot_event.h"

namespace life_cycle {

struct environment_only_life_cycle : public life_cycle_model {
    // Note: these offset and padding values are completely
    // arbitrary.  They are for debugging purposes
    // only.  The idea being that each generation always
    // starts at time t, where (t % LIFE_CYCLE_PADDING == 0)
    static const unsigned int LIFE_CYCLE_PADDING = 100;
    static const unsigned int MATURITY_OFFSET = 10;
    static const unsigned int MATE_OFFSET = 10;
    static const unsigned int DEATH_OFFSET = 99;

    static SystemClock::vtime_t convertGenerationsToVTime( unsigned int gen ) {
        return (SystemClock::vtime_t)(( gen + 1 )*LIFE_CYCLE_PADDING);
    }

    static SystemClock::vtime_t nextGeneration( SystemClock::vtime_t cur ) {
        return (((cur / LIFE_CYCLE_PADDING) + 1) * LIFE_CYCLE_PADDING);
    }
};

template < class ENV > 
class EnvironmentLifeCycle< environment_only_life_cycle, ENV, ClothoEvent > {
public:
    static void initialize() {}

    static void handle_event( ENV * env, const event * e) {
        assert(false);
        const ClothoEvent * evt = dynamic_cast< const ClothoEvent * >( e );
        if( ! evt ) return;

        handle_event(env, evt );
    }

    static void handle_event( ENV * env, const ClothoEvent * evt ) {
        event_type_t e_id = evt->getEventType();
        if( e_id == BirthEvent::TYPE_ID ) {
            handle_birth( env, evt );
        } else if( e_id == DeathEvent::TYPE_ID ) {
            handle_death(env, evt );
        } else if( e_id == MateSelectEvent::TYPE_ID ) {
            handle_mate_select( env, evt );
        }
    }

protected:
    static void handle_birth( ENV * env, const ClothoEvent * ce ) {
//        while( !env->m_pending.empty() ) {
//            system_id id = *env->m_pending.begin();
//            env->m_pending.erase( env->m_pending.begin());

//            typename ENV::individual_t * ind  = dynamic_cast< typename ENV::individual_t * >( env->m_sim_manager->getObject( id ) );
//            ind->getProperties()->setDOB( ce->getReceived() );
//           env->activateIndividual(id);
//        }

        ClothoEvent::vtime_t ctime = ce->getReceived();

        size_t idx = env->activateNextIndividual(); // activate next pending individual
        while( idx != ENV::active_pool_t::UNKNOWN_INDEX ) {
            env->m_individuals[idx].getProperties()->setDOB( ctime );            
            idx = env->activateNextIndividual();
        }

        MateSelectEvent * mse = new MateSelectEvent( ctime, ctime + environment_only_life_cycle::MATE_OFFSET, env, env, env->getNextEventID() );
        env->sendEvent(mse);
    }

    static void handle_mate_select( ENV * env, const ClothoEvent * ce ) {

        size_t nMateOps = env->getActiveIndividualCount();
        while( nMateOps-- ) {
            // get system ids of parent 0 and parent 1
            std::pair< system_id, system_id > parents = ENV::selection_model_t::select( env, (system_id *)NULL );

            // get system id of their future offspring
            system_id offspring_id = env->getIndividual();  // child is left in pending queue until birth event is handled

            typename ENV::individual_t & p0 = env->getIndividual( parents.first );
            typename ENV::individual_t & p1 = env->getIndividual( parents.second );
            typename ENV::individual_t & child = env->getIndividual( offspring_id );

            typedef typename ENV::individual_t::properties_t::gamete_t   gamete_t;
            gamete_t * z = ENV::individual_t::reproduction_model_t::reproduce( &p0, (gamete_t *) NULL );
            child.getProperties()->inheritFrom( parents.first, z );

            z = ENV::individual_t::reproduction_model_t::reproduce( &p1, (gamete_t *) NULL );
            child.getProperties()->inheritFrom( parents.second, z );
        }

        DeathEvent * de = new DeathEvent( ce->getReceived(), ce->getReceived() + environment_only_life_cycle::DEATH_OFFSET, env, env, env->getNextEventID() );
        env->sendEvent(de);
    }

    static void handle_death( ENV * env, const ClothoEvent * ce ) {
/*        while( !env->m_active_individuals.empty() ) {
            system_id id = env->m_active_individuals.begin()->first;
            typename ENV::individual_t * ind = dynamic_cast< typename ENV::individual_t * >( env->m_sim_manager->getObject(id));
            ind->getProperties()->died();
            env->deactivateIndividual( id );
        }
*/
        size_t idx = env->inactivateNextIndividual();
        while( idx != ENV::active_pool_t::UNKNOWN_INDEX ) {
            env->m_individuals[idx].getProperties()->died();
            idx = env->inactivateNextIndividual();
        }
        event::vtime_t bday = environment_only_life_cycle::nextGeneration( ce->getReceived() );

        BirthEvent * be = new BirthEvent(ce->getReceived(), bday, env, env, env->getNextEventID() );
        env->sendEvent( be );
    }
};
/*
template < >
class IndividualLifeCycle< environment_only_life_cycle > {
public:
    template < typename IND >
    static void handle_event( IND * ind, const event * e ) {
        assert(false);
        const ClothoEvent * evt = dynamic_cast< const ClothoEvent * >( e );
        if( !evt ) return;
        handle_event( ind, evt );
    }

    template < typename IND >
    static void handle_event( IND * ind, const ClothoEvent * evt ) {
        if( evt->getEventType() == BIRTH_EVENT_K ) {
            handle_birth( ind, evt );
        } else if( evt->getEventType() == DEATH_EVENT_K ) {
           handle_death(ind, evt );
        } else if( evt->getEventType() == INHERIT_EVENT_K ) {
            handle_inherit(ind, evt );
        } else if( evt->getEventType() == MATE_EVENT_K ) {
            handle_mate(ind, evt );
        //} else if( evt->getEventType() == MATURITY_EVENT_K ) {
        //    handle_maturity(ind, evt );
        }
    }

protected:
    template < typename IND >
    static void handle_birth( IND * ind, const ClothoEvent * evt ){
        ind->m_prop->setDOB( evt->getReceived() );

        BirthEvent * be = new BirthEvent( ind->getCurrentTime(), evt->getReceived(), ind->getSystemID(), ind->m_env_id, ind->getNextEventID() );
        ind->sendEvent( be );
    }

    template < typename IND >
    static void handle_maturity( IND * ind, const ClothoEvent * evt ) {
        MaturityEvent * me = new MaturityEvent( ind->getCurrentTime(), ind->getCurrentTime(), ind->getSystemID(), ind->m_env_id, ind->getNextEventID(), ind->getSystemID() );
        ind->sendEvent( me );
    }

    template < typename IND >
    static void handle_mate( IND * ind, const ClothoEvent * evt ) {
        const MateEvent * me  = static_cast< const MateEvent * >( evt );

        typedef typename IND::properties_t::gamete_t   gamete_t;
        
        gamete_t * z = IND::reproduction_model_t::reproduce( ind, (gamete_t *) NULL );

        typedef typename IND::inherit_event_t ievent_t;
        ievent_t * ie = new ievent_t( ind->getCurrentTime(), ind->getCurrentTime(), ind->getSystemID(), me->getOffspringID(), ind->getNextEventID(), z );
        ind->sendEvent( ie );
    }

    template < typename IND >
    static void handle_inherit( IND * ind, const ClothoEvent * evt ) {
        typedef typename IND::inherit_event_t ievent_t;
        const ievent_t * ie = static_cast< const ievent_t * >( evt );

        // assert that alive individual is not inheriting
        // new genetic material
        assert( !ind->m_prop->isAlive() );

        ind->m_prop->inheritFrom( ie->getSender(), ie->getGamete(), ie->getParentIndex() );

        if( ind->m_prop->hasSourceGametes() ) {
            event::vtime_t bday = evt->getReceived();
            bday = environment_only_life_cycle::nextGeneration( bday );

//            BirthEvent * be = new BirthEvent( ind->getCurrentTime(), bday, ind, ind, ind->getNextEventID() );
//            ind->sendEvent( be );
            ind->m_prop->setDOB(bday);
            BirthEvent * be = new BirthEvent( ind->getCurrentTime(), bday, ind->getSystemID(), ind->m_env_id, ind->getNextEventID());
            ind->sendEvent(be);
        }
    }

    template < typename IND >
    static void handle_death( IND * ind, const ClothoEvent * evt ) {
        ind->m_prop->died();

        DeathEvent * de = new DeathEvent( ind->getCurrentTime(), evt->getReceived(), ind->getSystemID(), ind->m_env_id, ind->getNextEventID());
        ind->sendEvent( de );
    }
};
*/
}   // namespace life_cycle

#endif  // ENVIRONMENT_ONLY_LIFE_CYCLE_HPP_
