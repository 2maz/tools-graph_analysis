
#include "FilterManager.hpp"

#include <QRectF>
#include <QSizeF>
#include <base/Logging.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

namespace graph_analysis {
namespace gui {

FilterManager::FilterManager(QWidget *checkBoxGrid, QWidget *parent)
    : QGraphicsView(parent)
    , mpCheckBoxGrid(checkBoxGrid)
{
    QGraphicsScene *custom_scene = new QGraphicsScene(this);
    custom_scene->setItemIndexMethod(QGraphicsScene::NoIndex);
    setScene(custom_scene);

    setCacheMode(CacheBackground);
//    setContextMenuPolicy(Qt::CustomContextMenu);
    setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);
    setViewportUpdateMode(BoundingRectViewportUpdate);
    setAlignment(Qt::AlignLeft | Qt::AlignTop);
    setRenderHint(QPainter::Antialiasing);
    setTransformationAnchor(AnchorUnderMouse);
//    scale(qreal(0.8), qreal(0.8));
    setMinimumWidth(110);


    // populating the filter board with 3 example filters: filter1, filter2 and filter3
    addFilter("filter1");
    addFilter("filter2");
    addFilter("filter3");
    addFilter("filter3");
    addFilter("filter3");
}

FilterManager::~FilterManager()
{
}

void FilterManager::addFilter(const std::string& label)
{
    // introducing a new filter item
    mFilters.push_back(new FilterItem(this, mFilters.size(), label));
    scene()->addItem(mFilters.back());
    mFilters.back()->setPos(0.,  FilterItem::sHeight * (qreal)(mFilters.size() - 1));
    // introducing its corresponding enabling checkbox
    QCheckBox *newCheckBox = new QCheckBox(mpCheckBoxGrid);
    mpCheckBoxGrid->setFixedHeight((mCheckBoxes.size() + 1) * FilterItem::sHeight);
    newCheckBox->setGeometry(0, mCheckBoxes.size() * FilterItem::sHeight, FilterItem::sHeight, FilterItem::sHeight);
    mCheckBoxes.push_back(newCheckBox);
}

void FilterManager::pushDown(FilterItem::filter_index_t index)
{
    dieOnIndex(index, "pushDown");
    if(mFilters.size() - 1 == index)
    {
        LOG_WARN_S << "graph_analysis::gui::FilterManager::pushDown: the provided index = " << index << " is bottom-most already! Skipping...";
        return;
    }
    FilterItem* cached_filter = mFilters[index];
    mFilters[index] = mFilters[index + 1];
    mFilters[index + 1] = cached_filter;

    cached_filter->setPos(cached_filter->pos() + QPointF(0., (qreal)FilterItem::sHeight));
    cached_filter->setIndex(index + 1);
}

void FilterManager::pushUp(FilterItem::filter_index_t index)
{
    dieOnIndex(index, "pushUp");
    if(0 == index)
    {
        LOG_WARN_S << "graph_analysis::gui::FilterManager::pushUp: the provided index = " << index << " is top-most already! Skipping...";
        return;
    }
    FilterItem* cached_filter = mFilters[index];
    mFilters[index] = mFilters[index - 1];
    mFilters[index - 1] = cached_filter;

    cached_filter->setPos(cached_filter->pos() - QPointF(0., (qreal)FilterItem::sHeight));
    cached_filter->setIndex(index - 1);
}

void FilterManager::dieOnIndex(FilterItem::filter_index_t index, const std::string& caller)
{
    if(index < 0 || index >= mFilters.size())
    {
        std::string method = ("" == caller) ? "dieOnIndex" : caller;
        std::string error_msg = std::string("graph_analysis::gui::FilterManager::") + method + ": the supplied index: "
                                        + boost::lexical_cast<std::string>(index)
                                        + " is out of bounds";
        LOG_ERROR_S << error_msg;
        throw std::runtime_error(error_msg);
    }
}

} // end namespace gui
} // end namespace graph_analysis
