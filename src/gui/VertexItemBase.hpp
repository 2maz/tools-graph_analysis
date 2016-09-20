#ifndef GRAPH_ANALYSIS_GUI_VERTEXITEMBASE_HPP
#define GRAPH_ANALYSIS_GUI_VERTEXITEMBASE_HPP

#include <QGraphicsWidget>
#include <graph_analysis/Vertex.hpp>

#include "GraphicsItemTypes.hpp"

class QGraphicsSceneMouseEvent;

namespace graph_analysis
{

class BaseGraph;

namespace gui
{

class EdgeItemBase;
class GraphWidget;

/**
 * \file VertexItemBase.hpp
 * \class VertexItemBase
 * \brief graphical node representation interface
 * \details used as polymorphic base for several graphical node implementations
 */
class VertexItemBase : public QGraphicsItem, public QGraphicsLayoutItem
{
public:
    /**
     * \brief constructor
     * \param graphWidget the parent and managing graph widget
     * \param vertex the internal conceptual vertex
     * \param parent the parent
     */
    VertexItemBase(GraphWidget* graphWidget, graph_analysis::Vertex::Ptr vertex,
                   QGraphicsItem* parent);

    // destructor
    virtual ~VertexItemBase();

    virtual int type() const
    {
        return VertexItemBaseType;
    };

    /**
     * return a point where an edge should point to when it connects to this
     * vertex
     */
    virtual QPointF getConnector() const
    {
        return mapToScene(boundingRect().center());
    };

    /// getter method for retrieving the underlying conceptual graph vertex
    graph_analysis::Vertex::Ptr getVertex() const
    {
        return mpVertex;
    }
    /// setter method for updating the underlying conceptual graph vertex
    void setVertex(graph_analysis::Vertex::Ptr vertex)
    {
        mpVertex = vertex;
    }
    /// getter method for retrieving the parent managing graph widget
    GraphWidget* getGraphWidget() const
    {
        return mpGraphWidget;
    }

    /**
     * callback to trigger the base-graph to adjust all edges of this vertex
     * after its position has changed in the scene.
     */
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant& value);

    /**
     * calculates the two intersection points between the connector of this
     * item and the connector of the otherItem with the bounding-box of the two
     * respective items.
     */
    QPointF getIntersectionPoint(VertexItemBase* otherItem) const;

protected:
    /* underlying graph vertex pointer */
    graph_analysis::Vertex::Ptr mpVertex;

    /* parent managing graph widget. this should be reachable via "scene()"? */
    GraphWidget* mpGraphWidget;

    /**
     * extracting the BaseGraph from the GraphWidget
     */
    shared_ptr<BaseGraph> getGraph() const;

    /**
     * this is used to update the statusbar of the current active widget with
     * information about the currently hovered (or not hovered) vertex
     */
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent* event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent* event);
};

/**
 * simplest possible implementation: just a box with two strings -- type and
 * label
 */
class VertexItemSimple : public VertexItemBase
{
public:
    VertexItemSimple(GraphWidget* graphWidget,
                     graph_analysis::Vertex::Ptr vertex, QGraphicsItem* parent);
    ~VertexItemSimple();
    virtual int type() const
    {
        return VertexItemSimpleType;
    };

    // inherited from QGraphicsLayoutItem
    virtual void setGeometry(const QRectF& geometry);
    virtual QSizeF sizeHint(Qt::SizeHint which, const QSizeF& constraint = QSizeF()) const;

    // inherited from QGraphicsItem
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
               QWidget* widget = 0);
    QRectF boundingRect() const;

    /**
     * uses this (read only) notification to update the label showing current
     * canvas coordinate of the whole item. nice for debugging.
     */
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant& value);

protected:
    /**
     * all items by default accept drag-n-drop events. override this function
     * in later classes to prevent certain drops?
     */
    void dragEnterEvent(QGraphicsSceneDragDropEvent* event);
    void dragLeaveEvent(QGraphicsSceneDragDropEvent* event);
    void dropEvent(QGraphicsSceneDragDropEvent* event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);

private:
    QGraphicsTextItem* mpLabel;
    QGraphicsTextItem* mpClassName;
    QGraphicsTextItem* mpCoordinate;
    QGraphicsRectItem* mpRect;

    /** convert the current scenePos of the item to smth like "(23, -14)" */
    QString getScenePosAsString() const;
};

} // end namespace gui
} // end namespace graph_analysis
#endif
