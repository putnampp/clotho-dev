#ifndef NO_RECOMBINATION_TCC_
#define NO_RECOMBINATION_TCC_

namespace reproduction {
namespace models {
namespace recombination {

template < unsigned char P >
class no_recomb : public RandomProcess {
public:
    template < class IP, class GM >
    static typename GM::pointer recombine( IP * ind, GM * ) {
        typename GM::pointer g = ind->getGamete( m_rng->nextInt( P ) );
        return g;
    }
};

template <>
template < class IP, class GM >
typename GM::pointer no_recomb< 1 >::recombine( IP * ind, GM * ) {
    return ind->getGamete( 0 );
}

template <>
template < class IP, class GM >
typename GM::pointer no_recomb< 2 >::recombine( IP * ind, GM * ) {
    return ind->getGamete( (m_rng->nextBool() ? 1 : 0) );
}

}   // namespace recombination
}   // namespace models
}   // namespace reproduction
#endif  // NO_RECOMBINATION_TCC_
