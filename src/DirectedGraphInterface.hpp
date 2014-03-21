#ifndef GRAPH_ANALYSIS_DIRECTED_GRAPH_INTERFACE_HPP
#define GRAPH_ANALYSIS_DIRECTED_GRAPH_INTERFACE_HPP

#include <graph_analysis/EdgeIterator.hpp>

namespace graph_analysis
{

class DirectedGraphInterface
{
    virtual EdgeIterator::Ptr getOutEdgeIterator(Vertex::Ptr vertex) = 0;
    virtual EdgeIterator::Ptr getInEdgeIterator(Vertex::Ptr vertex) = 0;
};

} // end graph_analysis
#endif // GRAPH_ANALYSIS_DIRECTED_GRAPH_INTERFACE_HPP
