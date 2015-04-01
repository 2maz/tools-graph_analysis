#ifndef GRAPH_ANALYSIS_SUB_GRAPH_HPP
#define GRAPH_ANALYSIS_SUB_GRAPH_HPP

#include <graph_analysis/Vertex.hpp>
#include <graph_analysis/Edge.hpp>
#include <graph_analysis/Filter.hpp>

namespace graph_analysis {

class BaseGraph;

class SubGraph
{
    BaseGraph* mpBaseGraph;

public:
    SubGraph(BaseGraph* graph);

    /**
     * Enable the given vertex
     * \param vertex Vertex that is part of the underlying main graph and should
     * also be part of the subgraph
     */
    virtual void enable(Vertex::Ptr vertex) = 0;

    /**
     * Disable the given vertex
     * \param vertex Vertex that is part of the underlying main graph and should
     * not(!) be part of the subgraph
     */
    virtual void disable(Vertex::Ptr vertex) = 0;

    /**
     * Enable the given edge
     * \param edge Edge that is part of the underlying main graph and should
     * be part of the subgraph
     */
    virtual void enable(Edge::Ptr edge) = 0;

    /**
     * Disable the given edge
     * \param edge Edge that is part of the underlying main graph and should
     * not be part of the subgraph
     */
    virtual void disable(Edge::Ptr edge) = 0;

    /**
     * Test if a vertex is enabled
     * \return True if vertex is enabled, false otherwise
     */
    virtual bool enabled(Vertex::Ptr vertex) const = 0;

    /**
     * Test if an edge is enabled
     * \return True if edge is enabled, false otherwise
     */
    virtual bool enabled(Edge::Ptr vertex) const = 0;

    /**
     * Apply filters to this subgraph
     */
    void applyFilters(Filter<Vertex::Ptr>::Ptr vertexFilter, Filter<Edge::Ptr>::Ptr edgeFilter);

protected:
    BaseGraph* getBaseGraph();
};

} // end namespace graph_analysis
#endif // GRAPH_ANALYSIS_SUB_GRAPH_HPP
