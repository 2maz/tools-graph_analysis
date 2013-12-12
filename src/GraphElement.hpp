#ifndef GRAPH_ANALYSIS_GRAPH_ELEMENT_HPP
#define GRAPH_ANALYSIS_GRAPH_ELEMENT_HPP

#include <vector>
#include <map>
#include <stdint.h>
#include <stdexcept>
#include <boost/shared_ptr.hpp>
#include <iostream>
#include <sstream>

namespace graph_analysis {

typedef uint32_t GraphElementId;
typedef uint32_t GraphId;
typedef std::map<GraphId, GraphElementId> GraphElementMap;
typedef std::vector<GraphId> GraphIdList;

class GraphElement
{
public:
    /**
     * \brief Default constructor
     */
    GraphElement();

    typedef boost::shared_ptr< GraphElement > Ptr;

    // Get class name
    // \return class name
    virtual std::string getClassName() const { return "graph_analysis::GraphElement"; }

    /**
     * Convert element to String
     */
    virtual std::string toString() const { return getClassName(); }

    /**
     * Test whether this property has been associated with
     * an edge or node
     */
    bool associated(GraphId graph) const { return mGraphElementMap.count(graph); }

    /**
     * Add the element to a corresponding graph
     * This allows reverse mapping from the property to the graphs it belongs to
     */
    void associate(GraphId graph, GraphElementId elementId) { mGraphElementMap[graph] = elementId; }

    /**
     * Remove the edge that corresponds to a given graph
     */
    void disassociate(GraphId graph) { mGraphElementMap.erase(graph); }

    /**
     * Get id of this element within a given graph
     */
    GraphElementId getId(GraphId graph) const;

    /**
     * Get a unique id of this vertex
     */
    GraphElementId getUid() const { return mUid; }

    /**
     * Get list of graph associations this element has
     */
    GraphIdList getGraphAssociations() const;

protected:
    GraphElementMap mGraphElementMap;

    GraphElementId mUid;
    static GraphElementId msUid;
};

} // end namespace graph_analysis
#endif // GRAPH_ANALYSIS_GRAPH_ELEMENT_HPP
