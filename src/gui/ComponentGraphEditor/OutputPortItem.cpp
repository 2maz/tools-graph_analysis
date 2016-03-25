#include "OutputPortItem.hpp"

#include <QPen>
#include <QGraphicsSceneMoveEvent>

#include <base/Logging.hpp>

namespace graph_analysis
{
namespace gui
{

// kiss:
OutputPortItem::OutputPortItem(GraphWidget* graphWidget,
                               graph_analysis::OutputPort::Ptr vertex,
                               QGraphicsItem* parent)
    : PortItem(graphWidget, vertex, parent)
{
    updateStrings();
}

OutputPortItem::~OutputPortItem()
{
}

void OutputPortItem::updateStrings()
{
    mpLabel->setPlainText(QString("out: ") +
                          QString(mpVertex->getLabel().c_str()));
    mpRect->setRect(mpLabel->boundingRect());
}

void OutputPortItem::paint(QPainter* painter,
                           const QStyleOptionGraphicsItem* option,
                           QWidget* widget)
{
}

QRectF OutputPortItem::boundingRect() const
{
    return childrenBoundingRect();
}

/* void OutputPortItem::dragEnterEvent(QGraphicsSceneDragDropEvent* event) */
/* { */
/*     // do nothing. decline. */
/* } */

} // end namespace gui
} // end namespace graph_analysis
