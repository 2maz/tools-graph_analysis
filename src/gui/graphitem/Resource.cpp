#include "Resource.hpp"

#include <cmath>
#include <QStyle>
#include <QPainter>
#include <QStyleOption>
#include <QGraphicsSceneDragDropEvent>
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>
#include <base/Logging.hpp>
#include <exception>
#include "Label.hpp"

#define max(X, Y) X > Y ? X : Y
#define ADJUST 23.69
#define EPSILON 0.001
#define SEPARATOR 69.


//#define MSEPARATOR
#define MMAXINPUTOUTPUTPORTWIDTH

namespace graph_analysis {
namespace gui {
namespace graphitem {

Resource::Resource(GraphWidget* graphWidget, graph_analysis::Vertex::Ptr vertex)
    : NodeItem(graphWidget, vertex)
    , mPen(Qt::blue)
    , mPenDefault(Qt::blue)
    , mFocused(false)
    , mSelected(false)
    , mID(0)
    , mInPorts(0)
    , mOutPorts(0)
    , mMaxInputPortWidth(0.)
    , mMaxOutputPortWidth(0.)
    , mSeparator(SEPARATOR)
{
    //setFlag(QGraphicsTextItem::ItemIsSelectable, true);
//    setFlag(ItemIsFocusable);
    mLabel = new Label(vertex->toString(), this);
    //setHandlesChildEvents(false);
    //mLabel->setTextInteractionFlags(Qt::TextEditorInteraction);
    //mLabel->setParentItem(this);
    //mLabel->setTextInteractionFlags(Qt::TextEditorInteraction);
    //mLabel->setFlag(QGraphicsItem::ItemIsSelectable, true);
    //mLabel->setZValue(-100.0);
    setFlag(ItemIsMovable);

    mpBoard = new QGraphicsWidget(this);
    mpBoard->setAcceptHoverEvents(true);
    QRectF rect = boundingRect();
    mpBoard->resize(rect.width(), rect.height());
}

void Resource::recomputeMaxInputPortWidth(void)
{
    Labels::iterator it = mLabels.begin();
#ifdef MMAXINPUTOUTPUTPORTWIDTH
    qDebug("Resource::recomputeMaxInputPortWidth (@cluster='%s'): mMaxInputPortWidth = %lf", mpVertex->getLabel().c_str(), mMaxInputPortWidth);
#endif
    mMaxInputPortWidth = 0;
#ifdef MMAXINPUTOUTPUTPORTWIDTH
    qDebug("Resource::recomputeMaxInputPortWidth (@cluster='%s'): mMaxInputPortWidth = %lf", mpVertex->getLabel().c_str(), mMaxInputPortWidth);
#endif
    for(++it; mLabels.end() != it; ++it)
    {
        Label *label = it->second;
        graph_analysis::Vertex::Ptr current_port = mVertices[it->first];
        if("graph_analysis::InputPortVertex" == current_port->getClassName())
        {
            qreal current_width = label->boundingRect().width();
            if(mMaxInputPortWidth < current_width)
            {
#ifdef MMAXINPUTOUTPUTPORTWIDTH
                qDebug("Resource::recomputeMaxInputPortWidth (@cluster='%s'): mMaxInputPortWidth = %lf", mpVertex->getLabel().c_str(), mMaxInputPortWidth);
#endif
                mMaxInputPortWidth = current_width;
#ifdef MMAXINPUTOUTPUTPORTWIDTH
                qDebug("Resource::recomputeMaxInputPortWidth (@cluster='%s'): mMaxInputPortWidth = %lf", mpVertex->getLabel().c_str(), mMaxInputPortWidth);
#endif
            }
        }
    }
}

void Resource::recomputeMaxOutputPortWidth(void)
{
    Labels::iterator it = mLabels.begin();
#ifdef MMAXINPUTOUTPUTPORTWIDTH
    qDebug("Resource::recomputeMaxOutputPortWidth (@cluster='%s'): mMaxOutputPortWidth = %lf", mpVertex->getLabel().c_str(), mMaxOutputPortWidth);
#endif
    mMaxOutputPortWidth = 0;
#ifdef MMAXINPUTOUTPUTPORTWIDTH
    qDebug("Resource::recomputeMaxOutputPortWidth (@cluster='%s'): mMaxOutputPortWidth = %lf", mpVertex->getLabel().c_str(), mMaxOutputPortWidth);
#endif
    for(++it; mLabels.end() != it; ++it)
    {
        Label *label = it->second;
        graph_analysis::Vertex::Ptr current_port = mVertices[it->first];
        if("graph_analysis::OutputPortVertex" == current_port->getClassName())
        {
            qreal current_width = label->boundingRect().width();
            if(mMaxOutputPortWidth < current_width)
            {
#ifdef MMAXINPUTOUTPUTPORTWIDTH
                qDebug("Resource::recomputeMaxOutputPortWidth (@cluster='%s'): mMaxOutputPortWidth = %lf", mpVertex->getLabel().c_str(), mMaxOutputPortWidth);
#endif
                mMaxOutputPortWidth = current_width;
#ifdef MMAXINPUTOUTPUTPORTWIDTH
                qDebug("Resource::recomputeMaxOutputPortWidth (@cluster='%s'): mMaxOutputPortWidth = %lf", mpVertex->getLabel().c_str(), mMaxOutputPortWidth);
#endif
            }
        }
    }
}

void Resource::setPortLabel(NodeItem::portID_t portID, const std::string& label)
{
    dieOnPort(portID, "setPortLabel");
    graph_analysis::Vertex::Ptr port = mVertices[portID];
    port->setLabel(label);
    Label *port_label = mLabels[portID];
    qreal pre_width = port_label->boundingRect().width();
    port_label->setPlainText(QString(label.c_str()));
    qreal post_width = port_label->boundingRect().width();
    if("graph_analysis::InputPortVertex" == port->getClassName())
    {
        if(post_width > mMaxInputPortWidth)
        {
#ifdef MMAXINPUTOUTPUTPORTWIDTH
            qDebug("Resource::setPortLabel (@cluster='%s'): mMaxInputPortWidth = %lf", mpVertex->getLabel().c_str(), mMaxInputPortWidth);
#endif
            mMaxInputPortWidth = post_width;
#ifdef MMAXINPUTOUTPUTPORTWIDTH
            qDebug("Resource::setPortLabel (@cluster='%s'): mMaxInputPortWidth = %lf", mpVertex->getLabel().c_str(), mMaxInputPortWidth);
#endif
        }
        else if(abs(pre_width - mMaxInputPortWidth) < EPSILON)
        {
            recomputeMaxInputPortWidth();
        }
    }
    else // ("graph_analysis::OutputPortVertex" == port->getClassName())
    {
        if(post_width > mMaxOutputPortWidth)
        {
#ifdef MMAXINPUTOUTPUTPORTWIDTH
    qDebug("Resource::setPortLabel (@cluster='%s'): mMaxOutputPortWidth = %lf", mpVertex->getLabel().c_str(), mMaxOutputPortWidth);
#endif
            mMaxOutputPortWidth = post_width;
#ifdef MMAXINPUTOUTPUTPORTWIDTH
    qDebug("Resource::setPortLabel (@cluster='%s'): mMaxOutputPortWidth = %lf", mpVertex->getLabel().c_str(), mMaxOutputPortWidth);
#endif
        }
        else if(abs(pre_width - mMaxOutputPortWidth) < EPSILON)
        {
            recomputeMaxOutputPortWidth();
        }
    }
}

void Resource::changeLabel(const std::string& label)
{
    mpVertex->setLabel(label);
    qreal pre_width = mLabel->boundingRect().width();
    mLabel->setPlainText(QString(label.c_str()));
    qreal post_width = mLabel->boundingRect().width();
    if(post_width > pre_width)
    {
        updateWidth();
        this->itemChange(QGraphicsItem::ItemPositionHasChanged, QVariant());
    }
    else if(!mLabels.size())
    {
        this->itemChange(QGraphicsItem::ItemPositionHasChanged, QVariant());
    }
}

void Resource::updateLabel()
{
    qreal pre_width = mLabel->boundingRect().width();
    mLabel->setPlainText(QString(mpVertex->getLabel().c_str()));
    qreal post_width = mLabel->boundingRect().width();
    if(post_width > pre_width)
    {
        updateWidth();
        this->itemChange(QGraphicsItem::ItemPositionHasChanged, QVariant());
    }
    else if(!mLabels.size())
    {
        this->itemChange(QGraphicsItem::ItemPositionHasChanged, QVariant());
    }
}

QRectF Resource::boundingRect() const
{
    //QRectF boundingRect( -10 - adjust, -10 - adjust, 23 + adjust, 23 + adjust);
    //return childrenBoundingRect() | boundingRect;
    QRectF childrenRect = childrenBoundingRect();
    return childrenRect;
}

QPainterPath Resource::shape() const
{
    QPainterPath path;
    path.addRect(boundingRect());
    return path;
}

void Resource::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* )
{
    // Draws fully filled item
    //painter->setPen(Qt::NoPen);
    //painter->setBrush(mPen.brush());
    //painter->drawEllipse(-7, -7, 20, 20);
    painter->setPen(QPen(Qt::black, 0));
    updateWidth(false); // in case the main label change triggered redrawing
//    qreal rect_width, max_width = mLabel->boundingRect().width();
    QRectF rect;
    foreach(Tuple tuple, mLabels)
    {
        rect = portBoundingRect(tuple.first);
        painter->drawRect(rect);
    }
    // Drawing of border: back to transparent background
    painter->setPen(mPen);
    rect = boundingRect();
    painter->drawRect(rect); //-7,-7,20,20);
//    QRadialGradient gradient(-3, -3, 10);
//    if (option->state & QStyle::State_Sunken)
//    {
//        gradient.setCenter(3, 3);
//        gradient.setFocalPoint(3, 3);
//        gradient.setColorAt(1, QColor(Qt::yellow).light(120));
//        gradient.setColorAt(0, QColor(Qt::darkYellow).light(120));
//    } else {
//        gradient.setColorAt(0, Qt::yellow);
//        gradient.setColorAt(1, Qt::darkYellow);
//    }
//    painter->setBrush(gradient);
//
//    painter->setPen(QPen(Qt::black, 0));
//    painter->drawEllipse(-10, -10, 20, 20);

    mpBoard->resize(rect.width(), rect.height());
    this->update(rect);
    this->itemChange(QGraphicsItem::ItemPositionHasChanged, QVariant());
}

void Resource::shiftOutports(qreal delta)
{
    if(0. == delta)
    {
        return;
    }
    Labels::iterator it = mLabels.begin();
    for(++it; mLabels.end() != it; ++it)
    {
        Label *label = it->second;
        graph_analysis::Vertex::Ptr current_port = mVertices[it->first];
        if("graph_analysis::OutputPortVertex" == current_port->getClassName())
        {
            label->setPos(label->pos() + QPointF(delta, 0.));
        }
    }
}

void Resource::updateHeight()
{
    QRectF rect = boundingRect();
    int slotCount = 1; // one for the node label
    int nslots = max(mInPorts, mOutPorts);
    if(nslots)
    {
        slotCount += 1 + nslots; // the n ports and the pad between them and the node label
    }
    rect.setHeight(slotCount * ADJUST);
    mpBoard->resize(rect.size());
    this->update(rect);
    this->itemChange(QGraphicsItem::ItemPositionHasChanged, QVariant());
}

void Resource::updateWidth(bool active)
{
    qreal max_width = mLabel->boundingRect().width();
#ifdef MMAXINPUTOUTPUTPORTWIDTH
            qDebug("Resource::updateWidth (@cluster='%s'): mMaxInputPortWidth = %lf", mpVertex->getLabel().c_str(), mMaxInputPortWidth);
            qDebug("Resource::updateWidth (@cluster='%s'): mMaxOutputPortWidth = %lf", mpVertex->getLabel().c_str(), mMaxOutputPortWidth);
#endif
    qreal ports_width = mMaxInputPortWidth + mMaxOutputPortWidth;
    if(ports_width > 0)
    {
        if(SEPARATOR + ports_width < max_width)
        {
            qreal separator = mSeparator;
#ifdef MSEPARATOR
            qDebug("Resource::updateWidth (@cluster='%s'): mSeparator was = %lf", mpVertex->getLabel().c_str(), mSeparator);
#endif
            mSeparator = max_width - ports_width;
#ifdef MSEPARATOR
            qDebug("Resource::updateWidth (@cluster='%s'): mSeparator now is = %lf", mpVertex->getLabel().c_str(), mSeparator);
#endif
            shiftOutports(mSeparator - separator);
        }
        else
        {
            max_width = ports_width + SEPARATOR;
            qreal separator = mSeparator;
#ifdef MSEPARATOR
            qDebug("Resource::updateWidth (@cluster='%s'): mSeparator was = %lf", mpVertex->getLabel().c_str(), mSeparator);
#endif
            mSeparator = SEPARATOR;
#ifdef MSEPARATOR
            qDebug("Resource::updateWidth (@cluster='%s'): mSeparator now is (default) = %lf", mpVertex->getLabel().c_str(), mSeparator);
#endif
            shiftOutports(mSeparator - separator);
        }
    }
    if(active)
    {
        QRectF rect = boundingRect();
        rect.setWidth(max_width);
        mpBoard->resize(rect.size());
        this->update(rect);
        this->itemChange(QGraphicsItem::ItemPositionHasChanged, QVariant());
    }
}

NodeItem::portID_t Resource::addPort(Vertex::Ptr node)
{
    if(!
        (
            "graph_analysis::InputPortVertex"   == node->getClassName()
                ||
            "graph_analysis::OutputPortVertex"  == node->getClassName()
        )
    )
    {
        std::string error_msg = std::string("graph_analysis::gui::graphitem::Resource::addPort: provided port node is not of a sub-type of PortVertex, but of inadmissible type '")
                                + node->getClassName() + "'!";
        LOG_ERROR_S << error_msg;
        throw std::runtime_error(error_msg);
    }
    Label *label = new Label(node->getLabel(), this, mpGraphWidget, mID);
    mLabels[mID] = label;
    mVertices[mID] = node;
    bool isInputPort = "graph_analysis::InputPortVertex" == node->getClassName();
    qreal width = label->boundingRect().width();
    if(isInputPort)
    {
        qDebug("Resource::addPort (@cluster='%s'): adding (In)port '%s'", mpVertex->getLabel().c_str(), node->getLabel().c_str());
        if(width > mMaxInputPortWidth)
        {
#ifdef MMAXINPUTOUTPUTPORTWIDTH
            qDebug("Resource::addPort (@cluster='%s'): mMaxInputPortWidth = %lf", mpVertex->getLabel().c_str(), mMaxInputPortWidth);
#endif
            mMaxInputPortWidth = width;
#ifdef MMAXINPUTOUTPUTPORTWIDTH
            qDebug("Resource::addPort (@cluster='%s'): mMaxInputPortWidth = %lf", mpVertex->getLabel().c_str(), mMaxInputPortWidth);
#endif
        }
        if(mInPorts + 1 < 0)
        {
            LOG_WARN_S << "graph_analysis::gui::grapitem::Resource::addPort: input ports counter overflowed";
            mInPorts = 0;
        }
        label->setPos(mLabel->pos() + QPointF(0., qreal(1 + (++mInPorts)) * ADJUST));
        updateWidth();
    }
    else
    {
        qDebug("Resource::addPort (@cluster='%s'): adding (Out)port '%s'", mpVertex->getLabel().c_str(), node->getLabel().c_str());
        if(width > mMaxOutputPortWidth)
        {
#ifdef MMAXINPUTOUTPUTPORTWIDTH
    qDebug("Resource::addPort (@cluster='%s'): mMaxOutputPortWidth = %lf", mpVertex->getLabel().c_str(), mMaxOutputPortWidth);
#endif
            mMaxOutputPortWidth = width;
#ifdef MMAXINPUTOUTPUTPORTWIDTH
    qDebug("Resource::addPort (@cluster='%s'): mMaxOutputPortWidth = %lf", mpVertex->getLabel().c_str(), mMaxOutputPortWidth);
#endif
        }
        if(mOutPorts + 1 < 0)
        {
            LOG_WARN_S << "graph_analysis::gui::grapitem::Resource::addPort: output ports counter overflowed";
            mOutPorts = 0;
        }
        label->setPos(mLabel->pos() + QPointF(mMaxInputPortWidth + mSeparator, qreal(1 + (++mOutPorts)) * ADJUST));
#ifdef MSEPARATOR
        qDebug("Resource::addPort (@cluster='%s'): made use of mSeparator = %lf", mpVertex->getLabel().c_str(), mSeparator);
#endif
        updateWidth();
    }
    NodeItem::portID_t portID = mID;
    // test if the IDs overflowed
    if(++mID < 0)
    {
        LOG_WARN_S << "graph_analysis::gui::grapitem::Resource::addPort: port IDs counter overflowed";
        mID = 0;
    }
    return portID; // returning this port's offset in the vector of ports
}

void Resource::removePort(NodeItem::portID_t portID)
{
    int nports = mLabels.size();
    if(!nports)
    {
        std::string error_msg = std::string("graph_analysis::gui::graphitem::Resource::removePort: node '")
                                        + getLabel()
                                        + "' has no ports whatsoever!";
        LOG_ERROR_S << error_msg;
        throw std::runtime_error(error_msg);
    }
    dieOnPort(portID, "removePort");
    graphitem::Label *label_to_delete = mLabels[portID];
    graph_analysis::Vertex::Ptr port_to_delete = mVertices[portID];
    // shifting up all ports initially under the port-to-be-removed
    prepareGeometryChange();
    bool isInputPort = "graph_analysis::InputPortVertex" == port_to_delete->getClassName();
    if(isInputPort)
    {
        bool maxInputPortWidthIsDirty = label_to_delete->boundingRect().width() == mMaxInputPortWidth;
        Labels::iterator it = mLabels.begin();
        for(++it; mLabels.end() != it; ++it)
        {
            Label *label = it->second;
            graph_analysis::Vertex::Ptr current_port = mVertices[it->first];
            if("graph_analysis::InputPortVertex" == current_port->getClassName() && label->pos().y() > label_to_delete->pos().y())
            {
                label->setPos(label->pos() - QPointF(0., ADJUST));
            }
        }
        mLabels.erase(mLabels.find(portID));
        mVertices.erase(mVertices.find(portID));
        removeFromGroup(label_to_delete);
        scene()->removeItem(label_to_delete);
        if(maxInputPortWidthIsDirty)
        {
            recomputeMaxInputPortWidth();
        }
        if(--mInPorts >= mOutPorts)
        {
            updateHeight();
        }
    }
    else // "graph_analysis::OutputPortVertex" == port_to_delete->getClassName();
    {
        bool maxOutputPortWidthIsDirty = label_to_delete->boundingRect().width() == mMaxOutputPortWidth;
        Labels::iterator it = mLabels.begin();
        for(++it; mLabels.end() != it; ++it)
        {
            Label *label = it->second;
            graph_analysis::Vertex::Ptr current_port = mVertices[it->first];
            if("graph_analysis::OutputPortVertex" == current_port->getClassName() && label->pos().y() > label_to_delete->pos().y())
            {
                label->setPos(label->pos() - QPointF(0., ADJUST));
            }
        }
        mLabels.erase(mLabels.find(portID));
        mVertices.erase(mVertices.find(portID));
        removeFromGroup(label_to_delete);
        scene()->removeItem(label_to_delete);
        if(maxOutputPortWidthIsDirty)
        {
            recomputeMaxOutputPortWidth();
        }
        if(--mOutPorts >= mInPorts)
        {
            updateHeight();
        }
    }
    this->update();
}

void Resource::swapPorts(NodeItem::portID_t port1, NodeItem::portID_t port2)
{
    dieOnPort(port1, "swapPorts");
    dieOnPort(port2, "swapPorts");

    // swapping GUI labels
#ifdef LABEL_SWAPPING
    QPointF pos1 = mLabels[port1]->pos();
    QPointF pos2 = mLabels[port2]->pos();
    mLabels[port1]->setPos(pos2);
    mLabels[port2]->setPos(pos1);
    this->itemChange(QGraphicsItem::ItemPositionHasChanged, QVariant());
#else
    QString str_swap = mLabels[port1]->toPlainText();
    mLabels[port1]->setPlainText(mLabels[port2]->toPlainText());
    mLabels[port2]->setPlainText(str_swap);
#endif
}

void Resource::removePorts()
{
    foreach(Tuple tuple, mLabels)
    {
        graphitem::Label *label = tuple.second;
        this->removeFromGroup(label);
        scene()->removeItem(label);
    }
    mLabels.clear();
    mVertices.clear();
    mInPorts = 0;
    mOutPorts = 0;
#ifdef MMAXINPUTOUTPUTPORTWIDTH
    qDebug("Resource::removePorts (@cluster='%s'): mMaxInputPortWidth = %lf", mpVertex->getLabel().c_str(), mMaxInputPortWidth);
    qDebug("Resource::removePorts (@cluster='%s'): mMaxOutputPortWidth = %lf", mpVertex->getLabel().c_str(), mMaxOutputPortWidth);
#endif
    mMaxInputPortWidth = 0.;
    mMaxOutputPortWidth = 0.;
#ifdef MMAXINPUTOUTPUTPORTWIDTH
    qDebug("Resource::removePorts (@cluster='%s'): mMaxInputPortWidth = %lf", mpVertex->getLabel().c_str(), mMaxInputPortWidth);
    qDebug("Resource::removePorts (@cluster='%s'): mMaxOutputPortWidth = %lf", mpVertex->getLabel().c_str(), mMaxOutputPortWidth);
#endif
    mpBoard->resize(mLabel->boundingRect().size());
    update();
}

graph_analysis::Vertex::Ptr Resource::getPort(NodeItem::portID_t portID)
{
    dieOnPort(portID, "getPort");
    return mVertices[portID];
}

void Resource::syncLabel(NodeItem::portID_t portID)
{
    if(-1 == portID)
    {
        std::string label = mLabel->toPlainText().toStdString();
        if(mpVertex->getLabel() != label)
        {
            mpVertex->setLabel(label);
            updateWidth();
        }
        return;
    }
    dieOnPort(portID, "syncLabel");
    Label *label = mLabels[portID];
    graph_analysis::Vertex::Ptr port = mVertices[portID];
    std::string tag = label->toPlainText().toStdString();
    if(port->getLabel() != tag)
    {
        port->setLabel(tag);
        qreal width = label->boundingRect().width();
        qDebug("Resource::syncLabel (@cluster='%s'): 1. Syncing label '%s' of width = %lf", mpVertex->getLabel().c_str(), tag.c_str(), width);
        if("graph_analysis::InputPortVertex" == port->getClassName())
        {
            qDebug("Resource::syncLabel (@cluster='%s'): 2. Item is of type 'graph_analysis::InputPortVertex'", mpVertex->getLabel().c_str());
            if(width > mMaxInputPortWidth)
            {
                qDebug("Resource::syncLabel (@cluster='%s'): 3. mMaxInputPortWidth was '%lf' -> updated it to be '%lf'", mpVertex->getLabel().c_str(), mMaxInputPortWidth, width);
#ifdef MMAXINPUTOUTPUTPORTWIDTH
    qDebug("Resource::syncLabel (@cluster='%s'): mMaxInputPortWidth = %lf", mpVertex->getLabel().c_str(), mMaxInputPortWidth);
#endif
                mMaxInputPortWidth = width;
#ifdef MMAXINPUTOUTPUTPORTWIDTH
    qDebug("Resource::syncLabel (@cluster='%s'): mMaxInputPortWidth = %lf", mpVertex->getLabel().c_str(), mMaxInputPortWidth);
#endif
            }
            else // there is no way to know it wasn't max before - forcing recalculation of max width nevertheless
            {
                qDebug("Resource::syncLabel (@cluster='%s'): 3. else: mMaxInputPortWidth = %lf", mpVertex->getLabel().c_str(), mMaxInputPortWidth);
                recomputeMaxInputPortWidth();
                qDebug("Resource::syncLabel (@cluster='%s'): 3. else: mMaxInputPortWidth recomputed as '%lf'", mpVertex->getLabel().c_str(), mMaxInputPortWidth);
            }
        }
        else // ("graph_analysis::OutputPortVertex" == port->getClassName())
        {
            if(width > mMaxOutputPortWidth)
            {
#ifdef MMAXINPUTOUTPUTPORTWIDTH
    qDebug("Resource::syncLabel (@cluster='%s'): mMaxOutputPortWidth = %lf", mpVertex->getLabel().c_str(), mMaxOutputPortWidth);
#endif
                mMaxOutputPortWidth = width;
#ifdef MMAXINPUTOUTPUTPORTWIDTH
    qDebug("Resource::syncLabel (@cluster='%s'): mMaxOutputPortWidth = %lf", mpVertex->getLabel().c_str(), mMaxOutputPortWidth);
#endif
            }
            else // there is no way to know it wasn't max before - forcing recalculation of max width nevertheless
            {
                recomputeMaxOutputPortWidth();
            }
        }
        updateWidth();
        update();
    }
}

QRectF Resource::portBoundingRect(NodeItem::portID_t portID)
{
    dieOnPort(portID, "portBoundingRect");
    QRectF result = boundingRect();
    Labels::iterator it = mLabels.find(portID);
    graph_analysis::Vertex::Ptr current_port = mVertices[it->first];
    bool isInputPort = "graph_analysis::InputPortVertex" == current_port->getClassName();
//#ifndef LABEL_SWAPPING
//    int offset = std::distance(mLabels.begin(), it);
//    result.adjust(
//                    isInputPort ? 0. : mMaxInputPortWidth + mSeparator,
//                    qreal(2 + offset) * ADJUST,
//                    isInputPort ? - (mSeparator + mMaxOutputPortWidth) : 0.,
//                    qreal(3 + offset) * ADJUST - result.height()
//                ); // forward enumeration
//#ifdef MSEPARATOR
//    qDebug("Resource::portBoundingRect (@cluster='%s'): made use of mSeparator = %lf", mpVertex->getLabel().c_str(), mSeparator);
//#endif
//#else
    qreal offset = mLabels[portID]->pos().y() - mLabel->pos().y();
    result.adjust(
                    isInputPort ? 0. : mMaxInputPortWidth + mSeparator,
                    offset,
                    isInputPort ? - (mSeparator + mMaxOutputPortWidth) : 0.,
                    offset + ADJUST - result.height()
                ); // forward enumeration
#ifdef MSEPARATOR
    qDebug("Resource::portBoundingRect (@cluster='%s'): made use of mSeparator = %lf", mpVertex->getLabel().c_str(), mSeparator);
#endif
//#endif
    return result;
}

QPolygonF Resource::portBoundingPolygon(NodeItem::portID_t portID)
{
    return QPolygonF(portBoundingRect(portID));
}

void Resource::mousePressEvent(::QGraphicsSceneMouseEvent* event)
{
    LOG_DEBUG_S << "Mouse RESOURCE: press";
    QGraphicsItem::mousePressEvent(event);
}

void Resource::mouseReleaseEvent(::QGraphicsSceneMouseEvent* event)
{
    LOG_DEBUG_S << "Mouse RESOURCE: release";
    QGraphicsItem::mouseReleaseEvent(event);
}

void Resource::grabFocus()
{
    mpGraphWidget->clearNodeFocus();
    mPen = QPen(Qt::red);
    update();
    mpGraphWidget->setVertexFocused(true);
    mFocused = true;
    mpGraphWidget->setFocusedVertex(mpVertex);
}

void Resource::mouseDoubleClickEvent(::QGraphicsSceneMouseEvent* event)
{
    mFocused ? releaseFocus() : grabFocus();
    QGraphicsItem::mouseDoubleClickEvent(event);
}

void Resource::releaseFocus()
{
    mPen = mSelected ? QPen(Qt::green) : mPenDefault;
    update();
    mFocused = false;
    mpGraphWidget->setVertexFocused(false);
}

void Resource::focusInEvent(QFocusEvent* event)
{
    LOG_DEBUG_S << "Focus-In event for %s" << mpVertex->toString();
    ::QGraphicsItemGroup::focusInEvent(event);
}

void Resource::focusOutEvent(QFocusEvent* event)
{
    LOG_DEBUG_S << "Focus-Out event for %s" << mpVertex->toString();
    ::QGraphicsItemGroup::focusOutEvent(event);
}

void Resource::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    qDebug("Hover ENTER event for %s", mpVertex->toString().c_str());
    if(!mFocused)
    {
        mPen = QPen(Qt::green);
    }
    mSelected = true;
    mpGraphWidget->setSelectedVertex(mpVertex);
    mpGraphWidget->setVertexSelected(true);
    QGraphicsItem::hoverEnterEvent(event);
}

void Resource::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
    qDebug("Hover LEAVE event for %s", mpVertex->toString().c_str());
    if(!mFocused)
    {
        mPen = mPenDefault;
    }
    mSelected = false;
    mpGraphWidget->setVertexSelected(false);
    QGraphicsItem::hoverLeaveEvent(event);
}

void Resource::dieOnPort(NodeItem::portID_t portID, const std::string& caller)
{
    if(mLabels.count(portID))
    {
        return;
    }

    std::string method = ("" == caller) ? "die" : caller;
    std::string error_msg = std::string("graph_analysis::gui::graphitem::Resource::") + method + ": the supplied portID: "
                                    + boost::lexical_cast<std::string>(portID)
                                    + " is not registered";
    LOG_ERROR_S << error_msg;
    throw std::runtime_error(error_msg);
}

void Resource::unselect()
{
    hoverLeaveEvent(new QGraphicsSceneHoverEvent());
}

void Resource::shiftPortUp(NodeItem::portID_t portID)
{
    Labels::iterator tuple = mLabels.find(portID);
    if(mLabels.end() == tuple)
    {
        dieOnPort(portID, "shiftPortUp");
    }

    const qreal y = tuple->second->pos().y();
    qreal delta = y - mLabel->pos().y();
    if(abs(delta - 2. * ADJUST) < EPSILON) // safety belt against loss in floating point precision
    {
        return;
    }
    // iterating for closest upper label
    foreach(Tuple tuple, mLabels)
    {
        delta = y - tuple.second->pos().y();
        if(abs(delta - ADJUST) < EPSILON)
        {
            swapPorts(tuple.first, portID);
            return;
        }
    }
}

void Resource::shiftPortDown(NodeItem::portID_t portID)
{
    Labels::iterator tuple = mLabels.find(portID);
    if(mLabels.end() == tuple)
    {
        dieOnPort(portID, "shiftPortDown");
    }

    const qreal y = tuple->second->pos().y();
    qreal delta = y - mLabel->pos().y();
    if(abs(delta - (double)(1 + mLabels.size()) * ADJUST) < EPSILON) // safety belt against loss in floating point precision
    {
        return;
    }
    // iterating for closest lower label
    foreach(Tuple tuple, mLabels)
    {
        delta = tuple.second->pos().y() - y;
        if(abs(delta - ADJUST) < EPSILON)
        {
            swapPorts(tuple.first, portID);
            return;
        }
    }
}

//void Resource::keyPressEvent(QKeyEvent* event)
//{
//    LOG_DEBUG_S << "Key RESOURCE: press";
//}

} // end namespace graphitem
} // end namespace gui
} // end namespace graph_analysis
