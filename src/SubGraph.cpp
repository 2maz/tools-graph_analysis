#include "SubGraph.hpp"

#include <boost/bind.hpp>
#include <graph_analysis/BaseGraph.hpp>
#include <graph_analysis/filters/EdgeContextFilter.hpp>
#include <graph_analysis/filters/CommonFilters.hpp>

namespace graph_analysis {

SubGraph::SubGraph(BaseGraph::Ptr baseGraph)
    : mpBaseGraph(baseGraph)
{}

/**
 * Apply filters to this subgraph, pass filters::Filter<Vertex::Ptr>::Null() or
 * filters::Filter<Edge::Ptr>::Null() to skip filter for vertices or nodes
 */
void SubGraph::applyFilters(Filter<Vertex::Ptr>::Ptr vertexFilter, Filter<Edge::Ptr>::Ptr edgeFilter)
{
    if(edgeFilter)
    {
        filters::EdgeContextFilter::Ptr contextFilter = dynamic_pointer_cast<filters::EdgeContextFilter>(edgeFilter);

        EdgeIterator::Ptr edgeIterator = getBaseGraph()->getEdgeIterator();

        while(edgeIterator->next())
        {
            Edge::Ptr edge = edgeIterator->current();
            // By default we enable
            enable(edge);

            if( edgeFilter->matches(edge) )
            {
                // A context filter should apply to source / target nodes -- no need to filter this edge specifically then
                if(contextFilter)
                {
                    bool filterTarget = contextFilter->matchesTarget(edge);
                    bool filterSource = contextFilter->matchesSource(edge);

                    if(filterSource && filterTarget)
                    {
                        disable(edge);
                    }
                }
            }
        }
    }

    if(vertexFilter)
    {
        VertexIterator::Ptr vertexIterator = getBaseGraph()->getVertexIterator();
        while(vertexIterator->next())
        {
            Vertex::Ptr vertex = vertexIterator->current();
            if( vertexFilter->matches(vertex) )
            {
                disable(vertex);
            } else {
                enable(vertex);
            }
        }
    }
}

BaseGraph::Ptr SubGraph::toBaseGraph()
{
    BaseGraph::Ptr graph = mpBaseGraph->newInstance();

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

BaseGraph::Ptr SubGraph::getBaseGraph() const
{
    return mpBaseGraph;
}

void SubGraph::enableAllVertices()
{
    Filter<Vertex::Ptr>::Ptr vertexFilter(new filters::PermitAll< Vertex::Ptr >() );
    applyFilters(vertexFilter, Filter<Edge::Ptr>::Null());
}

void SubGraph::enableAllEdges()
{
    Filter<Edge::Ptr>::Ptr edgeFilter(new filters::PermitAll< Edge::Ptr >() );
    applyFilters(Filter<Vertex::Ptr>::Null(), edgeFilter);
}

void SubGraph::disableAllVertices()
{
    Filter<Vertex::Ptr>::Ptr vertexFilter(new filters::DenyAll< Vertex::Ptr >() );
    applyFilters(vertexFilter, Filter<Edge::Ptr>::Null());
}

void SubGraph::disableAllEdges()
{
    Filter<Edge::Ptr>::Ptr edgeFilter(new filters::DenyAll< Edge::Ptr >() );
    applyFilters(Filter<Vertex::Ptr>::Null(), edgeFilter);
}

VertexIterator::Ptr SubGraph::getVertexIterator() const
{
    VertexIterator::Ptr vertexIt = getBaseGraph()->getVertexIterator();
    // Need to explicitely cast skip function to disambiguate (
    // disable(Vertex::Ptr) vs. disable(Edge::Ptr)
    VertexIterator::SkipFunction skipFunction( ::boost::bind(static_cast<bool (SubGraph::*)(Vertex::Ptr) const>(&SubGraph::disabled), this,_1) );
    vertexIt->setSkipFunction(skipFunction);
    return vertexIt;
}


EdgeIterator::Ptr SubGraph::getEdgeIterator() const
{
    EdgeIterator::Ptr edgeIt = getBaseGraph()->getEdgeIterator();
    // Need to explicitely cast skip function to disambiguate (
    // disable(Vertex::Ptr) vs. disable(Edge::Ptr)
    EdgeIterator::SkipFunction skipFunction( ::boost::bind(static_cast<bool (SubGraph::*)(Edge::Ptr) const>(&SubGraph::disabled), this,_1) );
    edgeIt->setSkipFunction(skipFunction);
    return edgeIt;
}

EdgeIterator::Ptr SubGraph::getEdgeIterator(Vertex::Ptr vertex) const
{
    if(disabled(vertex))
    {
        throw std::invalid_argument("graph_analysis::SubGraph::getEdgeIterator: cannot get iterator for a disabled vertex, use BaseGraph instance instead");
    }

    EdgeIterator::Ptr edgeIt = getBaseGraph()->getEdgeIterator(vertex);
    EdgeIterator::SkipFunction skipFunction( ::boost::bind(static_cast<bool (SubGraph::*)(Edge::Ptr) const>(&SubGraph::disabled), this,_1) );
    edgeIt->setSkipFunction(skipFunction);
    return edgeIt;
}

} // end namespace graph_analysis
