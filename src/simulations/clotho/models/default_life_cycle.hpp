#ifndef DEFAULT_LIFE_CYCLE_HPP_
#define DEFAULT_LIFE_CYCLE_HPP_

#include "../clotho.h"

#include "../object/environment_life_cycle.hpp"
#include "../object/individual_life_cycle.hpp"

#include <iostream>

#include "../event/clotho_event.h"
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

/*
 * The def_life_cycle model is a very simple abstraction of a natural
 * life cycle.  In this model, all individuals encounter the same
 * life cycle events at the same time.  There are 4 phases to this 
 * life cycle: Birth, Maturity, Mating, and Death.  For simplicity,
 * all individuals are born at t_0, mature at t_1 = t_0 + MATURITY_OFFSET,
 * mate at t_2 = t_1 + MATE_OFFSET, and die at t_3 = t_0 + DEATH_OFFSET.
 *
 * The life cycle starts with the birth of an individual.
 * When an individual handles a birth event, its DOB is updated to the
 * current simulation time and then notifies the environment of its
 * birth. The environment notification occurs with in the same "time step"
 * as the individual being born.  Upon notification of an individual's
 * birth, the environment generates a Maturity event
 * for the individual, and Death event for the individual.  Additionally,
 * the environment adds the individual to its "active" set.
 *
 * The next time step starts with an individual becoming mature.
 * A mature individual simply notifies the environment of its maturity.
 * The environment handles this notification by scheduling a mate_select event.
 * 
 * The mate select process starts the Mate phase. The mate select process
 * selects two parents from the current environment using the selection
 * procedure associated with the environment. An individual is created
 * to be used as their offspring.  A mate_event is sent to each parent.
 * When an individual encounters a mate_event, they produce a gamete
 * following the reproduction process associated with the individual.
 * The gamete is then sent to the offspring individual via an inherit_event.
 * Inheritance essentially sets the individual gamete property accordingly.
 * In this model, gamete index is "unknown"/"ignored". This is analogous
 * to not considering the gamete source's gender in mating.  Once both
 * parents have supplied their gamete to the offspring, the offspring's
 * birth is scheduled for the start of the next generation.
 *
 * Finally, the current generation enters the Death phase.  This starts
 * with the individual changing its state to being not alive (reseting
 * DOB to be POSITIVE_INFINITY).  It then notifies the environment of its
 * death.  The environment then simply removes the individual from its
 * active set.
 *
 * This life_cycle makes several "assumptions" about the natural life cycle.
 * First, all individuals follow the same life cycle. This means that all
 * individuals are born, mature, and die.  Whether they actually reproduce
 * to the next generation is left to the Selection process defined by environment.
 * Second, it assumes that by default all individuals require two parents
 * (mating population).  Third, this model assumes a constant population
 * where all mating individuals always produce an offspring.  In other words,
 * every mate_select event will result an offspring in the next generation.
 */
struct def_life_cycle : public life_cycle_model {
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

template < > 
class EnvironmentLifeCycle< def_life_cycle > {
public:
    template < class ENV >
    static void handle_event( ENV * env, const event * e) {
        const ClothoEvent * evt = dynamic_cast< const ClothoEvent * >( e );
        if( ! evt ) return;

        handle_event(env, evt );
    }

    template < class ENV >
    static void handle_event( ENV * env, const ClothoEvent * evt ) {
        if( evt->getEventType() == BIRTH_EVENT_K ) {
            handle_birth( env, evt );
        } else if( evt->getEventType() == MATURITY_EVENT_K ) {
            handle_maturity( env, evt );
        } else if( evt->getEventType() == DEATH_EVENT_K ) {
            handle_death(env, evt );
        } else if( evt->getEventType() == MATE_SELECT_EVENT_K ) {
            handle_mate_select( env, evt );
        }
    }

protected:
    template < class ENV >
    static void handle_birth( ENV * env, const ClothoEvent * ce ) {
        const BirthEvent * be = static_cast< const BirthEvent * >( ce );

        event::vtime_t ctime = env->getCurrentTime();
        MaturityEvent * me = new MaturityEvent( ctime, ctime + def_life_cycle::MATURITY_OFFSET, env->getSystemID(), ce->getSender(), env->getNextEventID(), be->getSender() );
        env->sendEvent( me );

        DeathEvent * de = new DeathEvent( ctime, ctime + def_life_cycle::DEATH_OFFSET, env->getSystemID(), ce->getSender(), env->getNextEventID() );
        env->sendEvent(de);

        env->activateIndividual( be->getSender() );
    }

    template < class ENV >
    static void handle_maturity( ENV * env, const ClothoEvent * ce ) {
        MateSelectEvent * mse = new MateSelectEvent( env->getCurrentTime(), env->getCurrentTime() + def_life_cycle::MATE_OFFSET, env, env, env->getNextEventID() );
        env->sendEvent( mse );
    }

    template < class ENV >
    static void handle_mate_select( ENV * env, const ClothoEvent * ce ) {

        // get system ids of parent 0 and parent 1
        std::pair< system_id, system_id > parents = ENV::selection_model_t::select( env, (std::pair< system_id, system_id> *)NULL );

        // get system id of their future offspring
        system_id offspring_id = env->getIndividual();

        ClothoEvent::vtime_t ctime = env->getCurrentTime();

        MateEvent * me0 = new MateEvent( ctime, ctime, env->getSystemID(), parents.first, env->getNextEventID(), offspring_id );
        MateEvent * me1 = new MateEvent( ctime, ctime, env->getSystemID(), parents.second, env->getNextEventID(), offspring_id );

//        std::cout << parents.first << " + " << parents.second << " = " << offspring_id << std::endl;

        env->sendEvent( me0 );
        env->sendEvent( me1 );
    }

    template < class ENV >
    static void handle_death( ENV * env, const ClothoEvent * ce ) {
        env->deactivateIndividual( ce->getSender() );
    }
};

template < >
class IndividualLifeCycle< def_life_cycle > {
public:
    template < typename IND >
    static void handle_event( IND * ind, const event * e ) {
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
        } else if( evt->getEventType() == MATURITY_EVENT_K ) {
            handle_maturity(ind, evt );
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
        
        gamete_t * z = IND::reproduction_model_t::reproduce( ind );

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
            bday = def_life_cycle::nextGeneration( bday );

            BirthEvent * be = new BirthEvent( ind->getCurrentTime(), bday, ind, ind, ind->getNextEventID() );
            ind->sendEvent( be );
        }
    }

    template < typename IND >
    static void handle_death( IND * ind, const ClothoEvent * evt ) {
        ind->m_prop->died();

        DeathEvent * de = new DeathEvent( ind->getCurrentTime(), evt->getReceived(), ind->getSystemID(), ind->m_env_id, ind->getNextEventID());
        ind->sendEvent( de );
    }
};

}   // namespace life_cycle

#endif  // DEFAULT_LIFE_CYCLE_HPP_