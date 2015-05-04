#include "Edge.hpp"

namespace graph_analysis {

Edge::Edge(const std::string& label)
    : GraphElement()
    , mCachedLabel(label)
    , mLabel(label)
    , mBidirectionalActive(false)
{}

Edge::Edge(Vertex::Ptr source, Vertex::Ptr target, const std::string& label)
    : mSourceVertex(source)
    , mTargetVertex(target)
    , mCachedLabel(label)
    , mLabel(label)
    , mBidirectionalActive(false)
{}

std::string Edge::getClassName() const
{
    return "graph_analysis::Edge";
}

std::string Edge::toString() const
{
    std::stringstream ss;
    if(' ' == mLabel[0])
    {
        return mLabel;
    }
    ss << (mLabel.empty() ? getClassName() : mLabel) << ":";
    if(mSourceVertex)
    {
        ss << mSourceVertex->toString();
    } else {
        ss << "n/a";
    }
    ss << (mBidirectionalActive ? "<->" : "->");
    if(mTargetVertex)
    {
        ss << mTargetVertex->toString();
    } else {
        ss << "n/a";
    }
    return ss.str();
}

std::vector<Vertex::Ptr> Edge::getInvolvedVertices(Edge::Ptr e0, Edge::Ptr e1)
{
    std::vector<Vertex::Ptr> vertices;
    Vertex::Ptr e0_source = e0->getSourceVertex();
    Vertex::Ptr e0_target = e0->getTargetVertex();
    if(e0_source == e1->getSourceVertex() || e0_source == e1->getTargetVertex())
    {
        // skip e0_source
    } else {
        vertices.push_back(e0_source);
    }

    if(e0_target == e1->getSourceVertex() || e0_target == e1->getTargetVertex())
    {
        // skip e0_target
    } else {
        vertices.push_back(e0_source);
    }

    vertices.push_back(e1->getSourceVertex());
    vertices.push_back(e1->getTargetVertex());

    return vertices;
}

bool Edge::areMeeting(Edge::Ptr e0, Edge::Ptr e1)
{
    return getInvolvedVertices(e0, e1).size() < 4;
}

}
