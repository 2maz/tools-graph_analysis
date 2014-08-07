#ifndef OMVIZ_GRAPHWIDGET_GRAPHITEM_EDGES_SIMPLE_HPP
#define OMVIZ_GRAPHWIDGET_GRAPHITEM_EDGES_SIMPLE_HPP

#include <omviz/graphwidget/EdgeItem.hpp>
#include <QPainterPath>

namespace omviz {
namespace graphitem {
namespace edges {

class EdgeLabel;

class Simple : public omviz::EdgeItem
{
    Simple(NodeItem* sourceNode, NodeItem* targetNode, graph_analysis::Edge::Ptr vertex);
public:
    Simple() {}
    virtual ~Simple() {}

    virtual void adjust(); 

protected:
    virtual QRectF boundingRect() const;
    virtual QPainterPath shape() const { return ::omviz::EdgeItem::shape(); }
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget*);

    virtual EdgeItem* createNewItem(NodeItem* sourceNode, NodeItem* targetNode, graph_analysis::Edge::Ptr edge) const { return new Simple(sourceNode, targetNode, edge); }

    EdgeLabel* mLabel;

};

} // end namespace edges
} // end namespace graphitem
} // end namespace omviz
#endif // OMVIZ_GRAPHWIDGET_GRAPHITEM_EDGES_SIMPLE_HPP
