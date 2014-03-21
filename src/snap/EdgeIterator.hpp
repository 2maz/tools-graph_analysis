#ifndef GRAPH_ANALYSIS_SNAP_EDGE_ITERATOR_HPP
#define GRAPH_ANALYSIS_SNAP_EDGE_ITERATOR_HPP

#include <graph_analysis/VertexIterator.hpp>

namespace graph_analysis {
namespace snap {

template<typename T>
class EdgeIterator : public graph_analysis::EdgeIterator
{
public:
    EdgeIterator(T& graph)
        : mGraph(graph)
        , mEdgeIt(graph.raw()->BegEI())
    {}

    bool next()
    {
        if( mEdgeIt != mGraph.raw()->EndEI() )
        {
            setNext( mGraph.mEdgeMap[mEdgeIt.GetId()] );
            mEdgeIt++;
            return true;
        }
        return false;
    }

protected:
    T& mGraph;
    typename TNEGraph::TEdgeI mEdgeIt;

};

template<typename T>
class OutEdgeIterator : public graph_analysis::EdgeIterator
{
public:
    OutEdgeIterator(T& graph, TNEGraph::TNodeI nodeIterator)
        : mGraph(graph)
        , mCurrentIndex(0)
        , mNodeIt(nodeIterator)
    {}

    bool next()
    {
        if( mCurrentIndex != mNodeIt.GetOutDeg())
        {
            TNEGraph::TEdgeI edgeIt = mGraph.raw()->GetEI(mNodeIt.GetId(), mNodeIt.GetOutNId(mCurrentIndex));
            setNext( mGraph.mEdgeMap[ edgeIt.GetId() ] );
            ++mCurrentIndex;
            return true;
        }
        return false;
    }

protected:
    T& mGraph;
    int mCurrentIndex;
    typename TNEGraph::TNodeI mNodeIt;
};

template<typename T>
class InEdgeIterator : public graph_analysis::EdgeIterator
{
public:
    InEdgeIterator(T& graph, TNEGraph::TNodeI nodeIterator)
        : mGraph(graph)
        , mCurrentIndex(0)
        , mNodeIt(nodeIterator)
    {}

    bool next()
    {
        if( mCurrentIndex != mNodeIt.GetInDeg())
        {
            TNEGraph::TEdgeI edgeIt = mGraph.raw()->GetEI(mNodeIt.GetId(), mNodeIt.GetInNId(mCurrentIndex));
            setNext( mGraph.mEdgeMap[edgeIt.GetId()] );
            ++mCurrentIndex;
            return true;
        }
        return false;
    }

protected:
    T& mGraph;
    int mCurrentIndex;
    typename TNEGraph::TNodeI mNodeIt;
};
} // end namespace snap
} // end namespace graph_analysis
#endif // GRAPH_ANALYSIS_SNAP_EDGE_ITERATOR_HPP
