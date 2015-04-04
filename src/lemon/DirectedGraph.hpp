#ifndef GRAPH_ANALYSIS_LEMON_DIRECTED_GRAPH_HPP
#define GRAPH_ANALYSIS_LEMON_DIRECTED_GRAPH_HPP


#include <lemon/adaptors.h>
#include <lemon/lgf_writer.h>
#include <lemon/connectivity.h>

#include <graph_analysis/DirectedGraphInterface.hpp>
#include <graph_analysis/lemon/DirectedSubGraph.hpp>


namespace graph_analysis {
namespace lemon {

/**
 * \class DirectedGraph
 * \brief Directed graph implementation based on lemon library
 */
class DirectedGraph : public TypedGraph< ::lemon::ListDigraph >, graph_analysis::DirectedGraphInterface
{
public:
    typedef boost::shared_ptr<DirectedGraph> Ptr;

    /**
     * \brief Default constructor of the graph
     */
    DirectedGraph();

    ~DirectedGraph();

    /**
     * Copy the graph
     */
    BaseGraph::Ptr copy();

    /**
     * Copy the graph
     */
    BaseGraph::Ptr cleanCopy();

    typedef graph_t::ArcMap< Edge::Ptr > EdgeMap;
    typedef graph_t::NodeMap< Vertex::Ptr > VertexMap;

    typedef graph_t::NodeMap<bool> VertexActivationMap;
    typedef graph_t::ArcMap<bool> EdgeActivationMap;

    typedef graph_t::ArcMap< std::string > EdgeStringMap;
    typedef graph_t::NodeMap< std::string > VertexStringMap;

    typedef graph_t::ArcMap< GraphElementId > EdgeIdMap;
    typedef graph_t::NodeMap< GraphElementId > VertexIdMap;

    // Register the DirectedSubGraph as subgraph type
    typedef DirectedSubGraph subgraph_t;

    friend class NodeIterator<DirectedGraph>;
    friend class ArcIterator<DirectedGraph>;
    friend class InArcIterator<DirectedGraph>;
    friend class OutArcIterator<DirectedGraph>;

    /**
     * \brief Add a vertex
     * \return the created vertex
     */
    virtual GraphElementId addVertex(Vertex::Ptr vertex);

    /**
     * \brief Add an edge
     * \return the created edge
     */
    GraphElementId addEdgeInternal(Edge::Ptr edge, GraphElementId sourceVertexId, GraphElementId targetVertexId);

    Vertex::Ptr getVertex(GraphElementId id) const;
    Edge::Ptr getEdge(GraphElementId id) const;

    /**
     * \brief Get the source vertex for this edge
     * \return Pointer to the vertex data
     */
    Vertex::Ptr getSourceVertex(Edge::Ptr e) const;

    /**
     * \brief Get the target vertex for this edge
     * \return Pointer to the vertex data
     */
    Vertex::Ptr getTargetVertex(const Edge::Ptr& e) const;

    void removeVertex(Vertex::Ptr vertex);
    void removeEdge(Edge::Ptr edge);

    DirectedGraph(const DirectedGraph& other);

    /**
     * \brief Direct usage off operator= is disallowed in lemon, thus
     * need for explicit usage of copy functions
     */
    DirectedGraph& operator=(const DirectedGraph& other);

    void write(std::ostream& ostream = std::cout) const;

    /**
     * Get the vertex iterator for this implementation
     */
    VertexIterator::Ptr getVertexIterator();

    /**
     * Get the edge iterator for this implementation
     */
    EdgeIterator::Ptr getEdgeIterator();

    EdgeIterator::Ptr getEdgeIterator(Vertex::Ptr vertex);

    EdgeIterator::Ptr getOutEdgeIterator(Vertex::Ptr vertex);
    EdgeIterator::Ptr getInEdgeIterator(Vertex::Ptr vertex);

    /**
     * Identifies the connected components
     */
    SubGraph::Ptr identifyConnectedComponents(BaseGraph::Ptr baseGraph);

protected:
    // Property maps to store data associated with vertices and edges
    EdgeMap mEdgeMap;
    VertexMap mVertexMap;

    /**
     * Get the subgraph -- by default all vertices and edges of the
     * base graph are available (enabled)
     */
    SubGraph::Ptr createSubGraph(BaseGraph::Ptr graph);
};

} // end namespace lemon
} // end namespace graph_analysis
#endif // GRAPH_ANALYSIS_LEMON_DIRECTED_GRAPH_HPP
