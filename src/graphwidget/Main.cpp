/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <omviz/graphwidget/NodeItem.hpp>
#include <omviz/graphwidget/GraphWidget.hpp>

#include <QApplication>
#include <QTime>
#include <QMainWindow>

#include <omviz/IRINode.hpp>
#include <omviz/graphwidget/graphitem/Resource.hpp>

int main(int argc, char **argv)
{
    using namespace omviz;

    QApplication app(argc, argv);
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));

    GraphWidget* widget = new GraphWidget;

    // Create vertices
    for(int i = 0; i < 10; ++i)
    {
        graph_analysis::Vertex::Ptr vertex(new graph_analysis::Vertex());
        widget->addVertex(vertex);
    }

    //  = owl_om::Ontology::fromFile(argv[1]);
    owl_om::Ontology::Ptr ontology(new owl_om::Ontology());


    owl_om::IRI resourceClass = "http://qt-test/om#Resource";
    ontology->subclassOf(resourceClass,"TOP");

    NodeItem::nodeTypeManager().registerVisualization(resourceClass.toString(), new graphitem::Resource());

    for(int i = 0; i < 10; ++i)
    {
        std::stringstream ss;
        ss << "http://qt-test/om#resource-instance-" << i;
        owl_om::IRI instance = ss.str();
        ontology->instanceOf(instance, resourceClass);

        omviz::IRINode::Ptr node(new IRINode(instance, ontology));
        widget->addVertex(node);
    }


    // Register nodes -- otherwise edgesItemMap will remains empty
    widget->updateFromGraph();

    // Create edges between all
    GraphWidget::NodeItemMap::const_iterator ait = widget->nodeItemMap().begin();

    for(; ait != widget->nodeItemMap().end(); ++ait)
    {
        GraphWidget::NodeItemMap::const_iterator bit = widget->nodeItemMap().begin();
        for(; bit != widget->nodeItemMap().end(); ++bit)
        {
            graph_analysis::Edge::Ptr edge(new graph_analysis::Edge());
            edge->setSourceVertex(ait->first);
            edge->setTargetVertex(bit->first);

            widget->addEdge(edge);
        }
    }

    widget->updateFromGraph();
    widget->shuffle();

    QMainWindow mainWindow;
    mainWindow.setCentralWidget(widget);

    mainWindow.show();
    return app.exec();
}
