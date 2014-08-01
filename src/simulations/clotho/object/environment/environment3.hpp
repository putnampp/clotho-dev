#ifndef ENVIRONMENT3_HPP_
#define ENVIRONMENT3_HPP_

#include "config.hpp"
namespace objects {
namespace environment {

template< class POP >
class TEnvironment : public AppObject {
public:
    typedef POP                             population_type;
    typedef typename POP::individual_type   individual_type;

    typedef individual_type *               individual_pointer;

    TEnvironment();
    TEnvironment( ClothoObject * co );
    TEnvironment( const TEnvironment< POP > & env);

    void initialize();
    void finalize();

    individual_pointer  allocateIndividual();
    void                deallocate( individual_pointer ind );

    virtual ~TEnvironment();
protected:
    population_type    m_individuals;
};

template < template< class T1, class... > class TT, class T1, class... REST >
class TEnvironment< TT< T1, REST...> > : public AppObject {
public:
    typedef T1                  individual_type;
    typedef TT< T1, REST... >   population_type;

    typedef individual_type *   individual_pointer;

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

} // namespace environment
} // namespace object

#endif  // ENVIRONMENT3_HPP_
