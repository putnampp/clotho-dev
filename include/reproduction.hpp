#ifndef REPRODUCTION_HPP_
#define REPRODUCTION_HPP_

namespace reproduction {

namespace models {

template < class P >
struct model {
    typedef P parameter_t;
};

}   // namespace models

template < class MODEL = models::model<void> >
class IndividualReproduction {
public:
    typedef MODEL model_t;
    typedef typename MODEL::parameter_t parameter_t;

    template < typename IND, typename GAM = typename IND::properties_t::gamete_t >
    static GAM * reproduce( IND * ind ) {
        return reproduce( ind, ind->reproduction_parameter() );
    }

    template < typename IND, typename GAM = typename IND::properties_t::gamete_t >
    static GAM * reproduce( IND * ind, parameter_t  * c ) {
        GAM * res = new GAM();
        return res;
    }

};

} // namespace reproduction

#endif  // REPRODUCTION_HPP_
