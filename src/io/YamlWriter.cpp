#include "YamlWriter.hpp"
#include <base-logging/Logging.hpp>
#include <boost/lexical_cast.hpp>
#include <graph_analysis/GraphElement.hpp>

namespace graph_analysis {
namespace io {

void YamlWriter::write(const std::string& filename, const BaseGraph& graph) const
{
    BaseGraph graph_copy = graph;
    write(filename, BaseGraph::Ptr(&graph_copy)); // reusing code
}

void YamlWriter::write(const std::string& filename, const BaseGraph::Ptr& graph) const
{
    std::string file(filename);
    if(std::string::npos == file.find(".yml") && std::string::npos == file.find(".yaml"))
    {   // forcing proper file extension
        file += std::string(".yml");
    }
    const char *fname = file.c_str();
    std::ofstream fout(fname);
    if(!fout.good())
    {
        LOG_ERROR_S << "failed to open output file '" << filename
                    << "' for graph export";
        return;
    }
    LOG_INFO_S << "rendering graph to file '" << fname << "'";
    // no file header
    VertexIterator::Ptr nodeIt = graph->getVertexIterator();
    fout << "nodes:\n";
    while(nodeIt->next()) // outputting nodes
    {
        Vertex::Ptr vertex = nodeIt->current();
        exportVertex(graph, fout, vertex);
    }

    EdgeIterator::Ptr edgeIt = graph->getEdgeIterator();
    fout << "edges:" << std::endl;
    while(edgeIt->next()) // outputting edges
    {
        Edge::Ptr edge = edgeIt->current();
        exportEdge(graph, fout, edge);
    }
    if(fout.is_open())
    {
        fout.close();
    }
}

void YamlWriter::exportVertex(const BaseGraph::Ptr& graph, std::ofstream& fout, Vertex::Ptr vertex) const
{
    fout << "  - id: "      << graph->getVertexId(vertex)   << std::endl;
    fout << "    type: "    << vertex->getClassName()       << std::endl;
    fout << "    label: "   << vertex->getLabel()           << std::endl;
}

void YamlWriter::exportEdge(const BaseGraph::Ptr& graph, std::ofstream& fout, Edge::Ptr edge) const
{
    fout << "  - fromNodeId: "  << graph->getVertexId(edge->getSourceVertex())  << std::endl;
    fout << "    toNodeId: "    << graph->getVertexId(edge->getTargetVertex())  << std::endl;
    fout << "    label: "       << edge->getLabel()                             << std::endl;
}

} // end namespace io
} // end namespace graph_analysis
