#ifndef GRAPH_ANALYSIS_GRAPH_VIEW_HPP
#define GRAPH_ANALYSIS_GRAPH_VIEW_HPP

#include <graph_analysis/Filter.hpp>

namespace graph_analysis
{

template<typename GraphType, typename VertexPropertyType, typename EdgePropertyType>
class GraphView
{
public:
  //  GraphView();

    typedef typename Filter<VertexPropertyType>::Ptr VertexFilterType;
    typedef typename Filter<EdgePropertyType>::Ptr EdgeFilterType;

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