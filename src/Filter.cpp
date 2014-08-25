#include "Filter.hpp"
#include <boost/assign/list_of.hpp>

namespace graph_analysis {
namespace filters {

std::map<Type, std::string> TypeTxt = boost::assign::map_list_of
    (CONTENT, "CONTENT")
    (CLASS, "CLASS");

std::map<std::string, Type> TxtType = boost::assign::map_list_of
    ("CONTENT", CONTENT)
    ("CLASS", CLASS);

} // end namespace filters


bool EdgeContextFilter::filterTarget(graph_analysis::Edge::Ptr e) const
{
    if(evaluateTarget(e))
    {
        return true;
    }

    typename FilterList::const_iterator cit = mFilters.begin();
    for(; cit != mFilters.end(); ++cit)
    {
        Ptr edgeFilter = boost::dynamic_pointer_cast<EdgeContextFilter>(*cit);
        if(edgeFilter && edgeFilter->evaluateTarget(e) )
        {
            return true;
        }
    }
    return false;
}

bool EdgeContextFilter::filterSource(graph_analysis::Edge::Ptr e) const
{
    if(evaluateSource(e))
    {
        return true;
    }

    typename FilterList::const_iterator cit = mFilters.begin();
    for(; cit != mFilters.end(); ++cit)
    {
        Ptr edgeFilter = boost::dynamic_pointer_cast<EdgeContextFilter>(*cit);
        if(edgeFilter && edgeFilter->evaluateSource(e) )
        {
            return true;
        }
    }
    return false;
}

} // end namespace graph_analysis
