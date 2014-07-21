#ifndef GRAMMAR_SENTENCE_DEF_HPP_
#define GRAMMAR_SENTENCE_DEF_HPP_

#include <boost/mpl/aux_/na.hpp>
#include "grammar/sentence/word_iterator.hpp"

namespace grammar {
namespace sentences {

template < typename L, typename W, typename WG, typename S = boost::mpl::na >
class sentence {
public:
    typedef L   lexicon_type;
    typedef W   word_type;
    typedef WG  word_group_type;
    typedef S   syntax_type;

    typedef typename lexicon_type::word_reference       word_reference;
    typedef sentence_iterator< >  iterator;

    friend class iterator;

    typedef sentence< L, W, G, S > *    pointer;

    static const sentence< L, W, G, S > EMPTY;

    sentence();
    sentence( const sentence< L, W, G, S > & s );

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

}   // namespace sentences
}   // namespace grammar

#endif  // GRAMMAR_SENTENCE_DEF_HPP_
