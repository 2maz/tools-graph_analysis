#include "Simple.hpp"

#include <QPainter>
#include <QStyle>
#include <QStyleOption>

namespace graph_analysis {
namespace gui {
namespace graphitem {

Simple::Simple(GraphWidget* graphWidget, graph_analysis::Vertex::Ptr vertex)
    : NodeItem(graphWidget, vertex, this)
{
}

QRectF Simple::boundingRect() const
{
    qreal adjust = 2;
    return QRectF( -10 - adjust, -10 - adjust, 23 + adjust, 23 + adjust);
}

QPainterPath Simple::shape() const
{
    QPainterPath path;
    path.addEllipse(-10, -10, 20, 20);
    return path;
}

void Simple::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* )
{
    painter->setPen(Qt::NoPen);
    painter->setBrush(Qt::darkGray);
    painter->drawEllipse(-7, -7, 20, 20);
}

} // end namespace graphitem
} // end namespace gui
} // end namespace graph_analysis
