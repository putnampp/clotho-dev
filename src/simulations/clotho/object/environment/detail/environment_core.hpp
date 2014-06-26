#ifndef ENVIRONMENT_CORE_GUARD_HPP_
#define ENVIRONMENT_CORE_GUARD_HPP_

#include "object/environment/environment3.hpp"

namespace object {
namespace environment {

#include <vector>

template < class IND >
class TEnvironment< std::vector< IND > > : public AppObject {
public:
    typedef IND                 individual_type;
    typedef std::vector< IND >  population_type;

    typedef IND *               individual_pointer;

    TEnvironment();
    TEnvironment( ClothoObject * co );
    TEnvironment( const TEnvironment< population_type > & env);

    void initialize();
    void finalize();

    individual_pointer  allocateIndividual();
    void                deallocate( individual_pointer ind );

    virtual ~TEnvironment();
protected:
    population_type    m_individuals;
};

//
// Implementation
//

#define BASE_TEMPLATE template < class IND >
#define SPECIALIZATION TEnvironment< std::vector< IND > >
#define STRUCTOR( m ) BASE_TEMPLATE SPECIALIZATION::m
#define DEFINE_METHOD( t, m ) BASE_TEMPLATE t SPECIALIZATION::m
#define CONSTRUCTOR() STRUCTOR( SPECIALIZATION )
#define DESTRUCTOR()  STRUCTOR( ~SPECIALIZATION )

CONSTRUCTOR()()
{}

CONSTRUCTOR()( ClothoObject * co )
{}

CONSTRUCTOR()( const SPECIALIZATION & env )
{}

DESTRUCTOR()()
{}

#undef BASE_TEMPLATE
#undef SPECIALIZATION
#undef STRUCTOR
#undef DEFINE_METHOD

}
}

#endif  // ENVIRONMENT_CORE_GUARD_HPP_
