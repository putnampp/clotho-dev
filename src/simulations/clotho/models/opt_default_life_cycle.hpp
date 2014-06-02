#ifndef OPT_DEFAULT_LIFE_CYCLE_HPP_
#define OPT_DEFAULT_LIFE_CYCLE_HPP_

#include "../clotho.h"

#include "../object/environment_life_cycle.hpp"
#include "../object/individual_life_cycle.hpp"

#include <iostream>

#include "../clotho_event.h"
#include "../event/events.h"
//#include "../event/birth_event.h"
//#include "../event/death_event.h"
//#include "../event/inherit_event.hpp"
//#include "../event/maturity_event.h"
//#include "../event/mate_select_event.h"
//#include "../event/mate_event.h"
//#include "../event/signal_mate_event.h"
//#include "../event/selection_event.h"
//#include "../event/environment_snapshot_event.h"

namespace life_cycle {

/*
 * The opt_default_life_cycle model is a very simple abstraction of a natural
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
struct opt_default_life_cycle : public life_cycle_model {
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
class EnvironmentLifeCycle< opt_default_life_cycle, ENV, ClothoEvent > {
public:
    typedef void (*handler_t)( ENV *, const ClothoEvent * );

    //typedef std::unordered_map< event_type_t, handler_t > handler_map_t;

    static void initialize() {
        //addHandler(BIRTH_EVENT_K, &handle_birth );
        //addHandler(DEATH_EVENT_K, &handle_death );
        //addHandler(MATE_SELECT_EVENT_K, &handle_mate_select );
    }

    static void handle_event( ENV * env, const event * e) {
        const ClothoEvent * evt = dynamic_cast< const ClothoEvent * >( e );
        if( ! evt ) return;

        handle_event(env, evt );
    }

    static void handle_event( ENV * env, const ClothoEvent * evt ) {
        event_type_t tmp = evt->getEventType();
        if( tmp == BirthEvent::TYPE_ID ) {
            handle_birth( env, (const BirthEvent * ) evt );
//        } else if( evt->getEventType() == MATURITY_EVENT_K ) {
//            handle_maturity( env, evt );
        } else if( tmp == DeathEvent::TYPE_ID ) {
            handle_death(env, (const DeathEvent * )evt );
        } else if( tmp == MateSelectEvent::TYPE_ID ) {
            handle_mate_select( env, (const MateSelectEvent * )evt );
        } else {
            assert(false);
        }
    }

protected:

//    static void addHandler( event_type_t key, handler_t h ) {
//        m_handlers[ key ] = h;
//    }

//    static void handle_birth( ENV * env, const ClothoEvent * ce ) {
    static void handle_birth( ENV * env, const BirthEvent * ce ) {
        system_id sender = ce->getSender();
        system_id env_id = ce->getReceiver();

        ClothoEvent::vtime_t ctime = ce->getReceived();
        ClothoObject * co = env->getClothoObject();

        if( env->getActiveIndividualCount() == 0 ) {
//            MateSelectEvent * mse = MateSelectEvent::getOrCreate();
            ClothoEvent::vtime_t mtime = ctime + opt_default_life_cycle::MATE_OFFSET;
            typename ClothoObject::event_id_t n_eid = co->getNextEventID();
            MateSelectEvent *mse = new MateSelectEvent( ctime, mtime, env_id, env_id, n_eid );

            co->sendEvent(mse, env_id, mtime);
        }

//        DeathEvent * de = DeathEvent::getOrCreate();
        ClothoEvent::vtime_t dtime = ctime + opt_default_life_cycle::DEATH_OFFSET;

        typename ClothoObject::event_id_t n_eid = co->getNextEventID();
        DeathEvent * de = new DeathEvent( ctime, dtime, env_id, sender, n_eid );
        co->sendEvent(de, sender, dtime );

        env->activateIndividual( sender );

        delete ce;
    }

//    static void handle_maturity( ENV * env, const ClothoEvent * ce ) {
//        MateSelectEvent * mse = new MateSelectEvent( env->getCurrentTime(), env->getCurrentTime() + opt_default_life_cycle::MATE_OFFSET, env, env, env->getNextEventID() );
//        env->sendEvent( mse );
//    }

//    static void handle_mate_select( ENV * env, const ClothoEvent * ce ) {
    static void handle_mate_select( ENV * env, const MateSelectEvent * ce ) {
        size_t n = env->getActiveIndividualCount();
        size_t nParents = n;

        ClothoEvent::vtime_t ctime = ce->getReceived();
        ClothoObject * co = env->getClothoObject();

        system_id id = co->getSystemID();

        while( n-- ) {
            // get system ids of parent 0 and parent 1
            //std::pair< system_id, system_id > parents = ENV::selection_model_t::select( env, (system_id *)NULL );
            std::pair< size_t, size_t > parents = ENV::selection_model_t::select( nParents  );

            // get system id of their future offspring
            size_t offspring_idx = env->getIndividualIndex();

            system_id p0 = env->getActiveIndividualAt( parents.first ).getClothoObject()->getSystemID();
            system_id p1 = env->getActiveIndividualAt( parents.second ).getClothoObject()->getSystemID();
            system_id child = env->getIndividualAt( offspring_idx ).getClothoObject()->getSystemID();

//            MateEvent * me0 = MateEvent::getOrCreate();
            ClothoObject::event_id_t n_eid = co->getNextEventID(2);
            MateEvent * me0 = new MateEvent( ctime, ctime, id, p0, n_eid++, child );
//            MateEvent * me1 = MateEvent::getOrCreate();
            MateEvent * me1 = new MateEvent( ctime, ctime, id, p1, n_eid, child );

            co->sendEvent( me0, p0, ctime );
            co->sendEvent( me1, p1, ctime );
        }

        delete ce;
    }

//    static void handle_death( ENV * env, const ClothoEvent * ce ) {
    static void handle_death( ENV * env, const DeathEvent * ce ) {
        env->deactivateIndividual( ce->getSender() );

        delete ce;
    }

//    static handler_map_t   m_handlers;
};

//template < class ENV >
//typename EnvironmentLifeCycle< opt_default_life_cycle, ENV, ClothoEvent >::handler_map_t EnvironmentLifeCycle< opt_default_life_cycle, ENV, ClothoEvent >::m_handlers;

template < class IND >
class IndividualLifeCycle< opt_default_life_cycle, IND, ClothoEvent > {
public:
    typedef typename IND::inherit_event_t ievent_t;

    static void handle_event( IND * ind, const event * e ) {
        const ClothoEvent * evt = dynamic_cast< const ClothoEvent * >( e );
        if( !evt ) return;
        handle_event( ind, evt );
    }

    static void handle_event( IND * ind, ClothoEvent * evt ) {
        event_type_t e_id = evt->getEventType();
        if( e_id == BirthEvent::TYPE_ID ) {
            handle_birth( ind, (BirthEvent * )evt );
        } else if( e_id == DeathEvent::TYPE_ID ) {
           handle_death(ind, (DeathEvent * )evt );
        } else if( e_id == IND::inherit_event_t::TYPE_ID ) {
            handle_inherit(ind, (ievent_t * )evt );
        } else if( e_id == MateEvent::TYPE_ID ) {
            handle_mate(ind, (MateEvent * ) evt );
        //} else if( evt->getEventType() == MATURITY_EVENT_K ) {
        //    handle_maturity(ind, evt );
        }
    }

protected:
    static void handle_birth( IND * ind, BirthEvent * evt ){
        ClothoEvent::vtime_t ctime = evt->getReceived();

        ind->getProperties()->setDOB( ctime );

        ClothoObject * co = ind->getClothoObject();

        //BirthEvent * be = BirthEvent::getOrCreate();
        system_id obj_id = co->getSystemID();
        system_id env_id = ind->getEnvironmentID();

        typename ClothoObject::event_id_t n_eid = co->getNextEventID();

//        BirthEvent * be = new BirthEvent( ctime, ctime, obj_id, env_id, n_eid );

        // re-purpose the event
        evt->init( ctime, ctime, obj_id, env_id, n_eid );
        co->sendEvent( evt, env_id, ctime );

//        delete evt;
    }

    static void handle_maturity( IND * ind, MaturityEvent * evt ) {
        ClothoEvent::vtime_t ctime = evt->getReceived();
        ClothoObject * co = ind->getClothoObject();

        system_id obj_id = co->getSystemID();
        system_id env_id = ind->getEnvironmentID();
        ClothoObject::event_id_t n_eid = co->getNextEventID();

//        MaturityEvent * me = new MaturityEvent( ctime, ctime, obj_id, env_id, n_eid, obj_id );

        // re-purpose the event
        evt->init( ctime, ctime, obj_id, env_id, n_eid, obj_id );
        co->sendEvent( evt, env_id, ctime );

        delete evt;
    }

    static void handle_mate( IND * ind, MateEvent * me ) {
        typedef typename IND::properties_t::gamete_t   gamete_t;
        typedef typename gamete_t::pointer gamete_ptr;
        
        gamete_ptr z = IND::reproduction_model_t::reproduce( ind, (gamete_t *) NULL );

        typedef typename IND::inherit_event_t ievent_t;

        ClothoEvent::vtime_t ctime = me->getReceived();
        ClothoObject * co = ind->getClothoObject();

        system_id o_id = me->getOffspringID();
        ClothoObject::event_id_t n_eid = co->getNextEventID();

        ievent_t * ie = new ievent_t( ctime, ctime, co->getSystemID(), o_id, n_eid, z );
        co->sendEvent( ie, o_id, ctime );

        delete me;
    }

    static void  handle_inherit( IND * ind, ievent_t * ie ) {

        //const ievent_t * ie = static_cast< const ievent_t * >( evt );

        typedef typename IND::gamete_t   gamete_t;
        typedef typename gamete_t::pointer gamete_ptr;

        typename IND::properties_t * props = ind->getProperties();
        // assert that alive individual is not inheriting
        // new genetic material
        assert( !props->isAlive() );

        props->inheritFrom( ie->getSender(), (gamete_ptr) ie->getGamete(), ie->getParentIndex() );

        if( props->hasSourceGametes() ) {
            ClothoEvent::vtime_t ctime = ie->getReceived();
            ClothoEvent::vtime_t bday = opt_default_life_cycle::nextGeneration( ctime );

            ind->getProperties()->setDOB(bday);
            ClothoObject * co = ind->getClothoObject();

//            BirthEvent * be = BirthEvent::getOrCreate();
            system_id env_id = ind->getEnvironmentID();
            system_id obj_id = co->getSystemID();
            ClothoObject::event_id_t n_eid = co->getNextEventID();

            BirthEvent * be = new BirthEvent( ctime, bday, obj_id, env_id, n_eid);

            co->sendEvent(be, env_id, bday);
        }

        delete ie;
    }

//    template < typename IND >
    static void handle_death( IND * ind, DeathEvent * evt ) {
        ind->getProperties()->died();

        ClothoEvent::vtime_t ctime = evt->getReceived();
        ClothoObject * co = ind->getClothoObject();

//        DeathEvent * de = DeathEvent::getOrCreate();
        system_id obj_id = co->getSystemID();
        system_id env_id = ind->getEnvironmentID();

        ClothoObject::event_id_t n_eid = co->getNextEventID();
        
//        DeathEvent * de = new DeathEvent( ctime, ctime, obj_id, env_id, n_eid);

        //re-purpose the event
        evt->init( ctime, ctime, obj_id, env_id, n_eid );      
        co->sendEvent( evt, env_id, ctime );
    }
};

}   // namespace life_cycle

#endif  // OPT_DEFAULT_LIFE_CYCLE_HPP_
