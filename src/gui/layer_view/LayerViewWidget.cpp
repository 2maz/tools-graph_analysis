#include "LayerViewWidget.hpp"

#include <exception>
#include <set>
#include <math.h>
#include <string>
#include <sstream>

#include <QDir>
#include <QTime>
#include <QMenu>
#include <QAction>
#include <QKeyEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QApplication>
#include <QInputDialog>
#include <QSignalMapper>

#include <boost/foreach.hpp>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>
#include <base/Time.hpp>
#include <base/Logging.hpp>

#include <graph_analysis/io/GVGraph.hpp>
#include <graph_analysis/filters/EdgeContextFilter.hpp>
#include <graph_analysis/filters/RegexFilters.hpp>

#include <graph_analysis/gui/layeritem/Resource.hpp>
#include <graph_analysis/gui/layeritem/edges/Simple.hpp>
#include <graph_analysis/gui/NodeItemTypeManager.hpp>
#include <graph_analysis/gui/EdgeItemTypeManager.hpp>


using namespace graph_analysis;

namespace graph_analysis {
namespace gui {

LayerViewWidget::LayerViewWidget(QWidget *parent)
    : GraphWidget(getName(), parent)
{
    QGraphicsScene *scene = new QGraphicsScene(this);
    scene->setItemIndexMethod(QGraphicsScene::NoIndex);
    setScene(scene);

    setCacheMode(CacheBackground);
    setContextMenuPolicy(Qt::CustomContextMenu);
    setViewportUpdateMode(BoundingRectViewportUpdate);
    setRenderHint(QPainter::Antialiasing);
    setTransformationAnchor(AnchorUnderMouse);
    scale(qreal(0.8), qreal(0.8));
    setMinimumSize(400, 400);
    setWindowTitle(tr("Graphview"));
    // Setting up filtering
    mGraphView.setVertexFilter(mpVertexFilter);
    mGraphView.setEdgeFilter(mpEdgeFilter);
    // End of setting up filters

    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)),
        this, SLOT(showContextMenu(const QPoint &)));
}

LayerViewWidget::~LayerViewWidget()
{}

void LayerViewWidget::showContextMenu(const QPoint& pos)
{
/*    ActionCommander comm(WidgetManager::getInstance()->getComponentEditorWidget());
    QMenu contextMenu(tr("Context menu"), this);

    QAction *actionRefresh = comm.addAction("Refresh", SLOT(refresh()), *(IconManager::getInstance()->getIcon("refresh")), this);
    QAction *actionShuffle = comm.addAction("Shuffle", SLOT(shuffle()), *(IconManager::getInstance()->getIcon("shuffle")), this);
    QAction *actionImport  = comm.addAction("Import", SLOT(importGraph()), *(IconManager::getInstance()->getIcon("import")));
    QAction *actionExport  = comm.addAction("Export", SLOT(exportGraph()), *(IconManager::getInstance()->getIcon("export")));
    QAction *actionReset   = comm.addAction("Reset", SLOT(resetGraph()), *(IconManager::getInstance()->getIcon("reset")));
    QAction *actionLayout  = comm.addAction("Layout", SLOT(changeLayout()), *(IconManager::getInstance()->getIcon("layout")), this);
    QAction *actionReloadPropertyDialog = comm.addAction("Reload Command Panel", SLOT(reloadPropertyDialog()), *(IconManager::getInstance()->getIcon("reload")), WidgetManager::getInstance()->getGraphWidgetManager());

    contextMenu.addAction(actionImport);
    contextMenu.addAction(actionExport);
    contextMenu.addSeparator();
    contextMenu.addAction(actionRefresh);
    contextMenu.addAction(actionShuffle);
    contextMenu.addAction(actionReset);
    contextMenu.addAction(actionLayout);
    if(!WidgetManager::getInstance()->getPropertyDialog()->isRunning())
    {
        contextMenu.addSeparator();
        contextMenu.addAction(actionReloadPropertyDialog);
    }
    contextMenu.exec(mapToGlobal(pos));
    */
}

void LayerViewWidget::enableVertex(graph_analysis::Vertex::Ptr vertex)
{
    mpSubGraph->enable(vertex);
    LOG_DEBUG_S << "Enabled a vertex of ID: " << mpSubGraph->getBaseGraph()->getVertexId(vertex);
}
void LayerViewWidget::enableEdge(graph_analysis::Edge::Ptr edge)
{
    mpSubGraph->enable(edge);
    LOG_DEBUG_S << "Enabled an edge of ID:  " << mpSubGraph->getBaseGraph()->getEdgeId(edge);
}

void LayerViewWidget::disableVertex(graph_analysis::Vertex::Ptr vertex)
{
    mpSubGraph->disable(vertex);
    LOG_DEBUG_S << "Disabled vertex '" << vertex->getLabel() << "' of ID: " << mpSubGraph->getBaseGraph()->getVertexId(vertex);
}
void LayerViewWidget::disableEdge(graph_analysis::Edge::Ptr edge)
{
    mpSubGraph->disable(edge);
    LOG_DEBUG_S << "Disabled edge '" << edge->getLabel() << "' of ID:  " << mpSubGraph->getBaseGraph()->getEdgeId(edge);
}

void LayerViewWidget::updateLayout()
{
    VertexIterator::Ptr nodeIt = mpGraph->getVertexIterator();
    while(nodeIt->next())
    {
        Vertex::Ptr vertex = nodeIt->current();

        // Registering new node items
        NodeItem* nodeItem = NodeItemTypeManager::getInstance()->createItem(this, vertex, layeritem::Resource::sType);
        mNodeItemMap[vertex] = nodeItem;
        scene()->addItem(nodeItem);

        mpLayoutingGraph->addVertex(vertex);
        mpGVGraph->addNode(vertex);
    }

    EdgeIterator::Ptr edgeIt = mpGraph->getEdgeIterator();
    while(edgeIt->next())
    {
        Edge::Ptr edge = edgeIt->current();

        Vertex::Ptr source = edge->getSourceVertex();
        Vertex::Ptr target = edge->getTargetVertex();

        NodeItem* sourceNodeItem = mNodeItemMap[ source ];
        NodeItem* targetNodeItem = mNodeItemMap[ target ];

        if(!sourceNodeItem || !targetNodeItem)
        {
            continue;
        }

        EdgeItem* edgeItem = EdgeItemTypeManager::getInstance()->createItem(this, sourceNodeItem, targetNodeItem, edge, LAYER_EDGE_TYPE);
        mEdgeItemMap[edge] = edgeItem;

        scene()->addItem(edgeItem);

        mpLayoutingGraph->addEdge(edge);
        mpGVGraph->addEdge(edge);
    }

}

void LayerViewWidget::itemMoved()
{
    if (!mTimerId)
    {
        mTimerId = startTimer(1000 / 25);
    }
}

void LayerViewWidget::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event);

    if(mLayout.toLower() == "force")
    {
        QList<NodeItem* > nodes;
        foreach (QGraphicsItem *item, scene()->items())
        {
            if (NodeItem* node = qgraphicsitem_cast<NodeItem* >(item))
            {
                nodes << node;
            }
        }

        foreach (NodeItem* node, nodes)
        {
            node->calculateForces();
        }

        bool itemsMoved = false;
        foreach (NodeItem* node, nodes)
        {
            if (node->advance())
                itemsMoved = true;
        }

        if (!itemsMoved)
        {
            killTimer(mTimerId);
            mTimerId = 0;
        }
    }
}

void LayerViewWidget::wheelEvent(QWheelEvent *event)
{
    scaleView(pow((double)2, - event->delta() / 240.0));
}

void LayerViewWidget::scaleView(qreal scaleFactor)
{
    qreal factor = transform().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
    if (factor < 0.07 || factor > 100)
    {
        return;
    }
    scale(scaleFactor, scaleFactor);
    std::string status_msg = scaleFactor > 1. ? "Zoomed-in" : "Zoomed-out";
    updateStatus(status_msg, GraphWidgetManager::TIMEOUT);
}

void LayerViewWidget::shuffle()
{
    updateStatus("Shuffelling all the nodes in the layers graph view...");
    foreach (QGraphicsItem *item, scene()->items())
    {
        if (qgraphicsitem_cast<NodeItem* >(item))
            item->setPos(-150 + qrand() % 300, -150 + qrand() % 300);
    }
    updateStatus("Shuffelled all nodes in the layers graph view", GraphWidgetManager::TIMEOUT);
}

void LayerViewWidget::zoomIn()
{
    scaleView(qreal(1.13));
}

void LayerViewWidget::zoomOut()
{
    scaleView(1 / qreal(1.13));
}

void LayerViewWidget::resetLayoutingGraph()
{
    mMaxNodeHeight  = 0;
    mMaxNodeWidth   = 0;

    GraphWidget::resetLayoutingGraph();
}


} // end namespace gui
} // end namespace graph_analysis
