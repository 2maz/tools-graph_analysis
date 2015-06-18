#ifndef GRAPH_ANALYSIS_BASEVERTEX_HPP
#define GRAPH_ANALYSIS_BASEVERTEX_HPP

#include <iostream>
#include <graph_analysis/Vertex.hpp>
#include <graph_analysis/GraphElement.hpp>

namespace graph_analysis
{

/**
 * \brief A Base vertex inherited to allow storing data of Base type
 * \details For the purpose of storing data with a vertex, this storage type inherits
 * from class Vertex
 */
class BaseVertex : public Vertex
{
public:
    BaseVertex(const std::string& label = "");
    /** Get class name
     * \return class name
     */
    virtual std::string getClassName() const { return "graph_analysis::BaseVertex"; }

protected:
    /**
     * Create a copy of this vertex
     */
    virtual BaseVertex* getClone() const { return new BaseVertex(*this); }

};

} // end namespace graph_analysis
#endif // GRAPH_ANALYSIS_BASEVERTEX_HPP
