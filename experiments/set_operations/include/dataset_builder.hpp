#ifndef DATASET_BUILDER_HPP_
#define DATASET_BUILDER_HPP_

template < class S >
struct dataset_builder;

template < template < class, class... > class SMPLS, class SUBSET, class... REST >
struct dataset_builder< SMPLS< SUBSET, REST...> > {
    typedef SUBSET subset_type;
    typedef SMPLS< SUBSET, REST... > samples_type;

    template < class URNG, class SubsetDist, class ValueDist >
    void operator()( samples_type & samples, unsigned int N, URNG & rgen, SubsetDist & ss, ValueDist & vdist ) {
        std::cerr << "Building sample of " << N << " space subsets" << std::endl;
        for( unsigned int i = 0; i < N; ++i ) {
            unsigned int m = ss( rgen );

            subset_type samp;
            for( unsigned int j = 0; j < m; ++j ) {
                double v = vdist( rgen );
                samp.push_back( v );
            }

            std::sort( samp.begin(), samp.end() );
            samples.push_back(samp);
        }
    }
};

#endif  // DATASET_BUILDER_HPP_
