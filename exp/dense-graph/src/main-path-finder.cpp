//
//=======================================================================
// Copyright (c) 2004 Kristopher Beevers
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
//


#include <boost/graph/astar_search.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/random.hpp>
#include <boost/random.hpp>
#include <boost/graph/graphviz.hpp>
#include <sys/time.h>
#include <vector>
#include <list>
#include <iostream>
#include <fstream>
#include <math.h>    // for sqrt

#include <boost/progress.hpp>
#include "graph.hpp"
#include "func.hpp"
#include <cassert>


using namespace boost;
using namespace std;


// auxiliary types
struct location {
    float y, x; // lat, long
};
typedef float cost;





// euclidean distance heuristic
template <class Graph, class CostType, class LocMap>
class distance_heuristic : public astar_heuristic<Graph, CostType> {
public:
            typedef typename graph_traits<Graph>::vertex_descriptor Vertex;
            distance_heuristic(LocMap l, Vertex goal)
: m_location(l), m_goal(goal) {}
            CostType operator()(Vertex u) {
                CostType dx = m_location[m_goal].x - m_location[u].x;
                CostType dy = m_location[m_goal].y - m_location[u].y;
                return ::sqrt(dx * dx + dy * dy);
            }
private:
            LocMap m_location;
            Vertex m_goal;
        };


struct found_goal {}; // exception for termination

// visitor that terminates when we find the goal
template <class Vertex>
class astar_goal_visitor : public boost::default_astar_visitor {
public:
astar_goal_visitor(Vertex goal) : m_goal(goal) {}
    template <class Graph>
    void examine_vertex(Vertex u, Graph& g) {
        if (u == m_goal)
            throw found_goal();
    }
private:
    Vertex m_goal;
};

// specify some types
typedef adjacency_list<listS, vecS, undirectedS, no_property,
  property<edge_weight_t, cost> > mygraph_t;
typedef property_map<mygraph_t, edge_weight_t>::type WeightMap;
typedef mygraph_t::vertex_descriptor myVertex;
typedef mygraph_t::edge_descriptor edge_descriptor;
typedef mygraph_t::vertex_iterator vertex_iterator;
typedef std::pair<int, int> edge;



void AStarSearch( mygraph_t g, mt19937 gen, location* locations, ostream &out );


int main(int argc, char **argv) {
    boost::progress_timer timerMain;



    Large arraySize = GRAPH_THETA * GRAPH_HEIGHT * GRAPH_WIDTH * 3;
    std::cout << "Edge Array Size: " << arraySize << "." << std::endl;
    Large arrayByteSize = sizeof( EdgeArray );
    std::cout << "Edge Array Byte Size: " << arrayByteSize << " bytes." << std::endl;
    cout << "CHECKPOINT: " << __LINE__ << std::endl;
    Edge* edgesArray = (Edge*)new EdgeArray;

    std::auto_ptr< Edge > edgesBetweenThatArray( (Edge*)(edgesArray) );
    cout << "CHECKPOINT: " << __LINE__ << std::endl;

    fillEdges( edgesArray );
    cout << "CHECKPOINT: " << __LINE__ << std::endl;
    assert( edgesArray->first == 0 );
    assert( edgesArray->second == 1 );
    assert( (edgesArray + 1)->first == 1 );
    assert( (edgesArray + 1)->second == 2 );
    assert( getDenseGraphLookup( 0, 1, 0 ) == GRAPH_WIDTH );

    assert( (edgesArray + getEdgeLookup( 0, 0, 1, 0 ) )->first == getDenseGraphLookup( 0, 1, 0 ) );
    assert( (edgesArray + getEdgeLookup( 0, 0, 1, 0 ) )->second == getDenseGraphLookup( 1, 1, 0 ) );

    assert( (edgesArray + getEdgeLookup( 0, 1, 1, 0 ) )->first == getDenseGraphLookup( 1, 1, 0 ) );
    assert( (edgesArray + getEdgeLookup( 0, 1, 1, 0 ) )->second == getDenseGraphLookup( 2, 1, 0 ) );

    assert( (edgesArray + getEdgeLookup( 0, 0, 0, 1 ) )->first == getDenseGraphLookup( 0, 0, 1 ) );
    assert( (edgesArray + getEdgeLookup( 0, 0, 0, 1 ) )->second == getDenseGraphLookup( 1, 0, 1 ) );

    assert( (edgesArray + getEdgeLookup( 0, 1, 0, 1 ) )->first == getDenseGraphLookup( 1, 0, 1 ) );
    assert( (edgesArray + getEdgeLookup( 0, 1, 0, 1 ) )->second == getDenseGraphLookup( 2, 0, 1 ) );

    assert( (edgesArray + getEdgeLookup( 1, 0, 0, 0 ) )->first == getDenseGraphLookup( 0, 0, 0 ) );
    assert( (edgesArray + getEdgeLookup( 1, 0, 0, 0 ) )->second == getDenseGraphLookup( 0, 1, 0 ) );

    assert( (edgesArray + getEdgeLookup( 1, 1, 0, 0 ) )->first == getDenseGraphLookup( 1, 0, 0 ) );
    assert( (edgesArray + getEdgeLookup( 1, 1, 0, 0 ) )->second == getDenseGraphLookup( 1, 1, 0 ) );

    location* locations = (location*)new location[GRAPH_THETA][GRAPH_HEIGHT][GRAPH_WIDTH];
    std::auto_ptr< location > loc( locations );
    cout << "CHECKPOINT: " << __LINE__ << std::endl;
    {
        boost::progress_timer timerLocations;
        for ( int t = 0; t < GRAPH_THETA; ++t ) {
            for ( int r = 0; r < GRAPH_HEIGHT; ++r ) {
                for ( int c = 0; c < GRAPH_WIDTH; ++c ) {
                    *( locations + getDenseGraphLookup( c, r, t ) ) = { c, r };
                }
            }
        }
        std::cout << "In file: " << __FILE__ << std::endl;
        std::cout << "Execution Time for filling locations: ";
    }
    cout << "CHECKPOINT: " << __LINE__ << std::endl;


    // create graph
    mygraph_t g(GRAPH_THETA * GRAPH_HEIGHT * GRAPH_WIDTH);
    WeightMap weightmap = get(edge_weight, g);
    cout << "CHECKPOINT: " << __LINE__ << std::endl;
    {
        boost::progress_timer timerWM;
        for ( int type = 0; type < 3; ++type ) {
            boost::progress_timer timerTheta;
            for ( int t = 0; t < GRAPH_THETA - ( (type == 2)?1:0 ); ++t ) {
                for ( int r = 0; r < GRAPH_HEIGHT - ( (type == 1)?1:0 ); ++r ) {
                    for ( int c = 0; c < GRAPH_WIDTH - ( (type == 0)?1:0 ); ++c ) {
                        Edge* current = edgesArray + getEdgeLookup( type, c, r, t );
                        edge_descriptor e;
                        bool inserted;
//            cout << "current->first: " << current->first << std::endl;
//            cout << "current->second: " << current->second << std::endl;
                        tie(e, inserted) = add_edge( current->first,
                                                     current->second, g);
                        // Treat all weights the same for now.
                        weightmap[e] = 1;
                    }
                }
            }
            std::cout << "In file: " << __FILE__ << std::endl;
            std::cout << "Execution Time for filling weightmap - THETA LOOP: ";
        }
        std::cout << "In file: " << __FILE__ << std::endl;
        std::cout << "Execution Time for filling weightmap: ";
    }
    cout << "CHECKPOINT: " << __LINE__ << std::endl;
    // pick random start/goal
    mt19937 gen(time(0));
    ofstream dotfile;
    dotfile.open("tests.csv");
    #define MAX_TESTS 100
    for( int i = 0; i < MAX_TESTS; ++i ) {
      AStarSearch( g, gen, locations, dotfile );
    }

    std::cout << "In file: " << __FILE__ << std::endl;
    std::cout << "Execution Time for \"" << __FUNCTION__ << "\": ";
    return 0;

}
#include <boost/timer.hpp>
void AStarSearch( mygraph_t g, mt19937 , location* locations, ostream &out ) {
    mt19937 gen(time(0));
    myVertex start = random_vertex(g, gen);
    myVertex goal = random_vertex(g, gen);
    cout << "CHECKPOINT: " << __LINE__ << std::endl;

    cout << "Start vertex: " << start << endl;
    cout << "Goal vertex: " << goal << endl;


    vector<mygraph_t::vertex_descriptor> p(num_vertices(g));
    vector<cost> d(num_vertices(g));
    cout << "CH ECKPOINT: " << __LINE__ << std::endl;
    bool found = false;
    {
        boost::progress_timer timerAStar();
        boost::timer timerAStar2;
        try {

            // call astar named parameter interface
            astar_search
            (g, start,
             distance_heuristic<mygraph_t, cost, location*>
             (locations, goal),
             predecessor_map(&p[0]).distance_map(&d[0]).
             visitor(astar_goal_visitor<myVertex>(goal)));
            cout << "CHECKPOINT: " << __LINE__ << std::endl;



        } catch (found_goal fg) { // found a path to the goal
            found = true;
            list<myVertex> shortest_path;
            cout << "CHECKPOINT: " << __LINE__ << std::endl;
            for (myVertex v = goal;; v = p[v]) {
                shortest_path.push_front(v);
                if (p[v] == v)
                    break;
            }
            cout << "CHECKPOINT: " << __LINE__ << std::endl;
            cout << "Shortest path from " << start << " to "
            << goal << ": ";
            list<myVertex>::iterator spi = shortest_path.begin();
            cout << start;
            for (++spi; spi != shortest_path.end(); ++spi)
                cout << " -> " << *spi;
            cout << endl << "Total travel time: " << d[goal] << endl;
        }
        //double temp = timerAStar2.elapsed();
        out << d[goal] << "," << (double)( timerAStar2.elapsed() );
        std::cout << "In file: " << __FILE__ << std::endl;
        std::cout << "Execution Time for A* search: ";
    }
    out << std::endl;
    if ( !found ) {
        cout << "Didn't find a path from " << start << "to"
          << goal << "!" << endl;
    }
}
