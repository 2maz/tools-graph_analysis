/********************************************************************************
** Form generated from reading UI file 'add_node_dialog_gui.ui'
**
** Created by: Qt User Interface Compiler version 4.8.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef RENAMEFEATUREDIALOG_H
#define RENAMEFEATUREDIALOG_H

#include <set>
#include <string>
#include <QObject>
#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <graph_analysis/gui/NodeItem.hpp>
#include <graph_analysis/VertexTypeManager.hpp>

namespace graph_analysis {
namespace gui {

    class NodeItem;

/**
 * \class RenameFeatureDialog
 * \file RenameFeatureDialog.hpp
 * \brief pop-up dialog manager for renaming a feature of a cluster node
 */
class RenameFeatureDialog : public QObject
{
    Q_OBJECT
public:
    /// constructor; stores the concerned cluster node reference
    RenameFeatureDialog(NodeItem *nodeItem);
    /// destructor
    ~RenameFeatureDialog(){}
    /// sets up the provided qt dialog instance
    void setupUi(QDialog *Dialog);
    /// sets up visible titles for the scene (e.g. buttons labels)
    void retranslateUi(QDialog *Dialog);
    /// getter method for boolean validation witness
    bool isValid() { return mValid; }
    /// getter method for the new feature label dialog result
    std::string getNewLabel() { return mNewLabel; }
    /// getter method for the node type dialog result
    std::string getFeatureID () { return mFeatureID;  }

public slots:
    /// slot for listening for whenever the user presses "Ok" in the resulting pop-up dialog window
    void renameAccept();
    /// slot for listening for whenever the user presses "Cancel" (or ESC/close window corner button) in the resulting pop-up dialog window
    void renameReject();

private:
    /// main qt dialog instance to be used
    QDialog mDialog;
    /// stores the new feature label user-interaction result
    std::string mNewLabel;
    /// stores the feature node ID user-interaction result
    std::string mFeatureID;

    /// concerned cluster node
    NodeItem *mpNodeItem;
    /// boolean witness for validation; true whenever the user-interaction ends with the "Ok" button being pressed; false otherwise
    bool mValid;
    /// in-scene set of buttons for this pop-window ("Ok" and "Cancel")
    QDialogButtonBox *mpButtonBox;
    /// in scene label displaying "New Label:"
    QLabel *mpNewLabel;
    /// in scene label displaying "Feature ID:"
    QLabel *mpFeatureLabel;
    /// GUI element where the user types the new desired feature label
    QLineEdit *mpLineEdit;
    /// GUI element from which the user selects the desired feature to be renamed
    QComboBox *mpComboBox;
};

} // end namespace gui
} // end namespace graph_analysis
#endif // RENAMEFEATUREDIALOG_H
