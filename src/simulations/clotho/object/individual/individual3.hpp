#ifndef INDIVIDUAL3_HPP_
#define INDIVIDUAL3_HPP_

#include "config.hpp"

namespace objects {
namespace individual {

template < class G, unsigned char P, class S, class PROP >
class TIndividual : public AppObject {
public:
    typedef G gamete_type;
    typedef S source_type;
    typedef PROP properties_type;

    typedef TIndividual< G, P, S, PROP > * pointer;
    typedef typename gamete_type::pointer  gamete_pointer;
    
    static const unsigned char PLOIDY = P;

    TIndividual();
    TIndividual( ClothoObject * co, ClothoObject * env );
    TIndividual( const TIndividual< G, P, S, PROP > & ind );

    void reset();

    static gamete_pointer reproduce( pointer ind );

    void initialize();
    void finalize();

    properties_type * getProperties();

    virtual ~TIndividual();
protected:
    properties_type m_props;
};

}   // namespace individual
}   // namespace objects

#endif  // INDIVIDUAL3_HPP_
