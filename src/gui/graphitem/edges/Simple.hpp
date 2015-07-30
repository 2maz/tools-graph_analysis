#ifndef GRAPH_ANALYSIS_GRAPHWIDGET_GRAPHITEM_EDGES_SIMPLE_HPP
#define GRAPH_ANALYSIS_GRAPHWIDGET_GRAPHITEM_EDGES_SIMPLE_HPP

#include <graph_analysis/gui/EdgeItem.hpp>
#include <QPainterPath>
#include <QColor>
#include <QGraphicsLineItem>

namespace graph_analysis {
namespace gui {
namespace graphitem {
namespace edges {

class EdgeLabel;

/**
 * \class Simple
 * \file Simple.hpp
 * \brief graphical edge implementation
 */
class Simple : public graph_analysis::gui::EdgeItem
{
private:
    /**
     * \brief constructor; private since instances of this class only spawn via the EdgeTypeManager factory class
     * \param graphWidget managing widget
     * \param sourceNode source port vertex
     * \param sourceNodePortID source ID
     * \param targetNode target port vertex
     * \param targetNodePortID target ID
     * \param edge internal conceptual edge
     */
    Simple(GraphWidget* graphWidget, NodeItem* sourceNode, int sourceNodePortID, NodeItem* targetNode, int targetNodePortID, graph_analysis::Edge::Ptr edge);
public:
    /// empty constructor
    Simple() {}
    /// getter method for the graphical text label member field
    virtual EdgeLabel* getLabel() { return mpLabel; }
    /// destructor
    virtual ~Simple() {}
    /// getter method for source ID
    int getSourcePortID() { return mSourceNodePortID; }
    /// getter method for target ID
    int getTargetPortID() { return mTargetNodePortID; }
    /// adjusts edge segment length and starting and ending points
    virtual void adjust();

    /// willingly gives up scene focus
    void releaseFocus();
    /// adjusts graphical text label position
    void adjustLabel();

protected:
    /// claims the scene focus (i.e. when double-clicked; turns red)
    void grabFocus();
    /// retrieves the bounding rectangular box around the area occupied by the node in the scene
    virtual QRectF boundingRect() const;
    /// retrieves the path of the bounding rectangular box around the area occupied by the node in the scene
    virtual QPainterPath shape() const { return ::graph_analysis::gui::EdgeItem::shape(); }
    /// qt edge painting method - here the edge components get placed in the scene (the line segment and arrow tip)
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget*);
    /// qt hovering ENTER callback
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    /// qt hovering LEAVE callback
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    /// qt mouse double click callback (toggles the scene focusing on this item)
    void mouseDoubleClickEvent(::QGraphicsSceneMouseEvent* event);

    /// establishes where the edge line segment shall end at one of the extremities (computes intersection with the rectangular boundary of one of the endpoints)
    QPointF getIntersectionPoint(NodeItem* item, const QLineF& line, int portID = -1);

    /// edge cloning/spawning method used by the factory to produce new edges
    virtual EdgeItem* createNewItem(GraphWidget* graphWidget, NodeItem* sourceNode, int sourceNodePortID, NodeItem* targetNode, int targetNodePortID, graph_analysis::Edge::Ptr edge) const { return new Simple(graphWidget, sourceNode, sourceNodePortID, targetNode, targetNodePortID, edge); }

    /// the graphical text label
    EdgeLabel* mpLabel;
    /// current qt drawing pen
    QPen mPen;
    /// the default qt drawing pen
    QPen mPenDefault;
    /// ending arrow tip
    QPolygonF mArrowHead;
    /// edge graphical line segment
    QLineF mLine;
    /// source ID
    int mSourceNodePortID;
    /// target ID
    int mTargetNodePortID;
    /// boolean flag: true when current edge is being the focused edge of the scene (has been double clicked); false otherwise
    bool mFocused;
    /// boolean flag: true when current edge is being the seleted edge of the scene (is being hovered on); false otherwise
    bool mSelected;
};

} // end namespace edges
} // end namespace graphitem
} // end namespace gui
} // end namespace graph_analysis
#endif // GRAPH_ANALYSIS_GRAPHWIDGET_GRAPHITEM_EDGES_SIMPLE_HPP
