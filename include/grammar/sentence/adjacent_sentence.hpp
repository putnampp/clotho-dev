#ifndef ADJACENT_SENTENCE_HPP_
#define ADJACENT_SENTENCE_HPP_

#include "grammar/sentence/sentence_def.hpp"
#include "grammar/lexicon.hpp"

#include "utility/bitset_iterator.hpp"

#include <boost/dynamic_bitset.hpp>

namespace grammar {
namespace sentences {

template < typename W, typename C, typename B, typename BA, typename S = boost::mpl::na >
class sentence < grammar::lexicons::lexicon< W, C, boost::dynamic_bitset<B, BA>::size_type >,
                W, boost::dynamic_bitset< B >, S > {
public:
    typedef boost::dynamic_bitset< B, BA >          word_group_type;
    typedef typename word_group_type::size_type     word_reference;

    typedef W                                       word_type;
    typedef grammar::lexicons::lexicon< word_type, C, word_reference >   lexicon_type;

    typedef S                                       syntax_type;

    typedef sentence< lexicon_type, word_type, word_group_type, syntax_type > this_type;
    typedef this_type * pointer;

    typedef bitset_iterator< word_group_type > iterator;

    static const this_type EMPTY;

    sentence();
    sentence( const this_type & s );

    pointer copy();
    pointer clone() const;

    void    addWord( const word_type & w );
    void    addWord( const word_reference & r );

    size_t  word_count();

    iterator    begin();
    iterator    end();

    void    reset();

    virtual ~sentence();
protected:
    static lexicon_type m_lexicon;

    size_t          m_ref_count;
    word_group_type m_words;
};

//
//  IMPLEMENTATION
//

#define THEADER template < typename W, typename C, typename R, typename B, typename S >
#define SPECIALIZATION sentence < grammar::lexicons::lexicon< W, C, R >, R, boost::dynamic_bitset< B >, S >

THEADER
typename SPECIALIZATION::lexicon_type   SPECIALIZATION::m_lexicon;

THEADER
SPECIALIZATION::sentence() : m_ref_count(1) {}

THEADER
SPECIALIZATION::sentence( const this_type & ot) : m_ref_count(ot.m_ref_count), m_words( ot.m_words ) {}

THEADER
SPECIALIZATION::pointer SPECIALIZATION::copy() {
    ++m_ref_count;
    return this;
}

THEADER
SPECIALIZATION::pointer SPECIALIZATION::clone() const {
    pointer tmp = new this_type( *this );
    tmp->m_ref_count = 1;

    return tmp;
}

THEADER
void    SPECIALIZATION::addWord( const word_type & w ) {
    word_reference wr = m_lexicon.reference_to( w, true );
    addWord( wr );
}

THEADER
void    SPECIALIZATION::addWord( const word_reference & w ) {
    if( w >= m_words.size() ) { m_words.resize( w + 1 ); }

    m_words[ w ] = true;
}

THEADER
size_t  SPECIALIZATION::word_count() {
    return m_words.count();
}

THEADER
SPECIALIZATION::iterator SPECIALIZATION::begin() {
    return iterator( &m_words, 0);
}

THEADER
SPECIALIZATION::iterator SPECIALIZATION::end() {
    return iterator( this );
}

THEADER
void SPECIALIZATION::reset() {
    if( --m_ref_count == 0 ) {
        delete this;
    }
}

THEADER
SPECIALIZATION::~sentence() {}

#undef THEADER
#undef SPECIALIZATION
}   // namespace sentences
}   // namespace grammar


#endif  // ADJACENT_SENTENCE_HPP_
