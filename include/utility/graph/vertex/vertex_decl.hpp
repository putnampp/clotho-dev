#ifndef VERTEX_DECL_HPP_
#define VERTEX_DECL_HPP_

namespace utility {
namespace graph {
namespace vertices {

template < class Properties,
           class InNeighbor,
           class OutNeighbor,
           class InNeighborhood,
           class OutNeighborhood >
class vertex {
public:
    typedef Properties          properties_type;
    typedef InNeighbor          in_neighbor_type;
    typedef InNeighborhood      in_neighborhood_type;
    typedef OutNeighbor         out_neighbor_type;
    typedef OutNeighborhood     out_neighborhood_type;

    typedef out_neighborhood_type::iterator out_iterator;
    typedef in_neighborhood_type::iterator  in_iterator;

    typedef std::size_t         size_type;

    void        addInNeighbor( const in_neighbor_type & );
    void        removeInNeighbor( const in_neighbor_type & );

    void        addOutNeighbor( const out_neighbor_type & );
    void        removeOutNeighbor( const out_neighbor_type & );

    size_type   in_degree() const;
    size_type   out_degree() const;

    out_iterator    out_begin();
    out_iterator    out_end();

    in_iterator     in_begin();
    in_iterator     in_end();

    virtual ~vertex();
};

}   // namespace vertices
}   // namespace graph
}   // namespace utility

#endif  // VERTEX_DECL_HPP_
