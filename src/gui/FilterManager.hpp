#ifndef FILTERMANAGER_HPP
#define FILTERMANAGER_HPP

#include <vector>
#include <string>
#include <QCheckBox>
#include <QGraphicsView>

#include "FilterItem.hpp"
#include "ViewWidget.hpp"
#include "LayerWidget.hpp"

namespace graph_analysis {
namespace gui {

class FilterManager : public QGraphicsView
{
    Q_OBJECT
public:
    typedef std::vector<FilterItem*> Filters;
    typedef std::vector<QCheckBox*>  CheckBoxes;
    typedef std::map<QCheckBox*, FilterItem::filter_index_t> CheckBoxIndexMap;

    FilterManager(ViewWidget *viewWidget, LayerWidget *layerWidget, QWidget *checkBoxGrid, QWidget *parent = 0);
    ~FilterManager();

    int filterCount() { return mFilters.size(); }
    void addFilter(const std::string& label, bool enable);
    void addFilter(const std::string& label);
    unsigned int createFilter(const std::string& label);
    void swapFilters(FilterItem::filter_index_t left, FilterItem::filter_index_t right);
    void dieOnIndex(FilterItem::filter_index_t index, const std::string& caller = std::string());
    void updateToolTip(FilterItem::filter_index_t index, bool witness);
    void refreshToolTip(FilterItem::filter_index_t index);

public slots:
    void addFilter();
    void removeFilters();
    void updateToolTip(int state);

    /// displays context menu (upon a right click on the scene)
    void showContextMenu(const QPoint& pos);

private:
    Filters mFilters;
    ViewWidget *mpViewWidget;
    LayerWidget *mpLayerWidget;
    QWidget *mpCheckBoxGrid;
    CheckBoxes mCheckBoxes;
    CheckBoxIndexMap mCheckBoxIndexMap;
};

} // end namespace gui
} // end namespace graph_analysis

#endif // FILTERMANAGER_HPP
