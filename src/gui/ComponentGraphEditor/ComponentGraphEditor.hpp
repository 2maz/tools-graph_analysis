#ifndef GRAPH_ANALYSIS_GUI_COMPONENTGRAPHEDITOR_HPP
#define GRAPH_ANALYSIS_GUI_COMPONENTGRAPHEDITOR_HPP

#include <graph_analysis/gui/GraphWidget.hpp>

#include <graph_analysis/gui/VertexItemBase.hpp>
#include <graph_analysis/gui/EdgeItemBase.hpp>

namespace graph_analysis
{
namespace gui
{

/**
 * \file ComponentGraphEditor.hpp
 * \class ComponentGraphEditor
 * \brief displaying the raw hypergraph in all its glory
 * \details maintains and displays a simple view of its BaseGraph
 *
 * treats the BaseGraph as read-only.
 */
class ComponentGraphEditor : public GraphWidget
{
    Q_OBJECT

  public:
    /**
     * \brief constructor
     * \param graph underlying base graph
     */
    ComponentGraphEditor(QWidget *parent = NULL);
    virtual ~ComponentGraphEditor();

    QString getClassName() const;

    typedef std::map<graph_analysis::Edge::Ptr, EdgeItemBase*> MyEdgeItemMap;
    typedef std::map<graph_analysis::Vertex::Ptr, VertexItemBase*> MyVertexItemMap;
    MyEdgeItemMap e_map;
    MyVertexItemMap v_map;

    void adjustEdgesOf(VertexItemBase* vertex);

    /**
     * deletes all internal information and rebuilds the visualization
     * according to the current BaseGraph. Respawns all graphical elements by
     * the underlying base graph and trigger the layouting
     */
    virtual void updateLayout();
};

} // end namespace gui
} // end namespace graph_analysis
#endif
