#ifndef GRAPH_ANALYSIS_ALGORITHMS_DFS_VISITOR_HPP
#define GRAPH_ANALYSIS_ALGORITHMS_DFS_VISITOR_HPP

#include <graph_analysis/algorithms/Visitor.hpp>
#include <base/Logging.hpp>

namespace graph_analysis {
namespace algorithms {

class DFSVisitor : public Visitor
{
public:
    typedef shared_ptr<DFSVisitor> Ptr;

    virtual void initializeVertex(Vertex::Ptr& vertex) const { LOG_DEBUG_S << "initializeVertex" << vertex->toString(); }
    virtual void leafVertex(Vertex::Ptr& vertex) const { LOG_DEBUG_S << "leafVertex" << vertex->toString(); }

    virtual void examineEdge(Edge::Ptr& edge) const { LOG_DEBUG_S << "examineEdge: " << edge->toString(); }
    virtual void treeEdge(Edge::Ptr& edge) const { LOG_DEBUG_S << "treeEdge: " << edge->toString(); }
    virtual void forwardOrCrossEdge(Edge::Ptr& edge) const { LOG_DEBUG_S << "forwardOrCrossEdge: " << edge->toString(); }
    virtual void backEdge(Edge::Ptr& edge) const { LOG_DEBUG_S << "backEdge: " << edge->toString(); }

    virtual void discoverVertex(Vertex::Ptr& vertex) const { LOG_DEBUG_S << "discoverVertex: " << vertex->toString(); }
    virtual void finishVertex(Vertex::Ptr& vertex) const { LOG_DEBUG_S << "finishVertex: " << vertex->toString(); }

};

} // end namespace algorithms
} // end namespace graph_analysis
#endif // GRAPH_ANALYSIS_ALGORITHMS_DFS_VISITOR_HPP
