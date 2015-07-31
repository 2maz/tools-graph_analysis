/********************************************************************************
** Form generated from reading UI file 'property_dialog_gui.ui'
**
** Created by: Qt User Interface Compiler version 4.8.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef PROPERTYDIALOG_H
#define PROPERTYDIALOG_H

#include "CustomDialog.hpp"

#include <vector>
#include <QLabel>
#include <QObject>
#include <QCloseEvent>
#include <QMainWindow>
#include <QtGui/QAction>
#include <QtGui/QDialog>
#include <QtGui/QWidget>
#include <QtCore/QVariant>
#include <QtGui/QTabWidget>
#include <QtGui/QHeaderView>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QStackedWidget>

#define WIDTH 306
#define PADDING 7
#define DEFAULT_NFRAMES 3
#define DEFAULT_NBUTTONS 9
#define DEFAULT_LINE_POINTS 15
#define DEFAULT_BUTTON_POINTS 31

namespace graph_analysis {
namespace gui {

    class ViewWidget;
    class LayerWidget;

/**
 * \class PropertyDialog
 * \file PropertyDialog.hpp
 * \brief dockable multi-tab command panel
 * \details accommodates options/properties for both the diagram view and also the layered conceptual graph view
 */
class PropertyDialog : public QObject
{
    Q_OBJECT
public:
    /// keeps track of both vertical and horizontal separators in the scene
    typedef std::vector<QFrame *> QFrames;

    /**
     * \brief constructor
     * \param viewWidget diagram view graph widget
     * \param layerWidget layers view graph widget
     * \param mainWindow parent main window
     * \param stackedWidget central widget in the main window (direct managing parent widget of the first two parameter widgets)
     * \param dragDropIsChecked current drag-n-drop mode boolean witness
     * \param vertexFocused boolean witness: true when there is a node currently focused on; false otherwise
     * \param edgeFocused boolean witness:  true when there is an edge currently focused on; false otherwise
     */
    PropertyDialog(ViewWidget *viewWidget, LayerWidget *layerWidget, QMainWindow *mainWindow, QStackedWidget *stackedWidget, bool dragDropIsChecked = false, bool vertexFocused = false, bool edgeFocused = false);
    /// destructor
    ~PropertyDialog();

    /// getter method for retrieving running status; returns true when the main panel is still in the scene; false otherwise
    bool isRunning() { return (&mDialog)->isVisible(); }
    /// setter method for drag-n-drop button boolean state - used by the viewWidget to sync the drag-n-drop mode with its button switch here in the command panel
    void setDragDrop(bool toggle) { mpDragDropButton->setChecked(toggle); }
    /// adds a horizontal separator
    void addFrame(QVBoxLayout* verticalLayout);
    /// adds a vertical separator
    void addFrame(QHBoxLayout* horizontalLayout);

    /// sets up all GUI components to the provided Dialog instance
    void setupUi(CustomDialog *Dialog, bool dragDropIsChecked = false);
    /// attributes meaningful titles to be displayed for all GUI components (e.g. buttons labels)
    void retranslateUi(CustomDialog *Dialog);

    /// setter method for the node-being-focused boolean witness
    void setVertexFocused(bool vertexFocused);
    /// setter method for the edge-being-focused boolean witness
    void setEdgeFocused(bool edgeFocused);

    /// getter method for the node-being-focused boolean witness
    bool getVertexFocused() { return mVertexFocused; }
    /// getter method for the edge-being-focused boolean witness
    bool getEdgeFocused  () { return mEdgeFocused;   }

public slots:
    /// switches the central widget to the widget of the given index: 0 for the ViewWidget; 1 for the LayerWidget
    void updateMainWidget(int index);

private:
    /// main qt dialog to be used
    CustomDialog mDialog;
    /// central widget in the main window (direct managing parent widget of the two widgets: the View- and the Layer-Widget)
    QStackedWidget* mpStackedWidget;
    /// diagram view graph widget
    ViewWidget *mpViewWidget;
    /// layers view graph widget
    LayerWidget *mpLayerWidget;
    /// parent main window
    QMainWindow *mpMainWindow;
    /// collection of tabs of the entire panel
    QTabWidget *mpTabWidget;
    /// the diagram view tab
    QWidget *mpHorizontalLayoutWidget;
    /// the layers view tab
    QWidget *mpHorizontalLayoutLayerWidget;
    /// main layout of the diagram view tab
    QHBoxLayout *mpHorizontalLayout;
    /// main layout of the layers view tab
    QHBoxLayout *mpHorizontalLayoutLayer;
    /// left hand-side vertical layout of the diagram view tab (hosts the general graph commands)
    QVBoxLayout *mpVerticalLayout;
    /// right hand-side vertical layout of the diagram view tab (hosts the focus-oriented node and edge specific commands)
    QVBoxLayout *mpVerticalLayoutFocus;
    /// left hand-side vertical layout of the layers view tab (hosts the general graph commands available on the layers view)
    QVBoxLayout *mpVerticalLayoutLayer;
    /// right hand-side vertical layout of the layers view tab (hosts the (layers) filtering specific commands)
    QVBoxLayout *mpVerticalLayoutComponentsLayer;
    /// button for spawning a new node
    QPushButton *mpAddNodeButton;
    /// button for refreshing the graph view in the diagram view tab
    QPushButton *mpRefreshButton;
    /// button for refreshing the graph view in the layers view tab
    QPushButton *mpRefreshLayerButton;
    /// button for shuffling all the nodes in the graph view in the diagram view tab
    QPushButton *mpShuffleButton;
    /// button for shuffling all the nodes in the graph view in the layers view tab
    QPushButton *mpShuffleLayerButton;
    /// button for loading a graph from file from the diagram view tab
    QPushButton *mpImportButton;
    /// button for loading a graph from file from the layers view tab
    QPushButton *mpImportLayerButton;
    /// button for saving a graph to file from the diagram view tab
    QPushButton *mpExportButton;
    /// button for saving a graph to file from the layers view tab
    QPushButton *mpExportLayerButton;
    /// button for resetting the graph from the diagram view tab
    QPushButton *mpResetButton;
    /// button for resetting the graph to file from the layers view tab
    QPushButton *mpResetLayerButton;
    /// button for chaging the layout of the diagram graph view
    QPushButton *mpLayoutButton;
    /// button for chaging the layout of the layers graph view
    QPushButton *mpLayoutLayerButton;
    /// button for toggling the drag-n-drop mode in the diagram view tab
    QPushButton *mpDragDropButton;
    /// button for renaming the focused node in the diagram view tab
    QPushButton *mpRenameNodeButton;
    /// button for adding a port to the focused node in the diagram view tab
    QPushButton *mpAddPortButton;
    /// button for renaming a port of the focused node in the diagram view tab
    QPushButton *mpRenamePortButton;
    /// button for removing a port of the focused node in the diagram view tab
    QPushButton *mpRemovePortButton;
    /// button for removing all ports of the focused node in the diagram view tab
    QPushButton *mpRemovePortsButton;
    /// button for removing the focused node in the diagram view tab
    QPushButton *mpRemoveNodeButton;
    /// button for renaming the focused edge in the diagram view tab
    QPushButton *mpRenameEdgeButton;
    /// button for removing the focused edge in the diagram view tab
    QPushButton *mpRemoveEdgeButton;
    /// button for swapping two ports of the focused node in the diagram view tab
    QPushButton *mpSwapPortsButton;
    /// GUI label displaying "Visible Layers:" in the layers view tab on top of the filtering commands
    QLabel *mpLayersLabel;
    /// button for toggling visibility of port vertices in the layers view tab
    QPushButton *mpTogglePortLayerButton;
    /// button for toggling visibility of cluster vertices in the layers view tab
    QPushButton *mpToggleClusterLayerButton;
    /// set of seeparators in all layouts used
    QFrames mFrames;
    /// boolean witness: true when a node is being focused on; false otherwise
    bool mVertexFocused;
    /// boolean witness: true when an edge is being focused on; false otherwise
    bool mEdgeFocused;
    /// current height of the panel in the scene
    int mHeight;
};

} // end namespace gui
} // end namespace graph_analysis

#endif // PROPERTYDIALOG_H
