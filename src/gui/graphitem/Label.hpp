#ifndef GRAPH_ANALYSIS_GRAPHWIDGET_GRAPHITEM_LABEL_HPP
#define GRAPH_ANALYSIS_GRAPHWIDGET_GRAPHITEM_LABEL_HPP

#include <QGraphicsTextItem>
#include <QTextCursor>

namespace graph_analysis {
namespace gui {
namespace graphitem {

class Label : public QGraphicsTextItem
{
private:
    int mPortID;
public:
    Label(const std::string& label, QGraphicsItem* item, int portID = -1)
        : QGraphicsTextItem( QString(label.c_str()), item)
        , mPortID(portID)
    {
        setFlags(QGraphicsTextItem::ItemIsSelectable | ItemIsFocusable);
        setTextInteractionFlags(Qt::NoTextInteraction);
    }

    void setTextInteraction(bool on, bool selectAll = false)
    {
        if(on && textInteractionFlags() == Qt::NoTextInteraction)
        {
            setTextInteractionFlags(Qt::TextEditorInteraction);
            setFocus(Qt::MouseFocusReason);
            setSelected(true);
            if(selectAll)
            {
                QTextCursor c = textCursor();
                c.select(QTextCursor::Document);
                setTextCursor(c);
            }

        } else if(!on && textInteractionFlags() == Qt::TextEditorInteraction)
        {
            setTextInteractionFlags(Qt::NoTextInteraction);
            QTextCursor c = this->textCursor();
            c.clearSelection();
            this->setTextCursor(c);
            clearFocus();
        }
    }

protected:
    void mouseDoubleClickEvent(::QGraphicsSceneMouseEvent* event)
    {
    //    if(textInteractionFlags() == Qt::TextEditorInteraction)
    //    {
    //        QGraphicsTextItem::mousePressEvent(event);
    //        return;
    //    }

    //    setTextInteraction(true);
    //    QGraphicsTextItem::mouseDoubleClickEvent(event);
    }

    void keyPressEvent(::QKeyEvent* event)
    {
        qDebug("KEYPRESS LABLE");
        QGraphicsTextItem::keyPressEvent(event);
    }

    void focusOutEvent(QFocusEvent* event)
    {
        qDebug("Lost focus");
        ::QGraphicsTextItem::focusOutEvent(event);
    }

    QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant& value)
    {
        if(change == QGraphicsItem::ItemSelectedChange)
        {
             qDebug("itemChange '%s', selected=%s, portID = %d", this->toPlainText().toStdString().c_str(), value.toString().toStdString().c_str(), mPortID);
        }
        if(change == QGraphicsItem::ItemSelectedChange && textInteractionFlags() != Qt::NoTextInteraction && !value.toBool())
        {
            // item received SelectedChange event AND is in editor mode AND is about to be deselected:
            setTextInteraction(false); // leave editor mode
        }
        return QGraphicsTextItem::itemChange(change, value);
    }
};

} // end namespace graphitem
} // end namespace gui
} // end namespace graph_analysis
#endif // GRAPH_ANALYSIS_GRAPHWIDGET_GRAPHITEM_LABEL_HPP
