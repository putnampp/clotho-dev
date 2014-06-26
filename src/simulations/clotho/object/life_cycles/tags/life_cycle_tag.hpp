#ifndef LIFE_CYCLE_TAG_HPP_
#define LIFE_CYCLE_TAG_HPP_

namespace life_cycle {
namespace tags {

struct life_cycle_tag {
    static const unsigned int GENERATION_PERIOD = 100;
    static const unsigned int MATURITY_OFFSET = 10;
    static const unsigned int MATE_OFFSET = 10;
    static const unsigned int DEATH_OFFSET = 99;

    static SystemClock::vtime_t convertGenerationsToVTime( unsigned int gen ) {
        return (SystemClock::vtime_t)(( gen + 1 )*GENERATION_PERIOD);
    }

    static SystemClock::vtime_t nextGeneration( SystemClock::vtime_t cur ) {
        return (((cur / GENERATION_PERIOD) + 1) * GENERATION_PERIOD);
    }
};

} // tags
} // life_cycle
#endif  // LIFE_CYCLE_TAG_HPP_
