// Original source code from:
// http://en.wikipedia.org/wiki/Splay_tree
//
//
#include <functional>
#include <ostream>
#include <string>

#ifndef SPLAY_TREE
#define SPLAY_TREE

using std::string;

template< typename T, typename Comp = std::less< T > >
class splay_tree {
private:
    Comp comp;
    unsigned long p_size;

public:
    struct node {
        node *left, *right;
        node *parent;
        T key;
        node( const T& init = T( ) ) : left( 0 ), right( 0 ), parent( 0 ), key( init ) { }
    } *root;

private:
    void left_rotate( node *x ) {
        node *y = x->right;
        x->right = y->left;
        if( y->left ) y->left->parent = x;
        y->parent = x->parent;
        if( !x->parent ) root = y;
        else if( x == x->parent->left ) x->parent->left = y;
        else x->parent->right = y;
        y->left = x;
        x->parent = y;
    }

    void right_rotate( node *x ) {
        node *y = x->left;
        x->left = y->right;
        if( y->right ) y->right->parent = x;
        y->parent = x->parent;
        if( !x->parent ) root = y;
        else if( x == x->parent->left ) x->parent->left = y;
        else x->parent->right = y;
        y->right = x;
        x->parent = y;
    }

    void splay( node *x ) {
        while( x->parent ) {
            if( !x->parent->parent ) {
                if( x->parent->left == x ) right_rotate( x->parent );
                else left_rotate( x->parent );
            } else if( x->parent->left == x && x->parent->parent->left == x->parent ) {
                right_rotate( x->parent->parent );
                right_rotate( x->parent );
            } else if( x->parent->right == x && x->parent->parent->right == x->parent ) {
                left_rotate( x->parent->parent );
                left_rotate( x->parent );
            } else if( x->parent->left == x && x->parent->parent->right == x->parent ) {
                right_rotate( x->parent );
                left_rotate( x->parent );
            } else {
                left_rotate( x->parent );
                right_rotate( x->parent );
            }
        }
    }

    void replace( node *u, node *v ) {
        if( !u->parent ) root = v;
        else if( u == u->parent->left ) u->parent->left = v;
        else u->parent->right = v;
        if( v ) v->parent = u->parent;
    }

    node* subtree_minimum( node *u ) {
        while( u->left ) u = u->left;
        return u;
    }

    node* subtree_maximum( node *u ) {
        while( u->right ) u = u->right;
        return u;
    }

public:
    splay_tree( ) : p_size(0), root( 0 ) { }

    node * insert( const T &key ) {
        node *z = root;
        node *p = 0;

        while( z ) {
            p = z;
            if( comp( z->key, key ) ) z = z->right;
            else z = z->left;
        }

        z = new node( key );
        z->parent = p;

        if( !p )    root = z;
        else if( comp( p->key, z->key ) ) p->right = z;
        else p->left = z;

        splay( z );
        p_size++;

        return z;
    }

    node* find( const T &key ) {
        node *z = root;
        while( z ) {
            if( comp( z->key, key ) ) z = z->right;
            else if( comp( key, z->key ) ) z = z->left;
            else return z;
        }
        return 0;
    }

    node * find( bool (*pred)( const T & ) ) {
        node * r = traverse( root, pred );
        if( r != NULL )        splay(r);

        return r;
    }

    node * traverse( node * r, bool (*pred)( const T & ) ) {
        if( r == NULL ) return NULL;

        if( (*pred)(r->key ) ) return r;

        node * res = NULL;
        if( r->left ) res = traverse( r->left, pred );

        if( res == NULL && r->right != NULL ) {
            res = traverse( r->right, pred );
        }

        return res;
    }

    void erase( const T &key ) {
        node *z = find( key );
        if( !z ) return;

        erase(z);
    }

    void erase( node * z ) {
        splay( z );

        if( !z->left ) replace( z, z->right );
        else if( !z->right ) replace( z, z->left );
        else {
            node *y = subtree_minimum( z->right );
            if( y->parent != z ) {
                replace( y, y->right );
                y->right = z->right;
                y->right->parent = y;
            }
            replace( z, y );
            y->left = z->left;
            y->left->parent = y;
        }

        delete z;
        p_size--;
    }

    void dump( std::ostream & out ) {
        string pad = "";
        printNode( out, root, pad );
    }

    void printNode( std::ostream & out, node * r, string pad ) {
        if( r == NULL ) return;

        if( r->left != NULL ) printNode(out, r->left, pad + "\t");
 
        out << pad << r << ":" << r->key << "\n";

        if( r->right != NULL ) printNode(out, r->right, pad + "\t");
    }

    const T& minimum( ) {
        return subtree_minimum( root )->key;
    }
    const T& maximum( ) {
        return subtree_maximum( root )->key;
    }

    node * getRoot() {
        return root;
    }

    bool empty( ) const {
        return root == 0;
    }
    unsigned long size( ) const {
        return p_size;
    }
};

#endif // SPLAY_TREE
