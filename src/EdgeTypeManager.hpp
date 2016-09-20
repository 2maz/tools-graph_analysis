#ifndef GRAPH_ANALYSIS_EDGE_TYPE_MANAGER_HPP
#define GRAPH_ANALYSIS_EDGE_TYPE_MANAGER_HPP

#include <graph_analysis/Edge.hpp>
#include <base-logging/Singleton.hpp>
#include <map>
#include <set>
#include <string>

namespace graph_analysis {

class Edge;

namespace edge {
    // datatype for edge type specification
    typedef std::string Type;
} // end namespace edge

/**
 * \brief Factory for Edge subclasses
 * \details EdgeTypeManager allows to register edge classes by type
 * - which is a given string. When loading a serialized graph via GraphIO::read
 * the EdgeTypeManager allows to instanciate edges corresponding to the
 * given class type -- which has to match the type string.
 * Instanciation is done via cloning the corresponding edge instance.
 */
class EdgeTypeManager : public base::Singleton<EdgeTypeManager>
{
public:
    typedef std::map<edge::Type, Edge::Ptr> TypeMap;

private:
    /// registration map - stores the registered types, mapping them to the example edge instances (i.e. from which new ones to be forked on request)
    TypeMap mTypeMap;
    /// registration list - maintains a complete list of all registered types
    std::set<std::string> mRegisteredTypes;
    /**
     * \brief internal method for type identification
     * \param type requested edge type
     * \param throwOnDefault flag indicating whether exceptions shall be thrown when fed with unregistered types; on false it silently picks the default type
     * \return smart pointer to the witness edge instance of the requested type
     */
    Edge::Ptr edgeByType(const edge::Type& type, bool throwOnDefault = false);

protected:
    /// constructor
    EdgeTypeManager();
    friend class base::Singleton<EdgeTypeManager>;
public:

    // Register visualization class
    // takes ownership of graphicsItem
    void registerType(const edge::Type& type, Edge::Ptr edge, bool throwOnAlreadyRegistered = false);
    /**
     * \brief clones a new edge of a specified type
     * \param type the requested edge type
     * \param label the requested edge label
     * \param throw when edge type is missing
     * \return smart pointer to the newly created edge instance
     */
    Edge::Ptr createEdge(const edge::Type& type, const std::string& label = "", bool throwOnMissing = false);

    Edge::Ptr createEdge(const edge::Type& type, const Vertex::Ptr& source, const Vertex::Ptr& target
            , const std::string& label = ""
            , bool throwOnMissing = false);
    /// lists the registered types
    std::set<std::string> getSupportedTypes();
};

} // end namespace graph_analysis
#endif /* GRAPH_ANALYSIS_EDGE_TYPE_MANAGER_HPP */
