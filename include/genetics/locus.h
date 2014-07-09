#ifndef LOCUS_H_
#define LOCUS_H_

typedef double Locus;

template < typename L, typename P >
struct locus_generator {
    typedef L return_type;
    typedef P parameter_type;

    locus_generator() {}
    locus_generator( parameter_type p ) : m_params(p) {}
    
    return_type operator()() { }

    parameter_type m_params;
};


//#include <functional>

//class Locus {
//public:
//    typedef double key_t;
//
//    Locus( key_t k );
//
//    inline key_t getKey() const { return m_key; }
//
//    virtual ~Locus();
//protected:
//    key_t       m_key;
//    alleles_t   m_alleles;
//};

//namespace std {
//
//template <>
//struct hash< Locus > {
//    typedef Locus arugment_type;
//    typedef std::size_t value_type;
//
//    value_type operator()( argument_type const& l ) const {
//        return key_hasher( l.getKey() );
//    }
//    static std::hash< typename Locus::key_t > key_hasher;
//};

//template <>
//struct equal_to< Locus > {
//    typedef bool result_type;
//    typedef Locus first_argument_type;
//    typedef Locus second_argument_type;
//
//    result_type operator()( const first_argument_type & lhs, const second_argument_type & rhs ) cosnt {
//        return lhs.getKey() == rhs.getKey();
//    }
//};
//
//} // namespace std

#endif  // LOCUS_H_
