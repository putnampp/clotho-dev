#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <cassert>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <boost/program_options.hpp>
#include <boost/algorithm/string/predicate.hpp>

namespace po=boost::program_options;
namespace algo=boost::algorithm;
using std::string;

typedef boost::property_tree::ptree json_tree_type;

const string HELP_K = "help";
const string INPUT_K = "input";

void join_performance_data( const std::vector< string > & files, boost::property_tree::ptree & p );
void to_canvasxpress_graph( const boost::property_tree::ptree & p );
void to_canvasxpress_graph2( const boost::property_tree::ptree & p );
void to_canvasxpress_graph3( const boost::property_tree::ptree & p );

void combine_sample_data( boost::property_tree::ptree::const_iterator first, boost::property_tree::ptree::const_iterator last, string child_key, boost::property_tree::ptree & res );

int parse_commandline( int argc, char ** argv, po::variables_map & vm );

int main( int argc, char ** argv ) {
    po::variables_map vm;

    unsigned int err = 0;
    if( (err = parse_commandline( argc, argv, vm )) != 0 ) {
        return err;
    }

    std::vector< string > inp = vm[ INPUT_K ].as< std::vector<string> >();
  
    json_tree_type raw_data, graph;

    join_performance_data( inp, raw_data );

    to_canvasxpress_graph3( raw_data );
    return 0;
}

int parse_commandline( int argc, char ** argv, po::variables_map & vm ) {
    po::options_description general("General");
    general.add_options()
        ((HELP_K + ",h").c_str(), "Print this" )
    ;

    po::options_description parameters("Parameters");
    parameters.add_options()
        ( (INPUT_K + ",i").c_str(), po::value< std::vector< string > >()->composing(), "JSON files")
    ;

    po::positional_options_description pos;
    pos.add( INPUT_K.c_str(), -1);

    po::options_description cmdline;
    cmdline.add(general).add(parameters);
    po::store( po::command_line_parser(argc, argv).options(cmdline).positional(pos).run(), vm );

    unsigned int err = 0;
    if( vm.count( HELP_K ) ) {
        std::cout << cmdline << std::endl;
        err = 1;
    }

    if( vm.count( INPUT_K ) == 0 ) {
        std::cerr << "No input files" << std::endl;
        err = 2;
    }

    return err;
}

void join_performance_data( const std::vector< string > & files, boost::property_tree::ptree & p ) {
    unsigned int file_idx = 0;
    for( std::vector< string >::const_iterator it = files.begin(); it != files.end(); ++it ) {
        if( algo::iends_with( (*it), ".json" )) {
            json_tree_type d;
            boost::property_tree::read_json( (*it), d );

            if( d.get_child_optional( "simulation.performance.data" ) == boost::none ) continue;

            std::ostringstream oss;
            oss << file_idx;

            p.put( oss.str() + ".filename", (*it) );
            p.put( oss.str() + ".mutation_rate", d.get<double>( "configuration.mutation.rate_per_region"));
            boost::property_tree::ptree::iterator first = d.get_child("simulation.performance.data").begin(), last = d.get_child("simulation.performance.data").end();
            while( first != last ) {
                p.add_child(oss.str() + "." + first->first, (first)->second );
                ++first;
            }
        }
        ++file_idx;
    }

    boost::property_tree::write_json( std::cerr, p );
}

void to_canvasxpress_graph( const boost::property_tree::ptree & p ) {
    typedef boost::property_tree::ptree tree_type;
    typedef tree_type::iterator tree_iterator;
    typedef tree_type::const_iterator tree_citerator;

    tree_type x_data, smps;
    tree_citerator first = p.begin();

    std::map< string, tree_type > data_sets;

    data_sets.insert( std::make_pair("generations", tree_type()) );
    data_sets.insert( std::make_pair("runtimes", tree_type()) );
    data_sets.insert( std::make_pair("table_size", tree_type()) );
    data_sets.insert( std::make_pair("alphabet_size", tree_type() ) );
    data_sets.insert( std::make_pair("sequence_count", tree_type()) );

    unsigned int nChildren = 0;
    while( first != p.end() ) {
        std::vector< string > to_remove;
        unsigned int max_gen = 0;
        for( std::map<string, tree_type>::iterator it = data_sets.begin(); it != data_sets.end(); ++it ) {

            if( first->second.get_child_optional( it->first ) == boost::none ) {
                to_remove.push_back( it->first );
            } else {
                tree_citerator v_it = first->second.get_child( it->first ).begin(), v_end = first->second.get_child( it->first ).end();

                if( it->first == "generations" ) {
                    unsigned int prev = max_gen;
                    while( v_it != v_end ) {
                        it->second.push_back( std::make_pair("", v_it->second ) );
                        prev = v_it->second.get< unsigned int >("");
                        ++v_it;
                    }
                    max_gen = prev;
                } else {
                    while( v_it != v_end ) {
                        it->second.push_back( std::make_pair("", v_it->second));
                        ++v_it;
                    }
                }
            }
        }
        for( std::vector< string >::iterator it = to_remove.begin(); it != to_remove.end(); ++it ) {
            data_sets.erase( (*it) );
        }

        if( max_gen > 0 ) {
            double mut_rate = first->second.get<double>("mutation_rate");
            tree_type m_node;
            m_node.put("", mut_rate);
            std::ostringstream oss;
            unsigned int i = 0;
            do {
                x_data.push_back( std::make_pair( "", m_node) );
                oss.str("");
                oss.clear();
                oss << first->first << "_" << i++;
                tree_type tmp;
                tmp.put( "", oss.str() );
                smps.push_back( std::make_pair("", tmp ) );
            } while (--max_gen );
        }
        
        ++nChildren;
        ++first;
    }

    tree_type y_data, vars, ring_data;
    unsigned int r_idx = 1;
    for( std::map< string, tree_type >::iterator it = data_sets.begin(); it != data_sets.end(); ++it ) {
        tree_type t;
        t.put("", it->first);
        vars.push_back( std::make_pair("", t ) );
        y_data.push_back( std::make_pair( "", it->second ) );

        tree_type r;
        r.put("", (int) ((it->first == "generations")? -1 : r_idx++));
        ring_data.push_back( std::make_pair("", r ) );
    }

    tree_type x, y, z, graph_opts, graph;
    x.add_child( "mutation_rate", x_data );

    y.add_child( "smps", smps );
    y.add_child( "vars", vars );
    y.add_child( "data", y_data);

    z.add_child( "Ring", ring_data );

    graph_opts.put( "graphType", "Circular" );
    graph_opts.put( "rAxis", "generations" );
    graph_opts.put( "segregateSamplesBy", "mutation_rate" );
    graph_opts.put( "segregateVariablesBy", "Ring" );

    graph.add_child( "x", x );
    graph.add_child( "y", y );
    graph.add_child( "z", z );
    graph.add_child( "graph_opts", graph_opts );

    boost::property_tree::write_json( std::cout, graph );
}

void to_canvasxpress_graph2( const boost::property_tree::ptree & p ) {
    typedef boost::property_tree::ptree tree_type;
    typedef tree_type::iterator tree_iterator;
    typedef tree_type::const_iterator tree_citerator;

    tree_type x_data, y_data, smps;
    tree_citerator first = p.begin();

    std::map< string, tree_type > data_sets;

    data_sets.insert( std::make_pair("generations", tree_type()) );
    data_sets.insert( std::make_pair("runtimes", tree_type()) );
    data_sets.insert( std::make_pair("table_size", tree_type()) );
    data_sets.insert( std::make_pair("alphabet_size", tree_type() ) );
    data_sets.insert( std::make_pair("sequence_count", tree_type()) );

    while( first != p.end() ) {
        for( std::map<string, tree_type>::iterator it = data_sets.begin(); it != data_sets.end(); ++it ) {

            if( first->second.get_child_optional( it->first ) != boost::none ) {
                it->second.push_back(std::make_pair( "", first->second.get_child( it->first ) ) );
            }
        }
        double mut_rate = first->second.get<double>("mutation_rate");
        tree_type m_node;
        m_node.put("", mut_rate);
        x_data.push_back( std::make_pair( "", m_node ) );

        std::ostringstream oss;
        oss << "s" << first->first;
        tree_type tmp;
        tmp.put("", oss.str() );
        smps.push_back( std::make_pair("", tmp ) );

        ++first;
    }

    tree_type vars, ring_data;
    unsigned int r_idx = 1;
    for( std::map< string, tree_type >::iterator it = data_sets.begin(); it != data_sets.end(); ++it ) {
        tree_type t;
        t.put("", (it->first));
        vars.push_back( std::make_pair("", t ) );
        y_data.push_back( std::make_pair( "", it->second ) );

        tree_type r;
        r.put("", (int) (( it->first == "generations")? -1 : r_idx++));
        ring_data.push_back( std::make_pair("", r ) );
    }

    tree_type x, y, z, graph_opts, graph;
    x.add_child( "mutation_rate", x_data );

    y.add_child( "smps", smps );
    y.add_child( "vars", vars );
    y.add_child( "data", y_data);

    z.add_child( "Ring", ring_data );

    graph_opts.put( "graphType", "Circular" );
    graph_opts.put( "rAxis", "generations" );
    graph_opts.put( "segregateSamplesBy", "mutation_rate" );
    graph_opts.put( "segregateVariablesBy", "Ring" );

    graph.add_child( "x", x );
    graph.add_child( "y", y );
    graph.add_child( "z", z );
    graph.add_child( "graph_opts", graph_opts );

    boost::property_tree::write_json( std::cout, graph );
}

void to_canvasxpress_graph3( const boost::property_tree::ptree & p ) {
    typedef boost::property_tree::ptree tree_type;
    typedef tree_type::iterator tree_iterator;
    typedef tree_type::const_iterator tree_citerator;

    tree_type graphs;
    std::vector< string > g_keys;
    g_keys.push_back( "runtimes" );
    g_keys.push_back( "table_size" );
    g_keys.push_back( "alphabet_size" );
    g_keys.push_back( "sequence_count" );
    for( std::vector< string >::iterator it = g_keys.begin(); it != g_keys.end(); ++it) {
        tree_type tmp;
        combine_sample_data( p.begin(), p.end(), (*it), tmp );
        graphs.add_child( "graphs." + (*it), tmp );
    }

    boost::property_tree::write_json( std::cout, graphs );
}

void combine_sample_data( boost::property_tree::ptree::const_iterator first, boost::property_tree::ptree::const_iterator last, string child_key, boost::property_tree::ptree & res ) {

    boost::property_tree::ptree vars, smps, data;

    bool gen_found = false;
    while( first != last ) {
        if( first->second.get_child_optional( child_key ) != boost::none ) {
            double mut_rate = first->second.get<double>("mutation_rate");
            boost::property_tree::ptree m_node;
            m_node.put("", mut_rate);

            vars.push_back( std::make_pair( "", m_node ) );
            if( !gen_found ) {
                smps = first->second.get_child( "generations" );
                gen_found = true;
/*            } else {
              // check not working ATM. assuming that once found all other data sets
              // have same number of generations
                // check that sampling sizes are equivalent for each variable
                boost::property_tree::ptree::iterator smps_it = smps.get_child("").begin(), smps_end = smps.get_child("").end();

                boost::property_tree::ptree::const_iterator c_it = first->second.get_child("generations").begin(), c_end = first->second.get_child("generations").end();

                while( smps_it != smps_end && c_it != c_end ) {
                    assert( smps_it->second == c_it->second);
                    ++smps_it; ++c_it;
                }
                assert( smps_it == smps_end && c_it == c_end );*/
            }
            data.push_back( std::make_pair( "", first->second.get_child( child_key ) ) );
        }
        ++first;
    }

    boost::property_tree::ptree y;

    y.add_child( "smps", smps );
    y.add_child( "data", data );
    y.add_child( "vars", vars );

    boost::property_tree::ptree graph_opts;

    graph_opts.put( "graphType", "Line" );
    graph_opts.put( "graphOrientation", "vertical");
    graph_opts.put( "title", child_key );
    graph_opts.put( "xAxisTitle", child_key );
    graph_opts.put( "yAxisTitle", "Generations" );
    
    res.add_child( "y", y );
    res.add_child( "graph_opts", graph_opts );
}
