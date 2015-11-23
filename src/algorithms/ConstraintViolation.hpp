#ifndef GRAPH_ANALYSIS_ALGORITHMS_CONSTRAINT_VIOLATION_HPP
#define GRAPH_ANALYSIS_ALGORITHMS_CONSTRAINT_VIOLATION_HPP

#include <map>
#include <graph_analysis/algorithms/MultiCommodityVertex.hpp>

namespace graph_analysis {
namespace algorithms {

class ConstraintViolation
{
public:
    enum Type { MinFlow, TransFlow };

    static std::map<Type, std::string> TypeTxt;

    ConstraintViolation(const MultiCommodityVertex::Ptr& v, uint32_t commodity, int32_t delta, Type type = MinFlow);

    uint32_t getCommodity() const { return mCommodity; }
    const Vertex::Ptr& getVertex() const { return mpVertex; }
    int32_t getDelta() const { return mDelta; }
    Type getType() const { return mType; }

    std::string toString() const;

private:
    Vertex::Ptr mpVertex;
    uint32_t mCommodity;
    int32_t mDelta;
    Type mType;
};

} // end namespace algorithms
} // end namespace graph_analysis
#endif // GRAPH_ANALYSIS_ALGORITHMS_CONSTRAINT_VIOLATION_HPP
