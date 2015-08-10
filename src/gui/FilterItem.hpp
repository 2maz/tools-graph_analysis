#ifndef FILTERITEM_HPP
#define FILTERITEM_HPP

#include <QPen>
#include <string>
#include <QGraphicsItem>
#include <QGraphicsItemGroup>

namespace graph_analysis {
namespace gui {

namespace layeritem {
    class Label;
} // end namespace layeritem

/**
 * \file FilterItem.hpp
 * \class FilterItem
 * \brief graphical filter representation class; specific to FilterManager widget in the layers tab
 */
class FilterItem : public QGraphicsItemGroup
{
public:
    /// constructor
    FilterItem(const std::string& filter = std::string());
    /// destructor
    ~FilterItem() {};

    /// retrieves the bounding rectangular box around the area occupied by the node in the scene
    QRectF boundingRect() const;
    /// retrieves the path of the bounding rectangular box around the area occupied by the node in the scene
    QPainterPath shape() const;
    /// qt node painting method - here the node components get placed in the scene
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget*);

protected:
    /// qt mouse double-click callback
    void mouseDoubleClickEvent(::QGraphicsSceneMouseEvent* event);
    /// qt mouse press callback
    void mousePressEvent(::QGraphicsSceneMouseEvent* event);
    /// qt mouse release callback
    void mouseReleaseEvent(::QGraphicsSceneMouseEvent* event);

    /// qt hovering ENTER callback
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    /// qt hovering LEAVE callback
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    /// qt item change callback
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

private:
    /// main node text label
    layeritem::Label* mLabel;
    /// qt drawing pen
    QPen mPen;
    /// default qt drawing pen
    QPen mPenDefault;
};

} // end namespace gui
} // end namespace graph_analysis
#endif // FILTERITEM_HPP