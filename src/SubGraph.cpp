#include "SubGraph.hpp"

#include <boost/bind.hpp>
#include <graph_analysis/BaseGraph.hpp>
#include <graph_analysis/filters/EdgeContextFilter.hpp>

namespace graph_analysis {

SubGraph::SubGraph(BaseGraph* baseGraph)
    : mpBaseGraph(baseGraph)
{}

/**
 * Apply filters to this subgraph
 */
void SubGraph::applyFilters(Filter<Vertex::Ptr>::Ptr vertexFilter, Filter<Edge::Ptr>::Ptr edgeFilter)
{
    if(edgeFilter)
    {
        filters::EdgeContextFilter::Ptr contextFilter = boost::dynamic_pointer_cast<filters::EdgeContextFilter>(edgeFilter);

        EdgeIterator::Ptr edgeIterator = getBaseGraph()->getEdgeIterator();

        while(edgeIterator->next())
        {
            Edge::Ptr edge = edgeIterator->current();
            if( edgeFilter->permits(edge) )
            {
                // A context filter should apply to source / target nodes -- no need to filter this edge specifically then
                if(contextFilter)
                {
                    bool filterTarget = contextFilter->permitsTarget(edge);
                    bool filterSource = contextFilter->permitsSource(edge);

                    if(filterSource && filterTarget)
                    {
                        enable(edge);
                    }
                }
            } else {
                disable(edge);
            }
        }
    }

    if(vertexFilter)
    {
        VertexIterator::Ptr vertexIterator = getBaseGraph()->getVertexIterator();
        while(vertexIterator->next())
        {
            Vertex::Ptr vertex = vertexIterator->current();
            if( vertexFilter->permits(vertex) )
            {
                enable(vertex);
            } else {
                disable(vertex);
            }
        }
    }
}

BaseGraph::Ptr SubGraph::toBaseGraph()
{
    BaseGraph::Ptr graph = mpBaseGraph->cleanCopy();

    VertexIterator::Ptr vertexIterator = mpBaseGraph->getVertexIterator();

    while(vertexIterator->next())
    {
        Vertex::Ptr vertex = vertexIterator->current();
        if(enabled(vertex))
        {
            graph->addVertex(vertex);
        }
    }

    EdgeIterator::Ptr edgeIterator = mpBaseGraph->getEdgeIterator();
    while(edgeIterator->next())
    {
        Edge::Ptr edge = edgeIterator->current();
        if(enabled(edge))
        {
            graph->addEdge(edge);
        }
    }
    return graph;
}

BaseGraph* SubGraph::getBaseGraph()
{
    return mpBaseGraph;
}

VertexIterator::Ptr SubGraph::getVertexIterator()
{
    VertexIterator::Ptr vertexIt = getBaseGraph()->getVertexIterator();
    // Need to explicitely cast skip function to disambiguate (
    // disable(Vertex::Ptr) vs. disable(Edge::Ptr)
    VertexIterator::SkipFunction skipFunction( boost::bind(static_cast<bool (SubGraph::*)(Vertex::Ptr) const>(&SubGraph::disabled), this,_1) );
    vertexIt->setSkipFunction(skipFunction);
    return vertexIt;
}


EdgeIterator::Ptr SubGraph::getEdgeIterator()
{
    EdgeIterator::Ptr edgeIt = getBaseGraph()->getEdgeIterator();
    // Need to explicitely cast skip function to disambiguate (
    // disable(Vertex::Ptr) vs. disable(Edge::Ptr)
    EdgeIterator::SkipFunction skipFunction( boost::bind(static_cast<bool (SubGraph::*)(Edge::Ptr) const>(&SubGraph::disabled), this,_1) );
    edgeIt->setSkipFunction(skipFunction);
    return edgeIt;
}

} // end namespace graph_analysis
