#ifndef GRAPH_ANALYSIS_SNAP_GRAPH_HPP
#define GRAPH_ANALYSIS_SNAP_GRAPH_HPP

#include <map>
#include <graph_analysis/Graph.hpp>

// Workaround for snap define
#ifndef SIZE_MAX
    #define SIZE_MAX 0xffffffff
#endif
#include <snap/snap-core/Snap.h>

#include <graph_analysis/snap/NodeIterator.hpp>
#include <graph_analysis/snap/EdgeIterator.hpp>

namespace graph_analysis {
namespace snap {

/**
 * \class DirectedGraph
 * \brief Directed graph implementation based on lemon library
 */
class DirectedGraph : public TypedGraph< PNEGraph >
{
public:
    typedef PNEGraph SubGraph;

    typedef std::map<TInt, Edge::Ptr> EdgeMap;
    typedef std::map<TInt, Vertex::Ptr> VertexMap;

    friend class NodeIterator<DirectedGraph>;
    friend class EdgeIterator<DirectedGraph>;

    /**
     * \brief Default constructor of the graph
     */
    DirectedGraph()
        : TypedGraph()
    {
        mGraph = TNEGraph::New();
    }

    /**
     * \brief Add a vertex
     * \return the created vertex
     */
    void addVertex(Vertex::Ptr vertex)
    {
        BaseGraph::addVertex(vertex);

        TInt nodeId = mGraph->AddNode();
        mVertexMap[nodeId] = vertex;

        vertex->associate(getId(), nodeId);
    }

    /**
     * \brief Add an edge
     * \return an edge interator
     */
    void addEdgeInternal(Edge::Ptr edge, GraphElementId sourceVertexId, GraphElementId targetVertexId)
    {
        TInt edgeId = mGraph->AddEdge(sourceVertexId,targetVertexId);
        edge->associate(getId(), edgeId);
        mEdgeMap[edgeId] = edge;
    }

    /**
     * \brief Get the source vertex for this edge
     * \return Pointer to the vertex data
     */
    Vertex::Ptr getSourceVertex(Edge::Ptr e) const
    {
        TInt nodeId = mGraph->GetEI( getEdgeId(e) ).GetSrcNId();

        VertexMap::const_iterator cit = mVertexMap.find(nodeId);
        if(cit != mVertexMap.end())
        {
            return cit->second;
        }

        throw std::runtime_error("SNAP: could not find source vertex for edge");
    }

    /**
     * \brief Get the target vertex for this edge
     * \return Pointer to the vertex data
     */
    Vertex::Ptr getTargetVertex(Edge::Ptr e) const
    {
        TInt nodeId = mGraph->GetEI( getEdgeId(e) ).GetDstNId();

        VertexMap::const_iterator cit = mVertexMap.find(nodeId);
        if(cit != mVertexMap.end())
        {
            return cit->second;
        }

        throw std::runtime_error("SNAP: could not find target vertex for edge");
    }

    SubGraph applyFilters(Filter<Vertex::Ptr>::Ptr vertexFilter, Filter<Edge::Ptr>::Ptr edgeFilter)
    {
        SubGraph subgraph;
        //TSnap::GetSubGraph(mGraph, TIntV::GetV(0));

        return subgraph;
    }

    /**
     * Get the vertex iterator for this implementation
     */
    VertexIterator::Ptr getVertexIterator()
    {
        NodeIterator<DirectedGraph>* it = new NodeIterator<DirectedGraph>(*this);
        return VertexIterator::Ptr(it);
    }

    /**
     * Get the edge iterator for this implementation
     */
    graph_analysis::EdgeIterator::Ptr getEdgeIterator()
    {
        EdgeIterator<DirectedGraph>* it = new EdgeIterator<DirectedGraph>(*this);
        return graph_analysis::EdgeIterator::Ptr(it);
    }

private:
    EdgeMap mEdgeMap;
    VertexMap mVertexMap;
};

} // end namespace snap
} // end namespace graph_analysis
#endif // GRAPH_ANALYSIS_SNAP_GRAPH_HPP
