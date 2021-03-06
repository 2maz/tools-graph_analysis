#ifndef GRAPH_ANALYSIS_VERTEX_TYPE_MANAGER_HPP
#define GRAPH_ANALYSIS_VERTEX_TYPE_MANAGER_HPP

#include <map>
#include <set>
#include <string>
#include <base-logging/Singleton.hpp>

#include "Vertex.hpp"
#include "AttributeManager.hpp"

namespace graph_analysis {

namespace vertex {
    // datatype for vertex type specification
    typedef std::string Type;
} // end namespace vertex

/**
 * \class VertexTypeManager
 * \brief Factory for Vertex subclasses
 * \details VertexTypeManager allows to register vertex classes by type
 * - which is a given string. When loading a serialized graph via GraphIO::read
 * the VertexTypeManager allows to instanciate vertices corresponding to the
 * given class type -- which has to match the type string.
 * Instanciation is done via cloning the corresponding vertex instance.
 */
class VertexTypeManager : public base::Singleton<VertexTypeManager>, public AttributeManager
{
public:
    typedef std::map<vertex::Type, Vertex::Ptr> TypeMap;

private:

    /// registration map - stores the registered types, mapping them to the example vertex instances (i.e. from which new ones to be forked on request)
    TypeMap mTypeMap;
    /// registration list - maintains a complete list of all registered types
    std::set<std::string> mRegisteredTypes;
    std::string mDefaultVertexType;

    /**
     * \brief internal method for type identification
     * \param type requested vertex type
     * \param throwOnMissing flag indicating whether exceptions shall be thrown when fed with unregistered types; on false it silently picks the default type
     * \return smart pointer to the witness vertex instance of the requested type
     */
    Vertex::Ptr vertexByType(const vertex::Type& type, bool throwOnMissing = false);

protected:
    /// constructor
    VertexTypeManager();
    friend class base::Singleton<VertexTypeManager>;

public:
    /**
     * Select the default vertex type from the list of registered types
     */
    void setDefaultType(const std::string& type);

    /**
     * Get the default vertex type
     */
    const std::string& getDefaultType() const { return mDefaultVertexType; }

    // Register vertex class
    void registerType(const Vertex::Ptr& vertex, bool throwOnAlreadyRegistered = false);

    // Register vertex class
    void registerType(const vertex::Type& type, const Vertex::Ptr& vertex, bool throwOnAlreadyRegistered = false);

    /**
     * \brief clones a new vertex of a specified type
     * \param type the requested vertex type
     * \param label the requested vertex label
     * \param throwOnMissing Throw exception when the vertex type is unknown,
     * otherwise a default vertex will be created
     * \return smart pointer to the newly created vertex instance
     */
    Vertex::Ptr createVertex(const vertex::Type& type, const std::string& label = std::string(),
            bool throwOnMissing = false);

    /**
     * Lists the registered types
     * \return list of registered types
     */
    std::set<std::string> getSupportedTypes();
};

} // end namespace graph_analysis
#endif /* GRAPH_ANALYSIS_VERTEX_TYPE_MANAGER_HPP */
