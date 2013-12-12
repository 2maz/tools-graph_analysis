#ifndef GRAPH_ANALYSIS_GRAPH_VIEW_HPP
#define GRAPH_ANALYSIS_GRAPH_VIEW_HPP

#include <graph_analysis/Filter.hpp>

namespace graph_analysis
{

typedef typename Filter<Vertex::Ptr>::Ptr VertexFilterType;
typedef typename Filter<Edge::Ptr>::Ptr EdgeFilterType;

template<typename GraphType>
class GraphView
{
public:
    /**
     * Add a vertex filter
     */
    void setVertexFilter(VertexFilterType filter)
    {
        if(filter)
        {
            mVertexFilter = filter;
        } else {
            throw std::runtime_error("GraphView: setVertexFilter called with null pointer");
        }
    }

    /**
     * Add an edge filter
     */
    void setEdgeFilter(EdgeFilterType filter)
    {
        if(filter)
        {
            mEdgeFilter = filter;
        } else {
            throw std::runtime_error("GraphView: setEdgeFilter called with null pointer");
        }
    }

    /**
     * Apply the filter to the graph
     */
    typename GraphType::SubGraph apply(GraphType& graph)
    {
        return graph.applyFilters(mVertexFilter, mEdgeFilter);
    }

private:
    VertexFilterType mVertexFilter;
    EdgeFilterType mEdgeFilter;
};

} // end namespace graph_analysis
#endif // GRAPH_ANALYSIS_GRAPH_VIEW_HPP
