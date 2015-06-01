#ifndef GRAPH_ANALYSIS_BASEVERTEX_HPP
#define GRAPH_ANALYSIS_BASEVERTEX_HPP

#include <iostream>
#include <graph_analysis/Vertex.hpp>
#include <graph_analysis/GraphElement.hpp>

namespace graph_analysis
{

/**
 * \brief A Cluster vertex inherited to allow storing data of Cluster type
 * \details For the purpose of storing data with a vertex, this storage type inherits
 * from class Vertex
 */
class ClusterVertex : public Vertex
{
public:
    ClusterVertex(const std::string& label = "");
    std::string type() { return "cluster"; }
    /**
     * Clone this cluster vertex -- the returned cluster vertex will have no
     * graph association
     */
    Vertex::Ptr clone() const;

    /** Get class name
     * \return class name
     */
    virtual std::string getClassName() const { return "graph_analysis::ClusterVertex"; }

protected:
    /**
     * Create a copy of this vertex
     */
    virtual ClusterVertex* getClone() const { return new ClusterVertex(*this); }

};

} // end namespace graph_analysis
#endif // GRAPH_ANALYSIS_BASEVERTEX_HPP
