#include "DirectedGraph.hpp"
#include <base/Logging.hpp>
#include <graph_analysis/filters/CommonFilters.hpp>

namespace graph_analysis {
namespace lemon {

DirectedGraph::DirectedGraph()
    : TypedGraph(LEMON_DIRECTED_GRAPH)
    , mEdgeMap(raw())
    , mVertexMap(raw())
{}

DirectedGraph::DirectedGraph(const DirectedGraph& other)
    : TypedGraph(LEMON_DIRECTED_GRAPH)
    , mEdgeMap(raw())
    , mVertexMap(raw())
{
    *this = other;
}

DirectedGraph::~DirectedGraph()
{}

GraphElementId DirectedGraph::addVertex(Vertex::Ptr vertex)
{
    BaseGraph::addVertex(vertex);

    graph_t::Node node = mGraph.addNode();
    int nodeId = mGraph.id(node);
    mVertexMap[node] = vertex;

    vertex->associate(getId(), nodeId);
    return nodeId;
}

GraphElementId DirectedGraph::addEdgeInternal(Edge::Ptr edge, GraphElementId sourceVertexId, GraphElementId targetVertexId)
{
    graph_t::Node sourceNode = mGraph.nodeFromId( sourceVertexId );
    graph_t::Node targetNode = mGraph.nodeFromId( targetVertexId );

    graph_t::Arc arc = mGraph.addArc(sourceNode, targetNode);
    int arcId = mGraph.id(arc);
    edge->associate(getId(), arcId);
    mEdgeMap[arc] = edge;

    return arcId;
}

Vertex::Ptr DirectedGraph::getVertex(GraphElementId id) const
{
    return mVertexMap[ mGraph.nodeFromId(id) ];
}

Edge::Ptr DirectedGraph::getEdge(GraphElementId id) const
{
    return mEdgeMap[ mGraph.arcFromId(id) ];
}

Vertex::Ptr DirectedGraph::getSourceVertex(Edge::Ptr e) const
{
    GraphElementId edgeId = getEdgeId(e);
    return mVertexMap[ mGraph.source( mGraph.arcFromId(edgeId)) ];
}

Vertex::Ptr DirectedGraph::getTargetVertex(const Edge::Ptr& e) const
{
    GraphElementId edgeId = getEdgeId(e);
    return mVertexMap[ mGraph.target( mGraph.arcFromId(edgeId)) ];
}

void DirectedGraph::removeVertex(Vertex::Ptr vertex)
{
    int nodeId = getVertexId( vertex );

    BaseGraph::removeVertex(vertex);

    graph_t::Node node = mGraph.nodeFromId(nodeId);
    mGraph.erase(node);
}

void DirectedGraph::removeEdge(Edge::Ptr edge)
{
    int edgeId = getEdgeId(edge);

    BaseGraph::removeEdge(edge);

    graph_t::Arc arc = mGraph.arcFromId(edgeId);
    mGraph.erase(arc);
}

/**
 * \brief Direct usage off operator= is disallowed in lemon, thus
 * need for explicit usage of copy functions
 */
DirectedGraph& DirectedGraph::operator=(const DirectedGraph& other)
{
    ::lemon::digraphCopy(other.mGraph, this->mGraph).
        nodeMap(other.mVertexMap, this->mVertexMap).
        arcMap(other.mEdgeMap, this->mEdgeMap).
        run();

    for( graph_t::NodeIt n(this->mGraph); n != ::lemon::INVALID; ++n)
    {
        Vertex::Ptr vertex = mVertexMap[n];
        vertex->associate(this->getId(), this->mGraph.id(n));
    }

    for( graph_t::ArcIt a(this->mGraph); a != ::lemon::INVALID; ++a)
    {
        Edge::Ptr edge = mEdgeMap[a];
        edge->associate(this->getId(), this->mGraph.id(a));
    }

    return *this;
}

void DirectedGraph::write(std::ostream& ostream) const
{
    // Workaround:
    // operator<
    // will be overloaded ambiguously due to using shared_ptr
    // Use explicit conversion to string map first

    EdgeStringMap edgeStringMap(mGraph);
    VertexStringMap vertexStringMap(mGraph);
    EdgeIdMap edgeIdMap(mGraph);
    VertexIdMap vertexIdMap(mGraph);

    for(graph_t::ArcIt a(mGraph); a != ::lemon::INVALID; ++a)
    {
        Edge::Ptr edge = mEdgeMap[a];
        if(edge)
        {
            edgeStringMap[a] = edge->toString();
            edgeIdMap[a] = getEdgeId(edge);
        }
    }

    for(graph_t::NodeIt n(mGraph); n != ::lemon::INVALID; ++n)
    {
        Vertex::Ptr vertex = mVertexMap[n];
        if(vertex)
        {
            vertexStringMap[n] = vertex->toString();
            vertexIdMap[n] = getVertexId(vertex);
        }
    }

    ::lemon::digraphWriter(mGraph, ostream).
        arcMap("edges", edgeStringMap).
        nodeMap("vertices", vertexStringMap).
        arcMap("edgeId", edgeIdMap).
        nodeMap("vertexId", vertexIdMap).
        attribute("caption", "test").
        run();
}

VertexIterator::Ptr DirectedGraph::getVertexIterator()
{
    NodeIterator<DirectedGraph>* it = new NodeIterator<DirectedGraph>(*this);
    return VertexIterator::Ptr(it);
}

EdgeIterator::Ptr DirectedGraph::getEdgeIterator()
{
    ArcIterator<DirectedGraph>* it = new ArcIterator<DirectedGraph>(*this);
    return EdgeIterator::Ptr(it);
}

EdgeIterator::Ptr DirectedGraph::getEdgeIterator(Vertex::Ptr vertex)
{
    InOutArcIterator<DirectedGraph>* it = new InOutArcIterator<DirectedGraph>(*this, vertex);
    return EdgeIterator::Ptr(it);
}

EdgeIterator::Ptr DirectedGraph::getOutEdgeIterator(Vertex::Ptr vertex)
{
    OutArcIterator<DirectedGraph>* it = new OutArcIterator<DirectedGraph>(*this, vertex);
    return EdgeIterator::Ptr(it);
}

EdgeIterator::Ptr DirectedGraph::getInEdgeIterator(Vertex::Ptr vertex)
{
    InArcIterator<DirectedGraph>* it = new InArcIterator<DirectedGraph>(*this, vertex);
    return EdgeIterator::Ptr(it);
}

SubGraph::Ptr DirectedGraph::identifyConnectedComponents(BaseGraph::Ptr baseGraph)
{
    SubGraph::Ptr subgraph = createSubGraph(baseGraph);

    ::lemon::Undirector<DirectedGraph::graph_t> undirected(raw());
    // Identify the components
    graph_t::NodeMap<int> nodeMap(raw(),false);
    int componentCount = ::lemon::connectedComponents(undirected, nodeMap);
    // Add a single vertex per identified component
    // activate that node in the subgraph
    Vertex::Ptr components[componentCount];
    for(int i = 0; i < componentCount; ++i)
    {
        Vertex::Ptr vertex(new Vertex());
        components[i] = vertex;

        // Activate this node in the subgraph
        // disabling all other will be in the next loop
        addVertex(vertex);
        subgraph->enable( vertex );
    }

    if(componentCount > 0)
    {
        // Disable all nodes in the subgraph that are not representing a component
        VertexIterator::Ptr vertexIterator = subgraph->getVertexIterator();
        while(vertexIterator->next())
        {
            Vertex::Ptr sourceVertex = vertexIterator->current();

            bool isComponentNode = false;
            for(int a = 0; a < componentCount; ++a)
            {
                if(sourceVertex->getUid() == components[a]->getUid())
                {
                    isComponentNode = true;
                    break;
                }
            }

            if(isComponentNode)
            {
                continue;
            }

            graph_t::Node node = raw().nodeFromId( sourceVertex->getId(getId()) );
            Vertex::Ptr targetVertex = components[ nodeMap[node] ];
            subgraph->disable(sourceVertex);

            // Add an edge to relate vertices to their components
            Edge::Ptr edge( new Edge(sourceVertex, targetVertex) );
            addEdge(edge);
        }
    } else {
        LOG_DEBUG("no component found in graph");
    }

    return subgraph;
}

SubGraph::Ptr DirectedGraph::createSubGraph(BaseGraph::Ptr baseGraph)
{
    DirectedGraph::Ptr directedGraph = boost::dynamic_pointer_cast<DirectedGraph>(baseGraph);
    if(!directedGraph)
    {
        throw std::invalid_argument("graph_analysis::lemon::DirectedGraph::createSubGraph: base graph could not be cast to DirectedGraph");
    }

    SubGraph::Ptr subgraph(new DirectedSubGraph(directedGraph));
    // Enable all nodes and edges by default
    Filter< Vertex::Ptr >::Ptr vertexFilter(new filters::PermitAll< Vertex::Ptr >() );
    Filter< Edge::Ptr >::Ptr edgeFilter(new filters::PermitAll< Edge::Ptr >() );
    subgraph->applyFilters(vertexFilter, edgeFilter);

    return subgraph;
}
BaseGraph::Ptr DirectedGraph::copy()
{
    return boost::shared_ptr<BaseGraph>(new DirectedGraph(*this));
}

BaseGraph::Ptr DirectedGraph::cleanCopy()
{
    return boost::shared_ptr<BaseGraph>(new DirectedGraph());
}

} // end namespace lemon
} // end namespace graph_analysis