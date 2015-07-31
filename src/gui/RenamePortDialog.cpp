#include "RenamePortDialog.hpp"
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>


namespace graph_analysis {
namespace gui {

RenamePortDialog::RenamePortDialog(NodeItem *nodeItem)
: mpNodeItem(nodeItem)
, mValid(false)
{
    setupUi(&mDialog);
    mDialog.exec();
}

void RenamePortDialog::setupUi(QDialog *Dialog)
{
    if (Dialog->objectName().isEmpty())
        Dialog->setObjectName(QString::fromUtf8("Dialog"));
    Dialog->resize(441, 169);
    mpButtonBox = new QDialogButtonBox(Dialog);
    mpButtonBox->setObjectName(QString::fromUtf8("mpButtonBox"));
    mpButtonBox->setGeometry(QRect(230, 120, 181, 32));
    mpButtonBox->setOrientation(Qt::Horizontal);
    mpButtonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
    mpNewLabel = new QLabel(Dialog);
    mpNewLabel->setObjectName(QString::fromUtf8("mpNewLabel"));
    mpNewLabel->setGeometry(QRect(20, 16, 91, 31));
    mpLineEdit = new QLineEdit(Dialog);
    mpLineEdit->setObjectName(QString::fromUtf8("mpLineEdit"));
    mpLineEdit->setGeometry(QRect(120, 20, 291, 27));
    mpPortLabel = new QLabel(Dialog);
    mpPortLabel->setObjectName(QString::fromUtf8("mpPortLabel"));
    mpPortLabel->setGeometry(QRect(20, 70, 91, 31));
    mpComboBox = new QComboBox(Dialog);
    // fetching all ports in the concerned cluster node
    foreach(NodeItem::VTuple tuple, mpNodeItem->getVertices())
    {
        std::string item = boost::lexical_cast<std::string>(tuple.first) + ": " + tuple.second->getLabel();
        mpComboBox->addItem(QString(item.c_str()));
    }
    mpComboBox->setObjectName(QString::fromUtf8("mpComboBox"));
    mpComboBox->setGeometry(QRect(117, 70, 291, 27));

    // attributing relevant titles to be displayed in the scene (e.g. buttons labels)
    retranslateUi(Dialog);
    // establishing signal-slot connections
    QObject::connect(mpButtonBox, SIGNAL(accepted()), this, SLOT(renameAccept()));
    QObject::connect(mpButtonBox, SIGNAL(rejected()), this, SLOT(renameReject()));
    QObject::connect(mpButtonBox, SIGNAL(accepted()), Dialog, SLOT(accept()));
    QObject::connect(mpButtonBox, SIGNAL(rejected()), Dialog, SLOT(reject()));

    QMetaObject::connectSlotsByName(Dialog);
} // setupUi

void RenamePortDialog::retranslateUi(QDialog *Dialog)
{
    Dialog->setWindowTitle(QApplication::translate("Dialog", "Rename Port", 0, QApplication::UnicodeUTF8));
    mpNewLabel->setText(QApplication::translate("Dialog", "New Label:", 0, QApplication::UnicodeUTF8));
    mpPortLabel->setText(QApplication::translate("Dialog", "Port ID:", 0, QApplication::UnicodeUTF8));
} // retranslateUi

void RenamePortDialog::renameAccept()
{
    mValid = true;
    mNewLabel = mpLineEdit->text().toStdString();
    mPortID  = mpComboBox->currentText().toStdString();
}

void RenamePortDialog::renameReject()
{
    mValid = false;
}

} // end namespace gui
} // end namespace graph_analysis
