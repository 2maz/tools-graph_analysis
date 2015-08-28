#ifndef GRAPH_ANALYSIS_GRAPHWIDGET_LAYERITEM_EDGES_EDGELABEL_HPP
#define GRAPH_ANALYSIS_GRAPHWIDGET_LAYERITEM_EDGES_EDGELABEL_HPP

#include <QGraphicsTextItem>
#include <QTextCursor>
#include <base/Logging.hpp>

namespace graph_analysis {
namespace gui {
namespace layeritem {
namespace edges {

/**
 * \file EdgeLabel.hpp
 * \class EdgeLabel
 * \brief graphical edge text label implementation
 * \details specific to layer widget graphical implementation - non-framed non-editable text representation
 */
class EdgeLabel : public QGraphicsTextItem
{
public:
    /**
     * \brief constructor
     * \param label required text label
     * \param item parent graphical edge item
     */
    EdgeLabel(const std::string& label, QGraphicsItem* item)
        : QGraphicsTextItem(QString(label.c_str()), item)
    {
        setFlags(QGraphicsTextItem::ItemIsSelectable | ItemIsFocusable);
        setTextInteractionFlags(Qt::NoTextInteraction);
    }

    /**
     * \brief toggles direct editing options of the displayed text
     * \param on boolean flag - true when the editing mode is to be turned on; false otherwise
     * \param selectAll boolean flag - true when in editing mode the whole text shall be initially automatically selected; false otherwise
     */
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

    /// updates displayed text string to 'label'
    void setText(const QString& label)
    {
        setPlainText(label);
    }

protected:
    /// qt mouse double-click callback
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

    /// qt keys press callback
    void keyPressEvent(::QKeyEvent* event)
    {
        LOG_DEBUG_S << "KEYPRESS LABLE";
        QGraphicsTextItem::keyPressEvent(event);
    }

    /// qt focus LEAVE callback
    void focusOutEvent(QFocusEvent* event)
    {
        LOG_DEBUG_S << "Lost focus";
        ::QGraphicsTextItem::focusOutEvent(event);
    }

    /// qt item change callback
    QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant& value)
    {
        if(change == QGraphicsItem::ItemSelectedChange)
        {
             LOG_DEBUG_S << "itemChange '" << this->toPlainText().toStdString() << "', selected=" << value.toString().toStdString();
        }
        if(change == QGraphicsItem::ItemSelectedChange && textInteractionFlags() != Qt::NoTextInteraction && !value.toBool())
        {
            // item received SelectedChange event AND is in editor mode AND is about to be deselected:
            setTextInteraction(false); // leave editor mode
        }
        return QGraphicsTextItem::itemChange(change, value);
    }
};

} // end namespace edges
} // end namespace layeritem
} // end namespace gui
} // end namespace graph_analysis
#endif // GRAPH_ANALYSIS_GRAPHWIDGET_LAYERITEM_EDGES_EDGELABEL_HPP
