#ifndef ALPHABET_DEF_HPP_
#define ALPHABET_DEF_HPP_

#include <vector>

namespace grammar {
namespace alphabets {

template < typename Symbol, typename SymbolAllocator, typename SymbolKey, typename Database = std::vector< Symbol, SymbolAllocator >, typename SR, typename TR >
class alphabet {
public:
    typedef alphabet< Symbol, SymbolAllocator, SymbolKey, Database, SR, TR > self_type;
    typedef Symbol          symbol_type;
    typedef SymbolAllocator symbol_allocator_type;
    typedef SymbolKey       symbol_key_type;
    typedef Database        database_type;
    typedef SR          symbol_reference;
    typedef TR          trait_type;

    typedef typename database_type::iterator iterator;

    typedef std::shared_ptr< self_type > pointer;

    alphabet();
    alphabet( const self_type & ot );

    symbol_reference    addSymbol( const symbol_type & sym );
    void                removeSymbol( symbol_reference sym );

    symbol_reference    findSymbol( const symbol_key_type & k );

    symbol_reference    operator[]( const symbol_type & sym );
    symbol_type         operator[]( symbol_reference ref );

    iterator begin( );
    iterator end( );

    virtual ~alphabet();

protected:
};

}   // namespace alphabet
}   // namespace grammar

#endif  // ALPHABET_DEF_HPP_
