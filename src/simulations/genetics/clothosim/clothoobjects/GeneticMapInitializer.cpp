/*******************************************************************************
 * Copyright (c) 2013, Patrick P. Putnam (putnampp@gmail.com)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are those
 * of the authors and should not be interpreted as representing official policies,
 * either expressed or implied, of the FreeBSD Project.
 ******************************************************************************/

#include "GeneticMapInitializer.h"
#include "Environment2.h"

#include "Locus.h"
#include "DefaultTrait.h"
#include "DefaultLocusGenotyper.h"

#include <iostream>

using std::cout;
using std::endl;

const string ENVIRONMENT_K = "environment";
const string MAX_LOCI_K = "max_loci";
const string LOCI_K = "loci";
const string LOCUS_K = "locus";
const string GENOTYPER_K = "genotyper";
const string ALLELIC_EFFECT_K = "allelic_effect";
const string TRAITS_K = "traits";

template<>
class ClothoInitializerCreator< GeneticMapInitializer, YAML::Node > : public InitializerCreator< YAML::Node > {
public:
    ClothoInitializerCreator( const char * name ) : m_name( name ) { /*, m_T(), m_rng(NULL)*/
        ClothoInitializerManager< YAML::Node >::getInstance()->registerInitializer( this );
    }

    const string & name() {
        return m_name;
    }

    void initialize( const YAML::Node & n, shared_ptr< vector< SimulationObject * > > objs ) {
        if( !objs ) return;

        string env_name = "";
        if( n[ ENVIRONMENT_K ] ) {
            env_name = n[ ENVIRONMENT_K ].as<string>();
        } else {
            abort();
        }

        // find the Environment from which the individuals are being initialized
        Environment2 * env = NULL;
        for( vector< SimulationObject * >::reverse_iterator it = objs->rbegin(); it != objs->rend(); it++ ) {
            env = dynamic_cast< Environment2 * >( (*it) );
            if( env ) {
                if( env->getName() == env_name ) {
                    break;
                } else {
                    env = NULL;
                }
            }
        }

        if( !env )  abort();    // could not find the environment

        if( n[ TRAITS_K ] ) {
            buildTraits( n[TRAITS_K] );
        }

        if( n[ LOCI_K ] ) {
            buildLoci( n[LOCI_K] );
        }

    }

    virtual ~ClothoInitializerCreator() {
        while( !m_traits.empty() ) {
            TraitPtr tmp = m_traits.back();
            m_traits.pop_back();
            tmp.reset();
        }

        while( !m_loci.empty() ) {
            LocusGenotyper * tmp = m_loci.back();
            m_loci.pop_back();
            delete tmp;
        }
    }
protected:
    void buildTraits( const YAML::Node & n ) {
        if( n.IsSequence() ) {
            for( YAML::Node::const_iterator it = n.begin(); it != n.end(); it++ ) {
                if( it->IsScalar() ) {
                    string name = it->as<string>();
                    TraitPtr t( new DefaultTrait(name) );
                    m_traits.push_back( t );
                }
            }
        }
    }

    void buildLoci( const YAML::Node & n ) {
        if( n.IsSequence() ) {
            cout << "Sequence Node" << endl;
            for( YAML::Node::const_iterator it = n.begin(); it != n.end(); it++ ) {
                YAML::Node tmp = (*it);
                if( tmp[ LOCUS_K ] ) {
                    LocusPtr lp = buildLocus( tmp[LOCUS_K ] );

                    LocusGenotyper * lg = NULL;
                    if( lp && tmp[ GENOTYPER_K ] ) {
                        lg = buildLocusGenotyper( tmp[ GENOTYPER_K ], lp );
                    } else {
                        lg = new DefaultLocusGenotyper( lp );
                    }

                    m_loci.push_back( lg ); // Initializer performs cleanup
                }
            }
        } else if( n.IsSequence() ) {
        }
    }

    LocusPtr buildLocus( const YAML::Node & n ) {
        if( !n.IsSequence() ) {
            cout << "Expected Sequence" << endl;
            return LocusPtr();
        }
        YAML::Node::const_iterator it = n.begin();
        if( it == n.end() ) return NULL;

        chromid_t id = (chromid_t)0;
        if( it->IsScalar() ) {
            id = it->as<chromid_t>();
        }

        it++;
        if( it == n.end() ) return NULL;

        pos_t start = (pos_t)0;
        if( it->IsScalar()) {
            start = it->as< pos_t >();
        }

        it++;
        if( it == n.end() ) return NULL;

        pos_t end = start + 1;
        if( it->IsScalar() ) {
            end = it->as< pos_t >();
        }

        it++;
        if( it == n.end() ) return NULL;

        allele_t nAlleles = (allele_t)0;
        if( it->IsScalar() ) {
            nAlleles = it->as< allele_t >();
        }

        LocusPtr lp( new Locus( id, start, end, nAlleles ) );

        return lp;
    }

    LocusGenotyper * buildLocusGenotyper( const YAML::Node & n, LocusPtr lp ) {

    }

    const string m_name;

    Traits  m_traits;
    vector< LocusGenotyper * >    m_loci;
};

DEFINE_REGISTERED_INITIALIZER( GeneticMapInitializer, YAML::Node )

