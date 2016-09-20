#ifndef GRAPHICSITEMTYPES_H
#define GRAPHICSITEMTYPES_H

#include <QGraphicsItemGroup>

namespace graph_analysis {
namespace gui {

enum GraphicsItemTypes {
    NodeItemType = QGraphicsItem::UserType + 1,
    EdgeItemType,
    FeatureItemType,
    VertexItemBaseType,
    VertexItemSimpleType,
    EdgeItemBaseType,
    EdgeItemSimpleType,
    UserType
};

}
}

#endif /*GRAPHICSITEMTYPES_H*/
