#include "ComponentEditorWidget.hpp"
#include "EdgeItem.hpp"
#include "NodeItem.hpp"
#include "IconManager.hpp"
#include "GraphManager.hpp"
#include "WidgetManager.hpp"
#include "AddNodeDialog.hpp"
#include "NodeTypeManager.hpp"
#include "EdgeTypeManager.hpp"
#include "ActionCommander.hpp"
#include "SwapFeaturesDialog.hpp"
#include "RenameFeatureDialog.hpp"

#include <set>
#include <math.h>
#include <sstream>
#include <QDir>
#include <QTime>
#include <QMenu>
#include <QLabel>
#include <sstream>
#include <QAction>
#include <QKeyEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QApplication>
#include <QInputDialog>
#include <QSignalMapper>
#include <boost/regex.hpp>
#include <base/Logging.hpp>
#include <boost/lexical_cast.hpp>

#include <graph_analysis/Vertex.hpp>
#include <graph_analysis/BaseVertex.hpp>
#include <graph_analysis/PortVertex.hpp>
#include <graph_analysis/InputPortVertex.hpp>
#include <graph_analysis/OutputPortVertex.hpp>
#include <graph_analysis/ClusterVertex.hpp>
#include <graph_analysis/PropertyVertex.hpp>
#include <graph_analysis/OperationVertex.hpp>

#include <graph_analysis/Filter.hpp>
#include <graph_analysis/io/GVGraph.hpp>
#include <graph_analysis/GraphIO.hpp>
#include <graph_analysis/io/YamlWriter.hpp>
#include <graph_analysis/io/GexfWriter.hpp>
#include <graph_analysis/io/GexfReader.hpp>
#include <graph_analysis/io/YamlReader.hpp>
#include <graph_analysis/io/GraphvizWriter.hpp>
#include <graph_analysis/filters/EdgeContextFilter.hpp>
#include <graph_analysis/gui/graphitem/edges/EdgeLabel.hpp>

#include <exception>
#include <boost/foreach.hpp>
#include <base/Time.hpp>

// comment out to toggle-out focused node be re-doule-clicking it; leave untouched to be able to cancel node focusing by double-clicking the background
#define CLEAR_BY_BACKGROUND

using namespace graph_analysis;

namespace graph_analysis {
namespace gui {

ComponentEditorWidget::ComponentEditorWidget(QWidget *parent)
    : GraphWidget(parent)
    , mpLayoutingGraph()
    , mVertexFocused(false)
    , mEdgeFocused(false)
    , mInitialized(false)
    , mMaxNodeHeight(0)
    , mMaxNodeWidth (0)
    , mDragDrop(false)
{
    // Add seed for force layout
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
    mScaleFactor *= 1.69;

    QGraphicsScene *scene = new QGraphicsScene(this);
    scene->setItemIndexMethod(QGraphicsScene::NoIndex);
    setScene(scene);

    setCacheMode(CacheBackground);
    setContextMenuPolicy(Qt::CustomContextMenu);
    setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);
    setViewportUpdateMode(BoundingRectViewportUpdate);
    setRenderHint(QPainter::Antialiasing);
    setTransformationAnchor(AnchorUnderMouse);
    scale(qreal(0.8), qreal(0.8));
    setMinimumSize(400, 400);
    setWindowTitle(tr("Component Editor"));
    // Setting up filtering
    mGraphView.setVertexFilter(mpVertexFilter);
    mGraphView.setEdgeFilter(mpEdgeFilter);

    // setting up the Reader and WriterMaps
    io::YamlWriter *yamlWriter = new io::YamlWriter();
    mWriterMap["yaml"]  = yamlWriter;
    mWriterMap["yml"]   = yamlWriter;
    io::YamlReader *yamlReader = new io::YamlReader();
    mReaderMap["yaml"]  = yamlReader;
    mReaderMap["yml"]   = yamlReader;
    io::GexfWriter *gexfWriter = new io::GexfWriter();
    mWriterMap["gexf"]  = gexfWriter;
    mWriterMap["xml"]   = gexfWriter;
    io::GexfReader *gexfReader = new io::GexfReader();
    mReaderMap["gexf"]  = gexfReader;
    mReaderMap["xml"]   = gexfReader;
    io::GraphvizWriter *gvWriter = new io::GraphvizWriter();
    mWriterMap["dot"]  = gvWriter;

    // setting up the context menu
    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(showContextMenu(const QPoint &)));
    reset();
}

ComponentEditorWidget::~ComponentEditorWidget()
{
    WriterMap::iterator it_writer = mWriterMap.begin();
    for(; mWriterMap.end() != it_writer; ++it_writer)
    {
        delete it_writer->second;
        it_writer->second = NULL;
    }

    ReaderMap::iterator it_reader = mReaderMap.begin();
    for(; mReaderMap.end() != it_reader; ++it_reader)
    {
       delete it_reader->second;
       it_reader->second = NULL;
    }
}

void ComponentEditorWidget::showContextMenu(const QPoint& pos)
{
    ActionCommander comm(this);
    QPoint position = mapTo(this, pos);
    QMenu contextMenu(tr("Context menu"), this);

    QAction *actionChangeEdgeLabel = comm.addAction("Rename Edge", SLOT(changeSelectedEdgeLabel()), *(IconManager::getInstance()->getIcon("label")));
    QAction *actionRemoveEdge  = comm.addAction("Remove Edge", SLOT(removeSelectedEdge()), *(IconManager::getInstance()->getIcon("remove")));
    QAction *actionChangeLabel = comm.addAction("Rename Node", SLOT(changeSelectedVertexLabel()), *(IconManager::getInstance()->getIcon("label")));
    QAction *actionRemoveNode  = comm.addAction("Remove Node", SLOT(removeSelectedVertex()), *(IconManager::getInstance()->getIcon("remove")));
    QAction *actionAddFeature     = comm.addAction("Add Feature", SLOT(addFeatureSelected()), *(IconManager::getInstance()->getIcon("addFeature")));
    QAction *actionSwapFeatures   = comm.addAction("Swap Features", SLOT(swapFeaturesSelected()), *(IconManager::getInstance()->getIcon("swap")));
    QAction *actionRenameFeature  = comm.addAction("Rename a Feature", SLOT(renameFeatureSelected()), *(IconManager::getInstance()->getIcon("featureLabel")));
    QAction *actionRemoveFeature  = comm.addAction("Remove a Feature", SLOT(removeFeatureSelected()), *(IconManager::getInstance()->getIcon("remove")));
    QAction *actionRemoveFeatures = comm.addAction("Remove Features", SLOT(removeFeaturesSelected()), *(IconManager::getInstance()->getIcon("removeAll")));
    QAction *actionAddNode = comm.addMappedAction("Add Node", SLOT(addNodeAdhoc(QObject*)), (QObject*)&position, *(IconManager::getInstance()->getIcon("addNode")));
    QAction *actionRefresh = comm.addAction("Refresh", SLOT(refresh()), *(IconManager::getInstance()->getIcon("refresh")));
    QAction *actionShuffle = comm.addAction("Shuffle", SLOT(shuffle()), *(IconManager::getInstance()->getIcon("shuffle")));
    QAction *actionImport = comm.addAction("Import", SLOT(importGraph()), *(IconManager::getInstance()->getIcon("import")));
    QAction *actionExport = comm.addAction("Export", SLOT(exportGraph()), *(IconManager::getInstance()->getIcon("export")));
    QAction *actionReset  = comm.addAction("Reset", SLOT(resetGraph()), *(IconManager::getInstance()->getIcon("reset")));
    QAction *actionLayout = comm.addAction("Layout", SLOT(changeLayout()), *(IconManager::getInstance()->getIcon("layout")));
    QAction *actionSetDragDrop = comm.addAction("Drag-n-Drop", SLOT(setDragDrop()), *(IconManager::getInstance()->getIcon("dragndrop")));
    QAction *actionUnsetDragDrop = comm.addAction("Move-around", SLOT(unsetDragDrop()), *(IconManager::getInstance()->getIcon("move")));
    QAction *actionReloadPropertyDialog = comm.addAction("Reload Command Panel", SLOT(reloadPropertyDialog()), *(IconManager::getInstance()->getIcon("reload")), WidgetManager::getInstance()->getGraphManager());

    // (conditionally) adding the actions to the context menu
    if(mEdgeSelected)
    {
        contextMenu.addAction(actionChangeEdgeLabel);
        contextMenu.addAction(actionRemoveEdge);
    }
    if(mVertexSelected)
    {
        if(mEdgeSelected)
        {
            contextMenu.addSeparator();
        }
        contextMenu.addAction(actionChangeLabel);
        contextMenu.addAction(actionAddFeature);
        contextMenu.addAction(actionSwapFeatures);
        contextMenu.addAction(actionRenameFeature);
        contextMenu.addAction(actionRemoveFeature);
        contextMenu.addAction(actionRemoveFeatures);
        contextMenu.addAction(actionRemoveNode);
    }
    if(mVertexSelected || mEdgeSelected)
    {
        contextMenu.addSeparator();
    }
    contextMenu.addAction(actionAddNode);
    contextMenu.addSeparator();
    contextMenu.addAction(actionImport);
    contextMenu.addAction(actionExport);
    contextMenu.addSeparator();
    contextMenu.addAction(actionRefresh);
    contextMenu.addAction(actionShuffle);
    if(!mpGraph->empty())
    {
        contextMenu.addAction(actionReset);
    }
    contextMenu.addAction(actionLayout);
    contextMenu.addSeparator();
    if(mDragDrop)
    {
        contextMenu.addAction(actionUnsetDragDrop);
    }
    else
    {
        contextMenu.addAction(actionSetDragDrop);
    }
    if(!WidgetManager::getInstance()->getPropertyDialog()->isRunning())
    {
        contextMenu.addSeparator();
        contextMenu.addAction(actionReloadPropertyDialog);
    }
    contextMenu.exec(mapToGlobal(pos));
}

void ComponentEditorWidget::addFeatureFocused()
{
    addFeature(mpFocusedVertex);
}

void ComponentEditorWidget::addFeatureSelected()
{
    addFeature(mpSelectedVertex);
}

void ComponentEditorWidget::addFeature(graph_analysis::Vertex::Ptr vertex)
{
    updateStatus(std::string("adding a feature to vertex '") + vertex->toString() + "' of type '" + vertex->getClassName() + "'...");

    NodeItem *item = mNodeItemMap[vertex];
    if(!item)
    {
        std::string error_msg = std::string("graph_analysis::ComponentEditorWidget::addFeature: provided vertex '") + vertex->getLabel() + "' is not registered with the GUI";
        LOG_ERROR_S << error_msg;
        throw std::runtime_error(error_msg);
        updateStatus(std::string("Failed to add a feature to vertex '") + vertex->toString() + "' of type '" + vertex->getClassName() + "': " + error_msg + "!", GraphManager::TIMEOUT);
    }
    bool ok;
    QStringList features_options;
    features_options << tr("input");
    features_options << tr("output");
    features_options << tr("property");
    features_options << tr("operation");
    // while the set of all possible types would be
//    std::set<std::string> options = VertexTypeManager::getInstance()->getSupportedTypes();
//    foreach(std::string option, options)
//    {
//        features_options << tr(option.c_str());
//    }
    QString strFeatureType = QInputDialog::getItem(this, tr("Choose Feature Type"),
                                         tr("Feature Type:"), features_options,
                                         0, false, &ok);
    if (ok && !strFeatureType.isEmpty())
    {
        // creating the feature vertex
        graph_analysis::Vertex::Ptr featureVertex;
        if("input" == strFeatureType)
        {
            featureVertex = VertexTypeManager::getInstance()->createVertex("inputport", "newInputPort");
        }
        else if("output" == strFeatureType)
        {
            featureVertex = VertexTypeManager::getInstance()->createVertex("outputport", "newOutputPort");
        }
        else if("property" == strFeatureType)
        {
            featureVertex = VertexTypeManager::getInstance()->createVertex("property", "newProperty");
        }
        else // ("operation" == strFeatureType)
        {
            featureVertex = VertexTypeManager::getInstance()->createVertex("operation", "newOperation");
        }
        // creating its affiliated graphics and registering it
        mpGraph->addVertex(featureVertex);
        enableVertex(featureVertex);
        createEdge(vertex, featureVertex, "featureRegistrationEdge");
        mFeatureMap[featureVertex] = item;
        mFeatureIDMap[featureVertex] = item->addFeature(featureVertex);
        // does not forget to update the parallel read-only view of this base graph mpGraph (the one in the layers graph widget)
        updateLayerViewWidget();
        updateStatus(std::string("Added an ") + strFeatureType.toStdString() + " feature to vertex '" + vertex->toString() + "' of type '" + vertex->getClassName() + "'!", GraphManager::TIMEOUT);
        item->update();
    }
    else
    {
        updateStatus(std::string("Failed to add an ") + strFeatureType.toStdString() + " feature to vertex '" + vertex->toString() + "' of type '" + vertex->getClassName() + "': aborted by user!", GraphManager::TIMEOUT);
    }
}

void ComponentEditorWidget::renameFeatureFocused()
{
    renameFeature(mpFocusedVertex);
}

void ComponentEditorWidget::renameFeatureSelected()
{
    renameFeature(mpSelectedVertex);
}

void ComponentEditorWidget::renameFeature(graph_analysis::Vertex::Ptr concernedVertex)
{
    updateStatus(std::string("renaming a feature in vertex '") + concernedVertex->toString() + "' of type '" + concernedVertex->getClassName() + "'...");
    NodeItem *item = mNodeItemMap[concernedVertex];
    if(!item)
    {
        std::string error_msg = std::string("graph_analysis::ComponentEditorWidget::renameFeature: provided vertex '") + concernedVertex->getLabel() + "' is not registered with the GUI";
        LOG_ERROR_S << error_msg;
        throw std::runtime_error(error_msg);
    }
    int featureCount = item->getFeatureCount();
    if(!featureCount)
    {
        QMessageBox::critical(this, tr("Cannot Rename a Feature"), tr("The selected vertex had no features!"));
        updateStatus(std::string("Failed to rename a feature of vertex '") + concernedVertex->toString() + "' of type '" + concernedVertex->getClassName() + ": there are no features!", GraphManager::TIMEOUT);
        return;
    }
    RenameFeatureDialog dialog(item);
    if(dialog.isValid())
    {
        std::string newLabel    = dialog.getNewLabel();
        std::string strFeatureID   = dialog.getFeatureID();
        int featureID;
        std::stringstream ss(strFeatureID);
        ss >> featureID;
        // having identified the feature to be renamed, ordering its re-labeling
        item->setFeatureLabel(featureID, newLabel);
        // does not forget to refresh the parallel read-only view of this base graph mpGraph (the one in the layers graph widget)
        refreshLayerViewWidget();
        updateStatus(std::string("Renamed the feature of local ID '" + boost::lexical_cast<std::string>(featureID) + "' of vertex '") + concernedVertex->toString() + "' of type '" + concernedVertex->getClassName() + "' to '" + newLabel + "'!", GraphManager::TIMEOUT);
    }
    else
    {
        updateStatus(std::string("Failed to rename a feature of vertex '") + concernedVertex->toString() + "' of type '" + concernedVertex->getClassName() + ": aborted by user!", GraphManager::TIMEOUT);
    }
}

void ComponentEditorWidget::removeFeatureFocused()
{
    removeFeature(mpFocusedVertex);
}

void ComponentEditorWidget::removeFeatureSelected()
{
    removeFeature(mpSelectedVertex);
}

void ComponentEditorWidget::removeFeature(graph_analysis::Vertex::Ptr concernedVertex)
{
    updateStatus(std::string("removing a feature from vertex '") + concernedVertex->toString() + "' of type '" + concernedVertex->getClassName() + "'...");
    NodeItem *item = mNodeItemMap[concernedVertex];
    if(!item)
    {
        std::string error_msg = std::string("graph_analysis::ComponentEditorWidget::removeFeature: provided vertex '") + concernedVertex->getLabel() + "' is not registered with the GUI";
        LOG_ERROR_S << error_msg;
        updateStatus(std::string("Failed to remove a feature of vertex '") + concernedVertex->toString() + "' of type '" + concernedVertex->getClassName() + ": " + error_msg + "!", GraphManager::TIMEOUT);
        throw std::runtime_error(error_msg);
    }
    int featureCount = item->getFeatureCount();
    if(!featureCount)
    {
        QMessageBox::critical(this, tr("Cannot Remove a Feature"), tr("The selected vertex had no features!"));
        updateStatus(std::string("Failed to remove a feature of vertex '") + concernedVertex->toString() + "' of type '" + concernedVertex->getClassName() + ": there are no features!", GraphManager::TIMEOUT);
        return;
    }
    bool ok;
    QStringList features_options;
    foreach(NodeItem::VTuple tuple, item->getVertices())
    {
        graph_analysis::Vertex::Ptr vertex = tuple.second;
        std::string option = boost::lexical_cast<std::string>(tuple.first) + ": " + vertex->getLabel();
        features_options << tr(option.c_str());
    }
    QString strFeatureID = QInputDialog::getItem(this, tr("Remove a Feature"),
                                         tr("Feature ID:"), features_options,
                                         0, false, &ok);
    if (ok && !strFeatureID.isEmpty())
    {
        std::stringstream ss(strFeatureID.toStdString());
        int featureID;
        ss >> featureID;
        // remove conceptual edges
        EdgeIterator::Ptr edgeIt = mpGraph->getEdgeIterator(item->getFeature(featureID));
        while(edgeIt->next())
        {
            Edge::Ptr edge = edgeIt->current();
            mpGraph->removeEdge(edge);
        }
        // remove physical edges and their graphics
        graph_analysis::Vertex::Ptr cluster = item->getVertex();
        edgeIt = mpLayoutingGraph->getEdgeIterator(cluster);
        while(edgeIt->next())
        {
            Edge::Ptr edge = edgeIt->current();
            if(mEdgeItemMap.count(edge))
            {
                EdgeItem *edgeItem = mEdgeItemMap[edge];
                if  (
                        edgeItem
                            &&
                        (
                            (item == edgeItem->sourceNodeItem() && featureID == edgeItem->getSourcePortID()) ||
                            (item == edgeItem->targetNodeItem() && featureID == edgeItem->getTargetPortID())
                        )
                    )
                    {
                        mpLayoutingGraph->removeEdge(edge);
                        syncEdgeItemMap(edge);
                        scene()->removeItem(edgeItem);
                    }
            }
        }
        // remove feature graphics
        item->removeFeature(featureID);
        // does not forget to refresh the parallel read-only view of this base graph mpGraph (the one in the layers graph widget)
        refreshLayerViewWidget();
        updateStatus(std::string("Removed the feature of local ID '" + boost::lexical_cast<std::string>(featureID) + "' of vertex '") + concernedVertex->toString() + "' of type '" + concernedVertex->getClassName() + "'!", GraphManager::TIMEOUT);
    }
    else
    {
        updateStatus(std::string("Failed to remove a feature of vertex '") + concernedVertex->toString() + "' of type '" + concernedVertex->getClassName() + ": aborted by user!", GraphManager::TIMEOUT);
    }
}

void ComponentEditorWidget::fromFile(const QString& file, bool prefers_gexf, bool status)
{
    int return_code;
    // removing trailing whitespaces in the filename
    QString filename(file.trimmed());
    if(filename.contains('.'))
    {
        if(filename.endsWith(QString(".gexf")) || filename.endsWith(QString(".xml")))
        {
            return_code = fromFile(filename.toStdString(), "gexf");
        }
        else if(filename.endsWith(QString(".yml")) || filename.endsWith(QString(".yaml")))
        {
            return_code = fromFile(filename.toStdString(), "yml");
        }
        else
        {
            QMessageBox::critical(this, tr("Graph Import Failed"), QString(std::string(std::string("Unsupported file format for file '") + filename.toStdString() + "'!").c_str()));
            updateStatus(std::string("Failed to import graph: from input file '") + filename.toStdString() + "': unsupported file format!", GraphManager::TIMEOUT);
            return;
        }
    }
    else
    {
        if(prefers_gexf)
        {
            return_code = fromFile(filename.toStdString() + ".gexf", "gexf");
        }
        else
        {
            return_code = fromFile(filename.toStdString() + ".yml", "yml");
        }
    }

    if(!return_code)
    {
        if(status)
        {
            updateStatus(std::string("Imported graph: from input file '") + filename.toStdString() + "'!", GraphManager::TIMEOUT);
        }
        WidgetManager::getInstance()->getLayerViewWidget()->refresh(false);
    }
}

void ComponentEditorWidget::importGraph()
{
    updateStatus(std::string("importing graph..."));
    QString selectedFilter;
    QString filename =  QFileDialog::getOpenFileName(this, tr("Choose Input File"), QDir::currentPath(), tr("GEXF (*.gexf *.xml);;YAML/YML (*.yaml *.yml)"), &selectedFilter);

    if (!filename.isEmpty())
    {
        fromFile(filename, selectedFilter.startsWith("GEXF"));
    }
    else
    {
        updateStatus(std::string("Failed to import graph: aborted by user - an empty input filename was provided!"), GraphManager::TIMEOUT);
    }
}

void ComponentEditorWidget::exportGraph()
{
    updateStatus(std::string("exporting graph..."));
    QString selectedFilter;
    QString label =  QFileDialog::getSaveFileName(this, tr("Choose Export File"), QDir::currentPath(), tr("GEXF (*.gexf *.xml);;YAML/YML (*.yaml *.yml);;DOT (*.dot)"), &selectedFilter);

    if (!label.isEmpty())
    {
        // removing trailing whitespaces in the filename
        label = label.trimmed();
        if(label.contains('.'))
        {
            if(label.endsWith(QString(".gexf")) || label.endsWith(QString(".xml")))
            {
                toXmlFile(label.toStdString());
            }
            else if(label.endsWith(QString(".yml")) || label.endsWith(QString(".yaml")))
            {
                toYmlFile(label.toStdString());
            }
            else if(label.endsWith(QString(".dot")))
            {
                toDotFile(label.toStdString());
            }
            else
            {
                QMessageBox::critical(this, tr("Graph Export Failed"), QString(std::string(std::string("Unsupported file format for file '") + label.toStdString() + "'!").c_str()));
                updateStatus(std::string("Failed to export graph: to output file '") + label.toStdString() + "': unsupported file format!", GraphManager::TIMEOUT);
                return;
            }
        }
        else
        {
            if(selectedFilter.startsWith("GEXF"))
            {
                toXmlFile(label.toStdString() + ".gexf");
            }
            else if(selectedFilter.startsWith("YAML"))
            {
                toYmlFile(label.toStdString() + ".yml");
            }
            else
            {
                toDotFile(label.toStdString() + ".dot");
            }
        }
        updateStatus(std::string("Exported graph: to output file '") + label.toStdString() + "'!", GraphManager::TIMEOUT);
    }
    else
    {
        updateStatus(std::string("Failed to export graph: aborted by user - an empty output filename was provided!"), GraphManager::TIMEOUT);
    }
}

Edge::Ptr ComponentEditorWidget::createEdge(Vertex::Ptr sourceNode, Vertex::Ptr targetNode, const std::string& label)
{
    Edge::Ptr edge(new Edge(sourceNode, targetNode, label));
    // registering new edge
    mpGraph->addEdge(edge);
    enableEdge(edge);
    return edge;
}

Vertex::Ptr ComponentEditorWidget::createVertex(const std::string& type, const std::string& label)
{
    std::set<std::string> types = VertexTypeManager::getInstance()->getSupportedTypes();
    std::set<std::string>::iterator type_it = types.find(type);
    if(types.end() == type_it)
    {
        std::string error_msg = std::string("graph_analysis::ComponentEditorWidget::createVertex: Given Vertex Type '") + type + "' is not registered!";
        LOG_ERROR_S << error_msg;
        throw std::runtime_error(error_msg);
    }
    graph_analysis::Vertex::Ptr vertex = VertexTypeManager::getInstance()->createVertex(type, label);
    // registering new vertex
    mpGraph->addVertex(vertex);
    enableVertex(vertex);
    return vertex;
}

Edge::Ptr ComponentEditorWidget::createStandaloneEdge(Vertex::Ptr sourceNode, Vertex::Ptr targetNode, const std::string& label)
{
    Edge::Ptr edge(new Edge(sourceNode, targetNode, label));
    return edge; // without registration!
}

Vertex::Ptr ComponentEditorWidget::createStandaloneVertex(const std::string& type, const std::string& label)
{
    std::set<std::string> types = VertexTypeManager::getInstance()->getSupportedTypes();
    std::set<std::string>::iterator type_it = types.find(type);
    if(types.end() == type_it)
    {
        std::string error_msg = std::string("graph_analysis::ComponentEditorWidget::createVertex: Given Vertex Type '") + type + "' is not registered!";
        LOG_ERROR_S << error_msg;
        throw std::runtime_error(error_msg);
    }
    graph_analysis::Vertex::Ptr vertex = VertexTypeManager::getInstance()->createVertex(type, label);
    return vertex; // without registration!
}

void ComponentEditorWidget::addNodeAdhoc(QObject *pos)
{
    updateStatus(std::string("adding new node..."));
    QPoint *position = (QPoint *)pos; // the scene position where to place the new node
    AddNodeDialog nodeDialog;
    if(nodeDialog.isValid())
    {
        graph_analysis::Vertex::Ptr vertex = VertexTypeManager::getInstance()->createVertex(nodeDialog.getNodeType(), nodeDialog.getNodeLabel());
        mpGraph->addVertex(vertex);
        mpLayoutingGraph->addVertex(vertex);
        enableVertex(vertex);
        // Registering and repositioning the new node item
        NodeItem* nodeItem = NodeTypeManager::getInstance()->createItem(this, vertex);
        nodeItem->setPos((double) position->x(), (double) position->y());
        mNodeItemMap[vertex] = nodeItem;
        scene()->addItem(nodeItem);
        // does not forget to update the parallel read-only view of this base graph mpGraph (the one in the layers graph widget)
        updateLayerViewWidget();
        updateStatus(std::string("Added new node '") + vertex->toString() + "' of type '" + vertex->getClassName() + "'!", GraphManager::TIMEOUT);
    }
    else
    {
        updateStatus(std::string("Failed to add new node: aborted by user!"), GraphManager::TIMEOUT);
    }
}

void ComponentEditorWidget::changeFocusedVertexLabel()
{
    updateStatus(std::string("renaming focused node..."));
    bool ok;
    QString label = QInputDialog::getText(this, tr("Input Node Label"),
                                         tr("New Label:"), QLineEdit::Normal,
                                         QString(mpFocusedVertex->getLabel().c_str()), &ok);
    if (ok && !label.isEmpty())
    {
        std::string old_label = mpFocusedVertex->toString();
        std::string new_label = label.toStdString();
        changeVertexLabel(mpFocusedVertex, new_label);
        updateStatus(std::string("Renamed focused node '") + old_label + "' to '" + new_label + "'!", GraphManager::TIMEOUT);
    }
    else
    {
        updateStatus(std::string("Failed to rename focused node: aborted by user!"), GraphManager::TIMEOUT);
    }
}

void ComponentEditorWidget::changeSelectedVertexLabel()
{
    updateStatus(std::string("renaming selected node..."));
    bool ok;
    QString label = QInputDialog::getText(this, tr("Input Node Label"),
                                         tr("New Label:"), QLineEdit::Normal,
                                         QString(mpSelectedVertex->getLabel().c_str()), &ok);
    if (ok && !label.isEmpty())
    {
        std::string old_label = mpSelectedVertex->toString();
        std::string new_label = label.toStdString();
        changeVertexLabel(mpSelectedVertex, new_label);
        updateStatus(std::string("Renamed selected node '") + old_label + "' to '" + new_label + "'!", GraphManager::TIMEOUT);
    }
    else
    {
        updateStatus(std::string("Failed to rename selected node: aborted by user!"), GraphManager::TIMEOUT);
    }
}

void ComponentEditorWidget::changeVertexLabel(graph_analysis::Vertex::Ptr vertex, const std::string& label)
{
    vertex->setLabel(label);
    NodeItem* nodeItem = mNodeItemMap[vertex];
    if(!nodeItem)
    {
        std::string error_msg = std::string("graph_analysis::ComponentEditorWidget::changeVertexLabel: provided vertex '") + vertex->getLabel() + "' is not registered with the GUI";
        LOG_ERROR_S << error_msg;
        throw std::runtime_error(error_msg);
    }
    nodeItem->updateLabel();
    // does not forget to refresh the parallel read-only view of this base graph mpGraph (the one in the layers graph widget)
    refreshLayerViewWidget();
}

void ComponentEditorWidget::changeFocusedEdgeLabel()
{
    updateStatus(std::string("renaming focused edge..."));
    bool ok;
    QString label = QInputDialog::getText(this, tr("Input Edge Label"),
                                         tr("New Label:"), QLineEdit::Normal,
                                          QString(mpFocusedEdge->getLabel().c_str()), &ok);
    if (ok && !label.isEmpty())
    {
        std::string old_label = mpFocusedEdge->toString();
        std::string new_label = label.toStdString();
        changeEdgeLabel(mpFocusedEdge, new_label);
        updateStatus(std::string("Renamed focused edge '") + old_label + "' to '" + new_label + "'!", GraphManager::TIMEOUT);
    }
    else
    {
        updateStatus(std::string("Failed to rename focused edge: aborted by user!"), GraphManager::TIMEOUT);
    }
}

void ComponentEditorWidget::changeSelectedEdgeLabel()
{
    updateStatus(std::string("renaming selected edge..."));
    bool ok;
    QString label = QInputDialog::getText(this, tr("Input Edge Label"),
                                         tr("New Label:"), QLineEdit::Normal,
                                          QString(mpSelectedEdge->getLabel().c_str()), &ok);
    if (ok && !label.isEmpty())
    {
        std::string old_label = mpSelectedEdge->toString();
        std::string new_label = label.toStdString();
        changeEdgeLabel(mpSelectedEdge, new_label);
        updateStatus(std::string("Renamed selected edge '") + old_label + "' to '" + new_label + "'!", GraphManager::TIMEOUT);
    }
    else
    {
        updateStatus(std::string("Failed to rename selected edge: aborted by user!"), GraphManager::TIMEOUT);
    }
}

void ComponentEditorWidget::changeEdgeLabel(graph_analysis::Edge::Ptr concernedEdge, const std::string& label)
{
    EdgeItem* edge = mEdgeItemMap[concernedEdge];
    if(!edge)
    {
        std::string error_msg = std::string("graph_analysis::ComponentEditorWidget::changeEdgeLabel: provided edge '") + concernedEdge->getLabel() + "' is not registered with the GUI";
        LOG_ERROR_S << error_msg;
        throw std::runtime_error(error_msg);
    }
    concernedEdge->setLabel(label);
    mEdgeMap[concernedEdge]->setLabel(label);
    graphitem::edges::EdgeLabel* edgeLabel = (graphitem::edges::EdgeLabel *) edge->getLabel();
    edgeLabel->setPlainText(QString(label.c_str()));
    edge->adjustLabel();
    // does not forget to refresh the parallel read-only view of this base graph mpGraph (the one in the layers graph widget)
    refreshLayerViewWidget();
}

void ComponentEditorWidget::removeFocusedEdge()
{
    clearEdge(mpFocusedEdge);
    clearEdgeFocus();
}

void ComponentEditorWidget::removeSelectedEdge()
{
    clearEdge(mpSelectedEdge);
}

void ComponentEditorWidget::clearEdge(graph_analysis::Edge::Ptr concernedEdge)
{
    std::string concernedEdgeLabel = concernedEdge->toString();
    updateStatus(std::string("removing edge '") + concernedEdgeLabel + "'...");
    EdgeItem *edge = mEdgeItemMap[concernedEdge];
    if(!edge)
    {
        std::string error_msg = std::string("graph_analysis::ComponentEditorWidget::clearEdge: provided edge '") + concernedEdge->getLabel() + "' is not registered with the GUI";
        LOG_ERROR_S << error_msg;
        throw std::runtime_error(error_msg);
    }
    scene()->removeItem(edge);
    const std::string label = concernedEdge->getLabel();
    const graph_analysis::Vertex::Ptr sourceClusterVertex = concernedEdge->getSourceVertex();
    const graph_analysis::Vertex::Ptr targetClusterVertex = concernedEdge->getTargetVertex();
    const NodeItem * sceneSourceNodeItem = mNodeItemMap[sourceClusterVertex];
    const NodeItem * sceneTargetNodeItem = mNodeItemMap[targetClusterVertex];
    if(!sceneSourceNodeItem || !sceneTargetNodeItem)
    {
        std::string error_msg = std::string("graph_analysis::gui::ComponentEditorWidget::removeSelectedEdge: the selected edge '")
                                        + label + "' in the layouting graph mpLayoutingGraph has invalid Vertex endpoints or invalid scene items correspondands";
        LOG_ERROR_S << error_msg;
        throw std::runtime_error(error_msg);
    }

    // locating and deleting the main edge in mpGraph correspondent to the concerned edge
    EdgeIterator::Ptr edgeIt = mpGraph->getEdgeIterator();
    while(edgeIt->next())
    {
        Edge::Ptr edge = edgeIt->current();
        if(label != edge->getLabel())
        {
            continue;
        }
        NodeItem * sourceNodeItem = mFeatureMap[edge->getSourceVertex()];
        NodeItem * targetNodeItem = mFeatureMap[edge->getTargetVertex()];
        if(!sourceNodeItem || !targetNodeItem)
        {
            std::string error_msg = std::string("graph_analysis::gui::ComponentEditorWidget::removeSelectedEdge: the iterated-over edge '")
                                            + label + "' in the main graph mpGraph has invalid Vertex endpoints or invalid scene items correspondands";
            LOG_ERROR_S << error_msg;
            throw std::runtime_error(error_msg);
        }
        if  (
                    sourceClusterVertex == sourceNodeItem->getVertex() && sceneSourceNodeItem == sourceNodeItem
                &&  targetClusterVertex == targetNodeItem->getVertex() && sceneTargetNodeItem == targetNodeItem
            )
        {
            mpGraph->removeEdge(edge);
            break; // assuming there is a single correspondent edge in the main graph mpGraph
        }
    }
    // does not forget to delete the default edge
    mpLayoutingGraph->removeEdge(concernedEdge);
    syncEdgeItemMap(concernedEdge);
    // does not forget to refresh the parallel read-only view of this base graph mpGraph (the one in the layers graph widget)
    refreshLayerViewWidget();
    updateStatus(std::string("Removed edge '") + concernedEdgeLabel + "'!", GraphManager::TIMEOUT);
}

void ComponentEditorWidget::removeFocusedVertex()
{
    clearVertex(mpFocusedVertex);
    clearNodeFocus();
}

void ComponentEditorWidget::removeSelectedVertex()
{
    clearVertex(mpSelectedVertex);
}

void ComponentEditorWidget::clearVertex(graph_analysis::Vertex::Ptr concernedVertex)
{
    std::string concernedVertexLabel = concernedVertex->toString();
    updateStatus(std::string("removing node '") + concernedVertexLabel + "'...");
    // removing possible (default?) edges of this cluster node within the main graph (and its feature-vertices)
    if(graph_analysis::ClusterVertex::vertexType() != concernedVertex->getClassName())
    {
        std::string error_msg = std::string("graph_analysis::ComponentEditorWidget::clearVertex: the supplied vertex '") + concernedVertex->getLabel()
                                        + "' is of unexpected type '" + concernedVertex->getClassName() + "'";
        LOG_ERROR_S << error_msg;
        throw std::runtime_error(error_msg);
    }
    EdgeIterator::Ptr edgeIt = mpGraph->getEdgeIterator(concernedVertex);
    while(edgeIt->next())
    {
        Edge::Ptr edge = edgeIt->current();
        Vertex::Ptr featureVertex;
        Vertex::Ptr sourceVertex = edge->getSourceVertex();
        Vertex::Ptr targetVertex = edge->getTargetVertex();
        if(sourceVertex != concernedVertex)
        {
            featureVertex = sourceVertex;
        }
        else
        {
            featureVertex = targetVertex;
        }
        // removing all edges of the featureVertex
        EdgeIterator::Ptr edgeIterator = mpGraph->getEdgeIterator(featureVertex);
        while(edgeIterator->next())
        {
            mpGraph->removeEdge(edgeIterator->current());
        }
        mpGraph->removeVertex(featureVertex);
        // already removed the edge itself up above //       mpGraph->removeEdge(edge);
    }
    // removing default edges (in the secondary/layouting graph and elliminating their corresponding graphical representations off the screen)
    edgeIt = mpLayoutingGraph->getEdgeIterator(concernedVertex);
    while(edgeIt->next())
    {
        Edge::Ptr edge = edgeIt->current();
        EdgeItem *edgeItem = mEdgeItemMap[edge];
        if(edgeItem)
        {
            scene()->removeItem(edgeItem);
        }
        syncEdgeItemMap(edge);
//        mpLayoutingGraph->removeEdge(edge); // commented out since it introduces bugs when mpLayoutingGraph is dirty
    }
//    mpLayoutingGraph->removeVertex(concernedVertex); // commented out since it introduces bugs when mpLayoutingGraph is 'dirty'
    NodeItem *item = mNodeItemMap[concernedVertex];
    if(!item)
    {
        std::string error_msg = std::string("graph_analysis::ComponentEditorWidget::removeVertex: provided vertex '") + concernedVertex->getLabel() + "' is not registered with the GUI";
        LOG_ERROR_S << error_msg;
        throw std::runtime_error(error_msg);
    }
    scene()->removeItem(item);
    mpGraph->removeVertex(concernedVertex);
    // does not forget to refresh the parallel read-only view of this base graph mpGraph (the one in the layers graph widget)
    refreshLayerViewWidget();
    updateStatus(std::string("Removed node '") + concernedVertexLabel + "'!", GraphManager::TIMEOUT);
}

void ComponentEditorWidget::changeLayout()
{
    updateStatus(std::string("changing graph layout..."));
    bool ok;
    QStringList options;
    std::set<std::string> supportedLayouts = mpGVGraph->getSupportedLayouts();
//    options << tr("circo") << tr("dot") << tr("fdp") << tr("neato") << tr("osage") << tr("sfdp") << tr("twopi"); // [circo, dot, fdp, neato, osage, sfdp, twopi]
    foreach(std::string supportedLayout, supportedLayouts)
    {
        options << tr(supportedLayout.c_str());
    }
    QString layout = QInputDialog::getItem(this, tr("Input New Layout"),
                                         tr("select a layout:"), options,
                                         0, false, &ok);
    if (ok && !layout.isEmpty())
    {
        std::string desiredLayout = layout.toStdString();
        reset(true /*keepData*/);
        setLayout(QString(desiredLayout.c_str()));
        updateStatus(std::string("Changed graph layout to '") + desiredLayout + "'!", GraphManager::TIMEOUT);
    }
    else
    {
        updateStatus(std::string("Failed to change graph layout: aborted by user!"), GraphManager::TIMEOUT);
    }
}

void ComponentEditorWidget::setStartVertex(graph_analysis::Vertex::Ptr startVertex, NodeItem::id_t featureID)
{
    updateStatus(std::string("drag-n-drop: setting source node to '") + startVertex->toString() + "' (feature=" + boost::lexical_cast<std::string>(featureID) + ")...");
    if(graph_analysis::ClusterVertex::vertexType() != startVertex->getClassName())
    {
        std::string error_msg = std::string("graph_analysis::gui::ComponentEditorWidget::setStartVertex: expected startVertex to be of type 'graph_analysis::ClusterVertex'; instead, found type '")
                                        + startVertex->getClassName() + "'";
        LOG_ERROR_S << error_msg;
        throw std::runtime_error(error_msg);
    }
    mpStartVertex   = startVertex;
    NodeItem *item  = mNodeItemMap[startVertex];
    mpStartFeature     = item->getFeature(featureID);
    updateStatus(std::string("Drag-n-drop: set source node to '") + startVertex->toString() + "' (feature=" + boost::lexical_cast<std::string>(featureID) + ")...", GraphManager::TIMEOUT);
}

void ComponentEditorWidget::setEndVertex(graph_analysis::Vertex::Ptr endVertex, NodeItem::id_t featureID)
{
    updateStatus(std::string("drag-n-drop: setting target node to '") + endVertex->toString() + "' (feature=" + boost::lexical_cast<std::string>(featureID) + ")...");
    if(graph_analysis::ClusterVertex::vertexType() != endVertex->getClassName())
    {
        std::string error_msg = std::string("graph_analysis::gui::ComponentEditorWidget::setEndVertex: expected endVertex to be of type 'graph_analysis::ClusterVertex'; instead, found type '")
                                        + endVertex->getClassName() + "'";
        LOG_ERROR_S << error_msg;
        throw std::runtime_error(error_msg);
    }
    mpEndVertex     = endVertex;
    NodeItem *item  = mNodeItemMap[endVertex];
    if(!item)
    {
        std::string error_msg = std::string("graph_analysis::ComponentEditorWidget::setEndVertex: provided vertex '") + endVertex->getLabel() + "' is not registered with the GUI";
        LOG_ERROR_S << error_msg;
        throw std::runtime_error(error_msg);
    }
    mpEndFeature       = item->getFeature(featureID);
    NodeItem *sourceNodeItem = mNodeItemMap[mpStartVertex];
    NodeItem *targetNodeItem = item;
    updateStatus(std::string("Drag-n-drop: set target node to '") + endVertex->toString() + "' (feature=" + boost::lexical_cast<std::string>(featureID) + ")...", GraphManager::TIMEOUT);
    if(sourceNodeItem == targetNodeItem)
    {
        // preventing self-edges and handling it into features swapping
        NodeItem::id_t start_featureID = mFeatureIDMap[mpStartFeature];
        NodeItem::id_t   end_featureID = mFeatureIDMap[mpEndFeature];
        updateStatus(std::string("drag-n-drop: found identical source and target node '") + endVertex->toString()
                        + "' -> swapping features of IDs " + boost::lexical_cast<std::string>(start_featureID) + " and "
                        + boost::lexical_cast<std::string>(end_featureID) + "..."
                    );
        if(mpStartFeature->getClassName() != mpEndFeature->getClassName())
        {
            std::string error_msg = std::string("The two features are of different types '") + mpStartFeature->getClassName() + "' and '" + mpEndFeature->getClassName() + "'";
            LOG_WARN_S << "graph_analysis::gui::ComponentEditorWidget::setEndVertex: failed to initiate features swapping: " << error_msg;
            QMessageBox::critical(this, tr("Features Swapping Failed"), QString(error_msg.c_str()));
            updateStatus(std::string("Drag-n-drop failed: tried to swap features '") + mpStartFeature->toString() + "' and '"
                            + mpEndFeature->toString() + "' of IDs " + boost::lexical_cast<std::string>(start_featureID) + " and "
                            + boost::lexical_cast<std::string>(end_featureID) + " respectively, of different types '"
                            + mpStartFeature->getClassName() + "' and '" + mpEndFeature->getClassName() + "' respectively!"
                            , GraphManager::TIMEOUT
                        );
            return;
        }
        sourceNodeItem->swapFeatures(start_featureID, end_featureID);
        updateStatus(std::string("Drag-n-drop: swapped features '") + mpStartFeature->toString() + "' and '"
                        + mpEndFeature->toString() + "' of IDs " + boost::lexical_cast<std::string>(start_featureID)
                        + " and " + boost::lexical_cast<std::string>(end_featureID) + " respectively and of consistent type '"
                        + mpStartFeature->getClassName() + "'!"
                        , GraphManager::TIMEOUT
                    );
    }
    else
    {
        if(graph_analysis::OutputPortVertex::vertexType() != mpStartFeature->getClassName())
        {
            std::string error_msg = std::string("Expected associated source featureVertex to be of type 'graph_analysis::OutputPortVertex'; instead, found type '")
                                            + mpStartFeature->getClassName() + "'";
            LOG_WARN_S << "graph_analysis::gui::ComponentEditorWidget::setEndVertex: " << error_msg;
            QMessageBox::critical(this, tr("Edge Creation Failed"), QString(error_msg.c_str()));
            updateStatus(std::string("Drag-n-drop failed: '") + error_msg + "'!"
                            , GraphManager::TIMEOUT
                        );
            return;
        }
        if(graph_analysis::InputPortVertex::vertexType() != mpEndFeature->getClassName())
        {
            std::string error_msg = std::string("Expected associated target featureVertex to be of type 'graph_analysis::InputPortVertex'; instead, found type '")
                                            + mpEndFeature->getClassName() + "'";
            LOG_WARN_S << "graph_analysis::gui::ComponentEditorWidget::setEndVertex: " << error_msg;
            QMessageBox::critical(this, tr("Edge Creation Failed"), QString(error_msg.c_str()));
            updateStatus(std::string("Drag-n-drop failed: '") + error_msg + "'!"
                            , GraphManager::TIMEOUT
                        );
            return;
        }
        addEdgeAdHoc();
    }
}

void ComponentEditorWidget::addEdgeAdHoc() // assumes the concerned edge-creation member fields are properly set already
{
    updateStatus(std::string("drag-n-drop: adding edge..."));
    bool ok;
    QString label = QInputDialog::getText(this, tr("Input New Edge Label"),
                                         tr("New Edge Label:"), QLineEdit::Normal,
                                         QString("newEdge"), &ok);
    if (ok && !label.isEmpty())
    {
        std::string edge_label = label.toStdString();
        spawnEdge(edge_label); // assumes the concerned edge-creation member fields are properly set already
        NodeItem::id_t start_featureID = mFeatureIDMap[mpStartFeature];
        NodeItem::id_t   end_featureID = mFeatureIDMap[mpEndFeature];
        // does not forget to update the parallel read-only view of this base graph mpGraph (the one in the layers graph widget)
        updateLayerViewWidget();
        updateStatus(std::string("Drag-n-drop completed: added edge '") + edge_label + "' in between features '"
                        + mpStartFeature->toString() + "' and '"
                        + mpEndFeature->toString() + "' of IDs " + boost::lexical_cast<std::string>(start_featureID)
                        + " and " + boost::lexical_cast<std::string>(end_featureID) + " respectively and of consistent type '"
                        + mpStartFeature->getClassName() +  "' of clusters '"
                        + mpStartVertex->toString() + "' and '" + mpEndVertex->toString() + "'!"
                        , GraphManager::TIMEOUT
                    );
    }
    else
    {
        updateStatus(std::string("Drag-n-drop failed: aborted by user!"), GraphManager::TIMEOUT);
    }
}

void ComponentEditorWidget::spawnEdge(const std::string& label) // assumes the concerned edge-creation member fields are properly set already
{
    NodeItem *sourceNodeItem = mNodeItemMap[mpStartVertex];
    NodeItem *targetNodeItem = mNodeItemMap[mpEndVertex];
    if(sourceNodeItem && targetNodeItem)
    {
        if(sourceNodeItem == targetNodeItem)
        {
            // preventing self-edges (handled it into features swapping already)
            return;
        }
        Edge::Ptr edge(new Edge(mpStartFeature, mpEndFeature, label));
        mpGraph->addEdge(edge);
        enableEdge(edge);
        Edge::Ptr default_edge(new Edge(mpStartVertex, mpEndVertex, label));
        mpLayoutingGraph->addEdge(default_edge);

        EdgeItem* edgeItem = EdgeTypeManager::getInstance()->createItem(this, sourceNodeItem, mFeatureIDMap[mpStartFeature], targetNodeItem, mFeatureIDMap[mpEndFeature], default_edge);
        scene()->addItem(edgeItem);
        edgeItem->adjust();
        mEdgeItemMap[default_edge] = edgeItem;
    }
    else
    {
        std::string error_msg = std::string("graph_analysis::gui::ComponentEditorWidget::spawnEdge: could not insert new edge of label '") + label
                                        + "' since its endpoint vertices are not both registered in mNodeItemMap";
        LOG_ERROR_S << error_msg;
        throw std::runtime_error(error_msg);
    }
}

void ComponentEditorWidget::toYmlFile(const std::string& filename)
{
    try
    {
        mWriterMap["yaml"]->write(filename, mpGraph);
    }
    catch(std::runtime_error e)
    {
        LOG_ERROR_S << "graph_analysis::gui::ComponentEditorWidget::toYmlFile: export failed: " << e.what();
        QMessageBox::critical(this, tr("Graph Export Failed"), QString(e.what()));
        updateStatus(std::string("Yaml Graph export failed: ") + std::string(e.what()), GraphManager::TIMEOUT);
    }
}

void ComponentEditorWidget::toDotFile(const std::string& filename)
{
    try
    {
        mWriterMap["dot"]->write(filename, mpGraph);
    }
    catch(std::runtime_error e)
    {
        LOG_ERROR_S << "graph_analysis::gui::ComponentEditorWidget::toDotFile: export via graphviz failed: " << e.what();
        QMessageBox::critical(this, tr("Graph Export via GraphViz Failed"), QString(e.what()));
        updateStatus(std::string("Dot Graph export failed: ") + std::string(e.what()), GraphManager::TIMEOUT);
    }
}

void ComponentEditorWidget::gvRender(const std::string& filename)
{
    try
    {
        mpGVGraph->renderToFile(filename, mLayout.toStdString());
    }
    catch(std::runtime_error e)
    {
        LOG_ERROR_S << "graph_analysis::gui::ComponentEditorWidget::toDotFile: export via graphviz failed: " << e.what();
        QMessageBox::critical(this, tr("Graph Export via GraphViz Failed"), QString(e.what()));
        updateStatus(std::string("Dot Graph export failed: ") + std::string(e.what()), GraphManager::TIMEOUT);
    }
}

void ComponentEditorWidget::toXmlFile(const std::string& filename)
{
    try
    {
        mWriterMap["gexf"]->write(filename, mpGraph);
    }
    catch(std::runtime_error e)
    {
        LOG_ERROR_S << "graph_analysis::gui::ComponentEditorWidget::toXmlFile: export to .gexf failed: " << e.what();
        QMessageBox::critical(this, tr("Graph Export to .gexf Failed"), QString(e.what()));
        updateStatus(std::string("Gexf Graph export failed: ") + std::string(e.what()), GraphManager::TIMEOUT);
    }
}

int ComponentEditorWidget::fromFile(const std::string& filename, const std::string& format)
{
    graph_analysis::BaseGraph::Ptr graph = BaseGraph::Ptr( new gl::DirectedGraph() );
    try
    {
        mReaderMap[format]->read(filename, graph);
    }
    catch(std::runtime_error e)
    {
        LOG_WARN_S << "graph_analysis::gui::ComponentEditorWidget::fromFile: graph import failed: " << e.what();
        QMessageBox::critical(this, tr("Graph Import Failed"), QString(e.what()));
        updateStatus(std::string("Graph import failed: ") + std::string(e.what()), GraphManager::TIMEOUT);
        return 1;
    }
    catch(std::string s)
    {
        LOG_WARN_S << "graph_analysis::gui::ComponentEditorWidget::fromFile: graph import failed: " << s;
        QMessageBox::critical(this, tr("Graph Import Failed"), QString(s.c_str()));
        updateStatus(std::string("Graph import failed: ") + s, GraphManager::TIMEOUT);
        return 1;
    }

    mpGraph = graph;
    updateLayerViewWidget();
    mpSubGraph = mGraphView.apply(mpGraph);
    mFiltered = true;

    mpSubGraph->enableAllVertices();
    mpSubGraph->enableAllEdges();
    refresh(false);
    return 0;
}

void ComponentEditorWidget::reset(bool keepData)
{
    clear();

    if(!keepData)
    {
        mpGraph = BaseGraph::Ptr( new gl::DirectedGraph() );
        // forgets not to update the layers widget main graph handle
        updateLayerViewWidget();
        mpSubGraph = mGraphView.apply(mpGraph);
        mFiltered = true;
    }

    if(mpGVGraph)
    {
        delete mpGVGraph;
    }
    mMaxNodeHeight  = 0;
    mMaxNodeWidth   = 0;
    mpLayoutingGraph = BaseGraph::Ptr( new gl::DirectedGraph() );
    mpGVGraph = new io::GVGraph(mpLayoutingGraph, "GVComponentEditorWidget");
}

void ComponentEditorWidget::clear()
{
    // TO-DO: add filtering clearing?
    if(mpGVGraph)
    {
        mpGVGraph->clearEdges();
        mpGVGraph->clearNodes();
    }

    mNodeItemMap.clear();
    mEdgeItemMap.clear();
    mFeatureIDMap.clear();
    mFeatureMap.clear();
    mEdgeMap.clear();
    scene()->clear();
}

void ComponentEditorWidget::refresh(bool status)
{
    if(status && mInitialized)
    {
        updateStatus(std::string("refreshing graph..."));
    }
    reset(true /*keepData*/);
    updateFromGraph();
    update();
    if(!mInitialized)
    {
        mInitialized = true;
    }
    else if(status)
    {
        updateStatus(std::string("Refreshed graph!"), GraphManager::TIMEOUT);
    }
}

void ComponentEditorWidget::enableVertex(graph_analysis::Vertex::Ptr vertex)
{
    mpSubGraph->enable(vertex);
    LOG_DEBUG_S << "Enabled vertex '" << vertex->getLabel() << "' of ID: " << mpSubGraph->getBaseGraph()->getVertexId(vertex);
}
void ComponentEditorWidget::enableEdge(graph_analysis::Edge::Ptr edge)
{
    mpSubGraph->enable(edge);
    LOG_DEBUG_S << "Enabled edge '" << edge->getLabel() << "' of ID:  " << mpSubGraph->getBaseGraph()->getEdgeId(edge);
}

void ComponentEditorWidget::disableVertex(graph_analysis::Vertex::Ptr vertex)
{
    mpSubGraph->disable(vertex);
    LOG_DEBUG_S << "Disabled vertex '" << vertex->getLabel() << "' of ID: " << mpSubGraph->getBaseGraph()->getVertexId(vertex);
}
void ComponentEditorWidget::disableEdge(graph_analysis::Edge::Ptr edge)
{
    mpSubGraph->disable(edge);
    LOG_DEBUG_S << "Disabled edge '" << edge->getLabel() << "' of ID:  " << mpSubGraph->getBaseGraph()->getEdgeId(edge);
}

void ComponentEditorWidget::updateFromGraph()
{
    /* !!! reset(true); shall be called apriori (unless you know what you're doing) !!! */
    VertexIterator::Ptr nodeIt = mpGraph->getVertexIterator();
    while(nodeIt->next())
    {
        Vertex::Ptr vertex = nodeIt->current();

        // Check on active filter
        if(mFiltered && !mpSubGraph->enabled(vertex))
        {
            LOG_DEBUG_S << "graph_analysis::gui::ComponentEditorWidget: Filtered out vertex: " << vertex->toString();
            continue;
        }

        if(mNodeItemMap.count(vertex))
        {
            continue;
        }

        if(graph_analysis::ClusterVertex::vertexType() == vertex->getClassName())
        {
            // Registering new Cluster node items only
            NodeItem* nodeItem = NodeTypeManager::getInstance()->createItem(this, vertex);
            mNodeItemMap[vertex] = nodeItem;
            scene()->addItem(nodeItem);
            mpLayoutingGraph->addVertex(vertex);
            mpGVGraph->addNode(vertex);
        }
    }
    EdgeIterator::Ptr edgeIt = mpGraph->getEdgeIterator();
    while(edgeIt->next())
    {
        Edge::Ptr edge = edgeIt->current();

        // Check on active filter
        if(mFiltered && !mpSubGraph->enabled(edge))
        {
            LOG_DEBUG_S << "Filtered out an edge of filtering value: " << mpSubGraph->enabled(edge);
            continue;
        }

        if(mEdgeItemMap.count(edge))
        {
            continue;
        }

        Vertex::Ptr source = edge->getSourceVertex();
        Vertex::Ptr target = edge->getTargetVertex();

        std::string sourceClassName = source->getClassName();
        std::string targetClassName = target->getClassName();

        if(graph_analysis::OutputPortVertex::vertexType() == sourceClassName && graph_analysis::InputPortVertex::vertexType() == targetClassName)
        {
            // physical edge - processing deflected until after all features will have been registered
            continue;
        }
//        else if (   // disabled Port vertices (only specialized InputPorts/OutputPortsVertex allowed
//                    (graph_analysis::PortVertex::vertexType() == sourceClassName && graph_analysis::ClusterVertex::vertexType() == targetClassName)
//                )
//        {
//            // semantical edge: links a cluster vertex to one of its features
//            std::string warn_msg = std::string("graph_analysis::ComponentEditorWidget::updateFromGraph: found reversed edge from source feature vertex '") +
//                                        source->toString() + "' of type '" + sourceClassName + "' to target Cluster vertex '" +
//                                        target->toString() + "' of type '" + targetClassName + "'!";
//            LOG_WARN_S << warn_msg; // warn. due to cluster being set as target of the semantically valid edge
//            NodeItem* targetNodeItem = mNodeItemMap[ target ];
//            if(!targetNodeItem)
//            {
//                continue;
//            }
//            mFeatureMap[source] = targetNodeItem;
//            mFeatureIDMap[source] = targetNodeItem->addFeature(source);
//        }
        else if (
                    (
                        graph_analysis::ClusterVertex::vertexType() == sourceClassName
                            &&
                        (
                            graph_analysis::InputPortVertex::vertexType() == targetClassName
                                ||
                            graph_analysis::OutputPortVertex::vertexType() == targetClassName
                                ||
                            graph_analysis::PropertyVertex::vertexType() == targetClassName
                                ||
                            graph_analysis::OperationVertex::vertexType() == targetClassName
                        )
                    )
                )
        {
            // semantical edge: links a cluster vertex to one of its features
            NodeItem* sourceNodeItem = mNodeItemMap[ source ];
            if(!sourceNodeItem)
            {
                continue;
            }
            mFeatureMap[target] = sourceNodeItem;
            mFeatureIDMap[target] = sourceNodeItem->addFeature(target);
        }
        else
        {
            // invalid edge
            std::string error_msg = std::string("graph_analysis::ComponentEditorWidget::updateFromGraph: found invalid edge from source vertex '") +
                                        source->toString() + "' of type '" + sourceClassName + "' to target vertex '" +
                                        target->toString() + "' of type '" + targetClassName + "'!";
            LOG_ERROR_S << error_msg;
            throw std::runtime_error(error_msg);
        }
    }

    // re-iterating the edges for rendering the physical ones
    edgeIt = mpGraph->getEdgeIterator();
    while(edgeIt->next())
    {
        Edge::Ptr edge = edgeIt->current();

        // Check on active filter
        if(mFiltered && !mpSubGraph->enabled(edge))
        {
            LOG_DEBUG_S << "graph_analysis::gui::ComponentEditorWidget: Filtered out edge: " << edge->toString();
            continue;
        }

        if(mEdgeItemMap.count(edge))
        {
            continue;
        }

        Vertex::Ptr source = edge->getSourceVertex();
        Vertex::Ptr target = edge->getTargetVertex();

        if(graph_analysis::OutputPortVertex::vertexType() == source->getClassName() && graph_analysis::InputPortVertex::vertexType() == target->getClassName())
        {
            NodeItem* sourceNodeItem = mFeatureMap[ source ];
            NodeItem* targetNodeItem = mFeatureMap[ target ];
            // physical edge - processing was deflected until now - i.e. after all features will have been registered
            Edge::Ptr default_edge(new Edge(sourceNodeItem->getVertex(), targetNodeItem->getVertex(), edge->getLabel()));
            mpLayoutingGraph->addEdge(default_edge);
            EdgeItem* edgeItem = EdgeTypeManager::getInstance()->createItem(this, sourceNodeItem, mFeatureIDMap[source], targetNodeItem, mFeatureIDMap[target], default_edge);
            scene()->addItem(edgeItem);
            mEdgeItemMap[default_edge] = edgeItem;
            mpGVGraph->addEdge(default_edge);
            mEdgeMap[default_edge] = edge;
        }
    }

    // computing max node height and width for informing GraphViz of max dimensions w.r.t. immediately subsequent layouting
    NodeItemMap::iterator node_it = mNodeItemMap.begin();
    for(; mNodeItemMap.end() != node_it; ++node_it)
    {
        QRectF nodeBoundingRect = node_it->second->boundingRect();
        qreal height = nodeBoundingRect.height();
        qreal width  = nodeBoundingRect.width();
        if(mMaxNodeHeight < height)
        {
            height = mMaxNodeHeight;
        }
        if(mMaxNodeWidth < width)
        {
            mMaxNodeWidth = width;
        }
    }

    // the mode is synchronized with the current state in the other layers (i.e. the mode that was active right before graphical graph reloading)
    syncDragDrop();

    // layouting - i.e. loading the designated layouting base graph into GraphViz then repositioning the correspoding scene nodes
    if(mLayout.toLower() != "force")
    {
        QApplication::setOverrideCursor(Qt::WaitCursor);
        LOG_INFO_S << "GV started layouting the graph. This can take a while ...";
        base::Time start = base::Time::now();
        mpGVGraph->setNodeAttribute("height", boost::lexical_cast<std::string>(mMaxNodeHeight));
        mpGVGraph->setNodeAttribute("width" , boost::lexical_cast<std::string>(mMaxNodeWidth ));
        mpGVGraph->applyLayout(mLayout.toStdString());
        base::Time delay = base::Time::now() - start;
        QApplication::restoreOverrideCursor();
        LOG_INFO_S << "GV layouted the graph after " << delay.toSeconds();
        {
            using namespace graph_analysis::io;
            std::vector<GVNode> nodes = mpGVGraph->nodes();
            std::vector<GVNode>::const_iterator cit = nodes.begin();
            for(; cit != nodes.end(); ++cit)
            {
                GVNode gvNode = *cit;
                NodeItem* nodeItem = mNodeItemMap[gvNode.getVertex()];
                if(!nodeItem)
                {
                    LOG_WARN_S << "NodeItem: mapped from " <<  gvNode.getVertex()->toString() << "is null";
                    continue;
                }
                // repositioning node in a scaled fashion
                nodeItem->setPos(mScaleFactor * gvNode.x(), mScaleFactor * gvNode.y());
            }
        }

//        {
//            using namespace graph_analysis::io;
//            std::vector<GVEdge> edges = mpGVGraph->edges();
//            std::vector<GVEdge>::const_iterator cit = edges.begin();
//            for(; cit != edges.end(); ++cit)
//            {
//                GVEdge gvEdge = *cit;
//                EdgeItem* edgeItem = mEdgeItemMap[ gvEdge.getEdge() ];
//                edgeItem->setPainterPath( edge.path );
//            }
//        }
    }
}

void ComponentEditorWidget::addVertex(Vertex::Ptr vertex)
{
    mpGraph->addVertex(vertex);
}

void ComponentEditorWidget::addEdge(Edge::Ptr edge)
{
    mpGraph->addEdge(edge);
}

void ComponentEditorWidget::removeVertex(Vertex::Ptr vertex)
{
    mpGraph->removeVertex(vertex);
}

void ComponentEditorWidget::removeEdge(Edge::Ptr edge)
{
    mpGraph->removeEdge(edge);
}

void ComponentEditorWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
#ifdef CLEAR_BY_BACKGROUND
    // deciding whether nodes or edges shall be unfocused
    bool unfocusNodes = !(
                            (mVertexSelected && mVertexFocused && mpSelectedVertex == mpFocusedVertex)
                            ||
                            (mVertexFocused && mEdgeSelected)
                        )
                        ;
    bool unfocusEdges = !(
                            (mEdgeSelected && mEdgeFocused && mpSelectedEdge == mpFocusedEdge)
                            ||
                            (mVertexSelected && mEdgeFocused)
                        )
                        ;
    if(unfocusNodes)
    {
        clearNodeFocus();
    }
    if(unfocusEdges)
    {
        clearEdgeFocus();
    }
#endif
    QGraphicsView::mouseDoubleClickEvent(event);
}

void ComponentEditorWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::MidButton)
    {
        // Use ScrollHand Drag Mode to enable Panning
        setDragMode(ScrollHandDrag);
        // deflecting the current event into propagating a custom default-panning left-mouse-button oriented behaviour
        QMouseEvent fake(event->type(), event->pos(), Qt::LeftButton, Qt::LeftButton, event->modifiers());
        QGraphicsView::mousePressEvent(&fake); // initiates scroll-button panning
    }
    else
    {
        QGraphicsView::mousePressEvent(event);
    }
}

void ComponentEditorWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::MidButton)
    {
        // Use ScrollHand Drag Mode to desable Panning
        setDragMode(NoDrag);
        // deflecting the current event into propagating a custom default-panning left-mouse-button oriented behaviour
        QMouseEvent fake(event->type(), event->pos(), Qt::LeftButton, Qt::LeftButton, event->modifiers());
        QGraphicsView::mouseReleaseEvent(&fake); // terminates scroll-button panning
    }
    else
    {
        QGraphicsView::mouseReleaseEvent(event);
    }
}

void ComponentEditorWidget::updateMoveAround(bool moveAround)
{
    WidgetManager::getInstance()->getPropertyDialog()->uncheckDragDrop(moveAround);
    setDragDrop(!moveAround);
}

void ComponentEditorWidget::updateDragDrop(bool dragDrop)
{
    WidgetManager::getInstance()->getPropertyDialog()->uncheckMoveAround(dragDrop);
    setDragDrop(dragDrop);
}

void ComponentEditorWidget::setDragDrop(bool dragDrop)
{
    updateStatus(std::string("toggling drag-n-drop mode to ") + (dragDrop ? "true" : "false") + "...");
    mDragDrop = dragDrop;
    NodeItemMap::iterator it = mNodeItemMap.begin();
    for(; mNodeItemMap.end() != it; ++it)
    {
        NodeItem *current = it->second;
        if(graph_analysis::ClusterVertex::vertexType() == current->getVertex()->getClassName())
        {
            current->setHandlesChildEvents(!mDragDrop);
            current->setFlag(QGraphicsItem::ItemIsMovable, !mDragDrop);
        }
    }
    updateStatus(std::string("Toggled drag-n-drop mode to ") + (dragDrop ? "true" : "false") + "...", GraphManager::TIMEOUT);
}

void ComponentEditorWidget::syncDragDrop()
{
    NodeItemMap::iterator it = mNodeItemMap.begin();
    for(; mNodeItemMap.end() != it; ++it)
    {
        NodeItem *current = it->second;
        if(graph_analysis::ClusterVertex::vertexType() == current->getVertex()->getClassName())
        {
            current->setHandlesChildEvents(!mDragDrop);
            current->setFlag(QGraphicsItem::ItemIsMovable, !mDragDrop);
        }
    }
}

void ComponentEditorWidget::setDragDrop()
{
    updateStatus(std::string("toggling drag-n-drop mode to true..."));
    mDragDrop = true;
    WidgetManager::getInstance()->getPropertyDialog()->setDragDrop(true);
    if(mVertexSelected)
    {
        NodeItem *item = mNodeItemMap[mpSelectedVertex];
        item->unselect();
    }
    NodeItemMap::iterator it = mNodeItemMap.begin();
    for(; mNodeItemMap.end() != it; ++it)
    {
        NodeItem *current = it->second;
        if(graph_analysis::ClusterVertex::vertexType() == current->getVertex()->getClassName())
        {
            current->setHandlesChildEvents(false); // of !mDragDrop
            current->setFlag(QGraphicsItem::ItemIsMovable, false);
        }
    }
    updateStatus(std::string("Toggled drag-n-drop mode to true!"), GraphManager::TIMEOUT);
}

void ComponentEditorWidget::unsetDragDrop()
{
    updateStatus(std::string("toggling drag-n-drop mode to false..."));
    mDragDrop = false;
    WidgetManager::getInstance()->getPropertyDialog()->setDragDrop(false);
    NodeItemMap::iterator it = mNodeItemMap.begin();
    for(; mNodeItemMap.end() != it; ++it)
    {
        NodeItem *current = it->second;
        if(graph_analysis::ClusterVertex::vertexType() == current->getVertex()->getClassName())
        {
            current->setHandlesChildEvents(true); // of !mDragDrop
            current->setFlag(QGraphicsItem::ItemIsMovable);
        }
    }
    updateStatus(std::string("Toggled drag-n-drop mode to false!"), GraphManager::TIMEOUT);
}

void ComponentEditorWidget::itemMoved()
{
    if (!mTimerId)
    {
        mTimerId = startTimer(1000 / 25);
    }
}

void ComponentEditorWidget::keyPressEvent(QKeyEvent *event)
{
    // check for a keys combination
    Qt::KeyboardModifiers modifiers = event->modifiers();

    if(modifiers & Qt::ControlModifier) // key combinations while holding 'CTRL' pressed
    {
        switch(event->key())
        {
            case Qt::Key_Q: // CTRL+Q and CTRL+W terminate the application
            case Qt::Key_W:
                exit(0);
            break;

            case Qt::Key_Plus: // CTRL+ zooms in
                zoomIn();
            break;

            case Qt::Key_Minus: // CTRL+- zooms out
                zoomOut();
            break;

            case Qt::Key_R: // CTRL+R deletes the graph (it first prompts again the user)
                resetGraph();
            break;

            case Qt::Key_E: // CTRL+S (save) or CTRL+E (export graph) saves the graph to file
            case Qt::Key_S:
                exportGraph();
            break;

            case Qt::Key_A: // CTRL+A prompts the user to add a node
                if(!mDragDrop)
                {
                    addNodeAdhoc();
                }
            break;

            case Qt::Key_I: // CTRL+O (open) or CTRL+I (input graph)  or CTRL+L (load graph) opens a graph from file
            case Qt::Key_O:
            case Qt::Key_L:
                importGraph();
            break;

            case Qt::Key_P: // CTRL+P reloads the property dialog (if it is currently not running)
                if(!WidgetManager::getInstance()->getPropertyDialog()->isRunning())
                {
                    WidgetManager::getInstance()->getGraphManager()->reloadPropertyDialog();
                }
            break;

            case Qt::Key_Left: // CTRL+LeftArrow rotates the view counterclockwise
                if(!mDragDrop)
                {
                    rotate(qreal(-1.13));
                }
            break;

            case Qt::Key_Right: // CTRL+RightArrow rotates the view clockwise
                if(!mDragDrop)
                {
                    rotate(qreal( 1.13));
                }
            break;

            case Qt::Key_CapsLock: // CTRL+CapsLock or CTRL+D toggles the active mode (drag-n-drop mode v. move-around mode)
            case Qt::Key_D:
                mDragDrop ? unsetDragDrop() : setDragDrop();
            break;
        }
    }
    else if(!mDragDrop) // simple keys (move-around mode only!)
    {
        switch(event->key())
        {
        //case Qt::Key_Up:
        //    break;
        //case Qt::Key_Down:
        //    break;
        //case Qt::Key_Left:
        //    break;
        //case Qt::Key_Right:
        //    break;
        case Qt::Key_Plus: // '+' zooms-in
            zoomIn();
        break;

        case Qt::Key_Minus: // '-' zooms-out
            zoomOut();
        break;

        case Qt::Key_Space: // Space, newline and 'R'/'r' refresh the view
        case Qt::Key_Enter:
        case Qt::Key_R:
            refresh();
        break;

        case Qt::Key_S: // 'S'/'s' shuffle the nodes
            shuffle();
        break;

        case Qt::Key_L: // 'L'/'l' changes the layout
            changeLayout();
        break;
        }
    }

    switch(event->key()) // simple keys (permanent)
    {
    case Qt::Key_Escape: // clears node and edge focus
        clearFocus();
    break;
    }

    QGraphicsView::keyPressEvent(event);
}

void ComponentEditorWidget::timerEvent(QTimerEvent *event)
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

#ifndef QT_NO_WHEELEVENT
void ComponentEditorWidget::wheelEvent(QWheelEvent *event)
{
    scaleView(pow((double)2, - event->delta() / 240.0));
}
#endif

void ComponentEditorWidget::drawBackground(QPainter *painter, const QRectF& rect)
{
    Q_UNUSED(rect);
    //// Shadow
//    QRectF sceneRect = this->sceneRect();
    //QRectF rightShadow(sceneRect.right(), sceneRect.top() + 5, 5, sceneRect.height());
    //QRectF bottomShadow(sceneRect.left() + 5, sceneRect.bottom(), sceneRect.width(), 5);
    //if (rightShadow.intersects(rect) || rightShadow.contains(rect))
    //    painter->fillRect(rightShadow, Qt::darkGray);
    //if (bottomShadow.intersects(rect) || bottomShadow.contains(rect))
    //    painter->fillRect(bottomShadow, Qt::darkGray);

    // Fill
    //QLinearGradient gradient(sceneRect.topLeft(), sceneRect.bottomRight());
    //gradient.setColorAt(0, Qt::white);
    //gradient.setColorAt(1, Qt::lightGray);
    //painter->fillRect(rect.intersected(sceneRect), gradient);
    //painter->setBrush(Qt::NoBrush);
    //painter->drawRect(sceneRect);

    //// Text
    //QRectF textRect(sceneRect.left() + 4, sceneRect.top() + 4,
    //                sceneRect.width() - 4, sceneRect.height() - 4);
    //QString message(tr("Click and drag the nodes around, and zoom with the mouse "
    //                   "wheel or the '+' and '-' keys"));

    //QFont font = painter->font();
    //font.setBold(true);
    //font.setPointSize(14);
    //painter->setFont(font);
    //painter->setPen(Qt::lightGray);
    //painter->drawText(textRect.translated(2, 2), message);
    //painter->setPen(Qt::black);
    //painter->drawText(textRect, message);
}

void ComponentEditorWidget::scaleView(qreal scaleFactor)
{
    qreal factor = transform().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
    if (factor < 0.07 || factor > 100)
    {
        return;
    }
    scale(scaleFactor, scaleFactor);
    std::string status_msg = scaleFactor > 1. ? "Zoomed-in" : "Zoomed-out";
    updateStatus(status_msg, GraphManager::TIMEOUT);
}

void ComponentEditorWidget::shuffle()
{
    updateStatus("Shuffelling all nodes ...");
    foreach (QGraphicsItem *item, scene()->items())
    {
        if (qgraphicsitem_cast<NodeItem* >(item))
        {
            item->setPos(-150 * mScaleFactor + mScaleFactor * (qrand() % 300), -150 * mScaleFactor + mScaleFactor * (qrand() % 300));
        }
    }
    updateStatus("Done shuffelling all nodes", GraphManager::TIMEOUT);
}

void ComponentEditorWidget::zoomIn()
{
    scaleView(qreal(1.13));
}

void ComponentEditorWidget::zoomOut()
{
    scaleView(1 / qreal(1.13));
}

void ComponentEditorWidget::resetGraph()
{
    updateStatus(std::string("Resetting the graph ..."));
    if(mpGraph->empty())
    {
        QMessageBox::information(this, tr("Nothing to Reset"), "The graph is already empty!");
        updateStatus(std::string("Failed to reset graph: the graph was already empty!"), GraphManager::TIMEOUT);
    }
    else
    {
        QMessageBox::StandardButton button = QMessageBox::question(this, tr("Confirm Reset"), tr("The Graph will be completely erased! Are you sure you want to continue?"), QMessageBox::Yes | QMessageBox::No);
        switch(button)
        {
            case QMessageBox::Yes:
                reset();
                updateStatus(std::string("Done resetting the graph"), GraphManager::TIMEOUT);
            break;

            default:
                updateStatus(std::string("Failed resetting the graph: aborted by user"), GraphManager::TIMEOUT);
            break;
        }
    }
}

void ComponentEditorWidget::setLayout(QString layoutName)
{
    /* !!! reset(true); shall be called apriori (unless you know what you're doing - e.g. at init) !!! */
    mLayout = layoutName;
    updateFromGraph();
}

void ComponentEditorWidget::setVertexFocused(bool focused)
{
    mVertexFocused = focused;
    WidgetManager::getInstance()->getPropertyDialog()->setVertexFocused(focused);
}

void ComponentEditorWidget::setEdgeFocused(bool focused)
{
    mEdgeFocused = focused;
    WidgetManager::getInstance()->getPropertyDialog()->setEdgeFocused(focused);
}

void ComponentEditorWidget::clearNodeFocus()
{
    if(mVertexFocused)
    {
        NodeItem *item = mNodeItemMap[mpFocusedVertex];
        item->releaseFocus();
        updateStatus(std::string("Cleared node focus off node '") + mpFocusedVertex->toString() + "'!", GraphManager::TIMEOUT);
    }
}

void ComponentEditorWidget::clearEdgeFocus()
{
    if(mEdgeFocused)
    {
        EdgeItem *item = mEdgeItemMap[mpFocusedEdge];
        item->releaseFocus();
        updateStatus(std::string("Cleared edge focus off edge '") + mpFocusedEdge->toString() + "'!", GraphManager::TIMEOUT);
    }
}

void ComponentEditorWidget::clearFocus()
{
    clearNodeFocus();
    clearEdgeFocus();
    if(mVertexFocused && mpFocusedEdge)
    {
        updateStatus(std::string("Cleared focuses: ")
                        + "off node '" + mpFocusedVertex->toString()
                        + "' and off edge '" + mpFocusedEdge->toString() + "'!"
                        , GraphManager::TIMEOUT
                    );
    }
}


void ComponentEditorWidget::removeFeaturesFocused()
{
    removeFeatures(mpFocusedVertex);
}

void ComponentEditorWidget::removeFeaturesSelected()
{
    removeFeatures(mpSelectedVertex);
}

void ComponentEditorWidget::removeFeatures(graph_analysis::Vertex::Ptr concernedVertex)
{
    updateStatus(std::string("Removing all features from vertex '") + concernedVertex->toString() + "' of type '" + concernedVertex->getClassName() + "'...");
    NodeItem *item = mNodeItemMap[concernedVertex];
    // error checking on features removal
    if(!item)
    {
        std::string error_msg = std::string("graph_analysis::ComponentEditorWidget::removeFeatures: provided vertex '") + concernedVertex->getLabel() + "' is not registered with the GUI";
        LOG_ERROR_S << error_msg;
        updateStatus(std::string("Failed to remove all features from vertex '") + concernedVertex->toString() + "' of type '" + concernedVertex->getClassName() + "': " + error_msg + "!", GraphManager::TIMEOUT);
        throw std::runtime_error(error_msg);
    }
    int nfeatures = item->getFeatureCount();
    if(!nfeatures)
    {
        QMessageBox::critical(this, tr("No features to remove"), "The cluster is already empty!");
        updateStatus(std::string("Failed to remove all features from vertex '") + concernedVertex->toString() + "' of type '" + concernedVertex->getClassName() + "': there are no features!", GraphManager::TIMEOUT);
        return;
    }
    else
    {
        // prompting the user for all features deletion
        QMessageBox::StandardButton button = QMessageBox::question(this, tr("Confirm complete features-removal"), tr((QString("All features in node '") + QString(concernedVertex->getLabel().c_str()) + QString("' will be deleted! Are you sure you want to continue?")).toAscii()), QMessageBox::Yes | QMessageBox::No);
        EdgeIterator::Ptr edgeIt;
        graph_analysis::Vertex::Ptr cluster;
        switch(button)
        {
            case QMessageBox::Yes:
                // remove conceptual edges
                foreach(NodeItem::VTuple tuple, item->getVertices())
                {
                    edgeIt = mpGraph->getEdgeIterator(tuple.second);
                    while(edgeIt->next())
                    {
                        Edge::Ptr edge = edgeIt->current();
                        mpGraph->removeEdge(edge);
                    }
                }
                // remove physical edges and their graphics
                cluster = item->getVertex();
                edgeIt = mpLayoutingGraph->getEdgeIterator(cluster);
                while(edgeIt->next())
                {
                    Edge::Ptr edge = edgeIt->current();
                    if(mEdgeItemMap.count(edge))
                    {
                        EdgeItem *edgeItem = mEdgeItemMap[edge];
                        if(edgeItem)
                        {
                            mpLayoutingGraph->removeEdge(edge);
                            syncEdgeItemMap(edge);
                            scene()->removeItem(edgeItem);
                        }
                    }
                    else
                    {
                        mpLayoutingGraph->removeEdge(edge);
                        syncEdgeItemMap(edge);
                    }
                }
                // remove features graphics
                item->removeFeatures();
                // does not forget to refresh the parallel read-only view of this base graph mpGraph (the one in the layers graph widget)
                refreshLayerViewWidget();
                updateStatus(std::string("Removed all features from vertex '") + concernedVertex->toString() + "' of type '" + concernedVertex->getClassName() + "'!", GraphManager::TIMEOUT);
            break;

            default:
                updateStatus(std::string("Failed to remove all features from vertex '") + concernedVertex->toString() + "' of type '" + concernedVertex->getClassName() + "': aborted by user!", GraphManager::TIMEOUT);
            break;
        }
    }
}

void ComponentEditorWidget::syncEdgeItemMap(graph_analysis::Edge::Ptr concernedEdge)
{
    EdgeItemMap::iterator it = mEdgeItemMap.find(concernedEdge);
    if(mEdgeItemMap.end() != it)
    {
        mEdgeItemMap.erase(it);
    }
}

void ComponentEditorWidget::swapFeaturesFocused()
{
    swapFeatures(mpFocusedVertex);
}

void ComponentEditorWidget::swapFeaturesSelected()
{
    swapFeatures(mpSelectedVertex);
}

void ComponentEditorWidget::swapFeatures(graph_analysis::Vertex::Ptr concernedVertex)
{
    updateStatus("swapping features within node '" + concernedVertex->toString() + "' ...");
    NodeItem *item = mNodeItemMap[concernedVertex];
    if(!item)
    {
        std::string error_msg = std::string("graph_analysis::ComponentEditorWidget::swapFeatures: provided vertex '") + concernedVertex->getLabel() + "' is not registered with the GUI";
        LOG_ERROR_S << error_msg;
        throw std::runtime_error(error_msg);
    }
    int featureCount = item->getFeatureCount();
    if(featureCount < 2)
    {
        QMessageBox::critical(this, tr("Cannot Swap Features"), tr("The selected vertex did not have enough features!"));
        updateStatus("Failed to swap features within node '" + concernedVertex->toString() + "': this vertex did not have enough features!", GraphManager::TIMEOUT);
        return;
    }
    SwapFeaturesDialog dialog(item);
    if(dialog.isValid())
    {
        std::string strFeature1ID   = dialog.getFeature1ID();
        std::string strFeature2ID   = dialog.getFeature2ID();
        int feature1ID;
        std::stringstream ss1(strFeature1ID);
        ss1 >> feature1ID;
        int feature2ID;
        std::stringstream ss2(strFeature2ID);
        ss2 >> feature2ID;
        if(feature1ID - feature2ID)
        {
            try
            {
                item->swapFeatures(feature1ID, feature2ID);
            }
            catch(std::runtime_error e)
            {
                LOG_ERROR_S << "graph_analysis::gui::ComponentEditorWidget::swapFeatures: swapping operation failed: " << e.what();
                QMessageBox::critical(this, tr("Swapping Failed"), QString(e.what()));
                updateStatus("Failed to swap features within node '" + concernedVertex->toString() + "': " + std::string(e.what()) + "!", GraphManager::TIMEOUT);
            }
        }
        else
        {
            QMessageBox::information(this, tr("Swapped Features In-place"), tr("identical features were selected!"));
        }
        updateStatus("Swapped features of IDs " + boost::lexical_cast<std::string>(feature1ID) + " and "
                        + boost::lexical_cast<std::string>(feature2ID) + " within node '" + concernedVertex->toString()
                        , GraphManager::TIMEOUT
                    );
    }
    else
    {
        updateStatus("Failed to swap features within node '" + concernedVertex->toString() + "': aborted by user!", GraphManager::TIMEOUT);
    }
}

void ComponentEditorWidget::updateLayerViewWidget()
{
    LayerViewWidget *layerWidget = WidgetManager::getInstance()->getLayerViewWidget();
    layerWidget->setGraph(mpGraph);
    layerWidget->refresh(false);
}

void ComponentEditorWidget::refreshLayerViewWidget(bool status)
{
    WidgetManager::getInstance()->getLayerViewWidget()->refresh(status);
}

void ComponentEditorWidget::toggleDragDrop()
{
    mDragDrop ? unsetDragDrop() : setDragDrop();
}

} // end namespace gui
} // end namespace graph_analysis
