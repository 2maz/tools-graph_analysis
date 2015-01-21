#ifndef OMVIZ_MAINWINDOW_HPP
#define OMVIZ_MAINWINDOW_HPP

#include <QtGui/QMainWindow>
#include <QDir>
#include <owl_om/OrganizationModel.hpp>

class QTableWidgetItem;
class QTreeWidget;

namespace Ui
{
    class Omviz;
}

namespace omviz
{

class GraphWidget;
class PlanningWidget;
class OrganizationModelWidget;
class PropertiesWidget;
class FilterWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = 0);
    ~MainWindow();

private slots:
    void updateFromModel();

private:
    Ui::Omviz* mUiOmviz;
    owl_om::OrganizationModel::Ptr mpOrganizationModel;

    GraphWidget* mGraphWidget;
};

} // end namespace omviz
#endif // OMVIZ_MAINWINDOW_HPP
