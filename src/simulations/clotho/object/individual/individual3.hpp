#ifndef INDIVIDUAL3_HPP_
#define INDIVIDUAL3_HPP_

#include "config.hpp"

#include "object/life_cycles/detail/life_cycle_handler.hpp"

#include "initializer.hpp"
#include "finalizer.hpp"

namespace objects {
namespace individual {

template < class G, unsigned char P, class S, class PROP >
class TIndividual {
public:
    typedef G gamete_type;
    typedef S source_type;
    typedef PROP properties_type;

    typedef TIndividual< G, P, S, PROP > * pointer;
    typedef typename gamete_type::pointer  gamete_pointer;

    static const unsigned char PLOIDY = P;

    friend class initializer::InitWorker< TIndividual< G, P, S, PROP > >;
    friend class finalizer::FinalWorker< TIndividual< G, P, S, PROP > >;

    TIndividual();
    //TIndividual( ClothoObject * co, ClothoObject * env );
    TIndividual( const TIndividual< G, P, S, PROP > & ind );

    void reset();

    static gamete_pointer reproduce( pointer ind );

    void initialize();
    void finalize();

    properties_type * getProperties();

    virtual ~TIndividual();

#if defined USE_FRIENDS
    template < typename O, typename E, typename L > 
    friend struct life_cycle::detail::life_cycle_handler;
protected:
#endif

    properties_type m_props;
};

}   // namespace individual
}   // namespace objects

#endif  // INDIVIDUAL3_HPP_
