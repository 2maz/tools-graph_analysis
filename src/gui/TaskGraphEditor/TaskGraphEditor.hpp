#ifndef GRAPH_ANALYSIS_GUI_TASKGRAPHEDITOR_HPP
#define GRAPH_ANALYSIS_GUI_TASKGRAPHEDITOR_HPP

#include <QTreeWidget>
#include <QWidget>
#include <QProcess>
#include <graph_analysis/Graph.hpp>
#include <graph_analysis/gui/QLauncher.hpp>

namespace Ui
{
class TaskGraphEditor;
}

namespace graph_analysis
{
namespace gui
{
class TaskGraphViewer;

class TaskGraphEditor : public QWidget
{
    Q_OBJECT

public:
    TaskGraphEditor(graph_analysis::BaseGraph::Ptr graph,
                    QWidget* parent = NULL);
    ~TaskGraphEditor();

    QString getClassName() const
    {
        return "graph_analysis::gui::TaskGraphEditor";
    }

private:
    // GUI Elements
    Ui::TaskGraphEditor* mpUi;

    // Graph
    graph_analysis::BaseGraph::Ptr mpGraph;
    TaskGraphViewer* mpTaskGraphViewer;

    // FIXME
    // Stores a template file name
    QString mTemplate;

    /** adding one new file into the task-container and the tree widget */
    bool addFile(QString filename);
    void updateTreeWidget();

    // just for HMI
    QLauncher mLauncher;
    QString lastSavedComponentNetworkDescription;

signals:
    // Will be triggered by the TaskGraphViewer
    void currentStatus(QString, int);

private slots:
    // Needed to observe the TaskGraphViewer
    void currentStatus_internal(QString, int);

    // just for HMI
    void launcher_execution_finished();
    void launcher_execution_started();

public slots:
    // Adding/Removing TaskTemplates
    void on_addButton_clicked();
    void on_removeButton_clicked();

    // Loading/Storing Task Networks
    void on_loadButton_clicked();
    void on_saveButton_clicked();
    void on_updateButton_clicked();
    void on_clearButton_clicked();
    void on_executeNetwork_clicked();

    // On double click we want to create new tasks :)
    void on_taskTemplateTree_itemDoubleClicked(QTreeWidgetItem* item,
                                               int column);

    // For updating the TaskGraphViewer
    void updateVisualization();
};
}
}

#endif
