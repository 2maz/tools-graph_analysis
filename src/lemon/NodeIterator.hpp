#ifndef GRAPH_ANALYSIS_LEMON_NODE_ITERATOR_HPP
#define GRAPH_ANALYSIS_LEMON_NODE_ITERATOR_HPP

#include "../VertexIterator.hpp"
#include <lemon/core.h>

namespace graph_analysis {
namespace lemon {

template<typename T>
class NodeIterator : public VertexIterator
{
public:
    NodeIterator(const T& graph)
        : mGraph(graph)
        , mNodeIt(graph.raw())
    {}

    bool next()
    {
        while( mNodeIt != ::lemon::INVALID)
        {
            Vertex::Ptr vertex = mGraph.mVertexMap[mNodeIt];
            ++mNodeIt;
            if(skip(vertex))
            {
                continue;
            }
            setNext(vertex);
            return true;
        }
        return false;
    }

protected:
    const T& mGraph;
    typename T::graph_t::NodeIt mNodeIt;

};

} // end namespace lemon
} // end namespace graph_analysis
#endif // GRAPH_ANALYSIS_LEMON_NODE_ITERATOR_HPP
