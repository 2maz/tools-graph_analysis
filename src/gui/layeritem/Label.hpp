#ifndef GRAPH_ANALYSIS_GRAPHWIDGET_LAYERITEM_LABEL_HPP
#define GRAPH_ANALYSIS_GRAPHWIDGET_LAYERITEM_LABEL_HPP

#include <QGraphicsTextItem>
#include <QTextCursor>

namespace graph_analysis {
namespace gui {
namespace layeritem {

class Label : public QGraphicsTextItem
{
public:
    Label(const std::string& label, QGraphicsItem* item)
        : QGraphicsTextItem( QString(label.c_str()), item)
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
        LOG_DEBUG_S << "KEYPRESS LABLE";
        QGraphicsTextItem::keyPressEvent(event);
    }

    void focusOutEvent(QFocusEvent* event)
    {
        LOG_DEBUG_S << "Lost focus";
        ::QGraphicsTextItem::focusOutEvent(event);
    }

    QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant& value)
    {
        if(change == QGraphicsItem::ItemSelectedChange)
        {
             LOG_DEBUG_S << "itemChange '" <<  this->toPlainText().toStdString() << "', selected=" << value.toString().toStdString();
        }
        if(change == QGraphicsItem::ItemSelectedChange && textInteractionFlags() != Qt::NoTextInteraction && !value.toBool())
        {
            // item received SelectedChange event AND is in editor mode AND is about to be deselected:
            setTextInteraction(false); // leave editor mode
        }
        return QGraphicsTextItem::itemChange(change, value);
    }
};

} // end namespace layeritem
} // end namespace gui
} // end namespace graph_analysis
#endif // GRAPH_ANALYSIS_GRAPHWIDGET_LAYERITEM_LABEL_HPP
