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

#include <graph_analysis/gui/NodeItem.hpp>
#include <graph_analysis/gui/GraphWidget.hpp>

#include <QApplication>
#include <QTime>
#include <QMainWindow>

#include <graph_analysis/gui/graphitem/Resource.hpp>
#include <boost/foreach.hpp>
#include <graph_analysis/filters/RegexFilters.hpp>
#define TESTNO 1
//#define RENDER

//  possible layouts: circo, dot, fdp, neato, osage, sfdp, twopi
#define LAYOUT "dot"

int main(int argc, char **argv)
{
    using namespace graph_analysis::gui;

    QApplication app(argc, argv);
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));

    GraphWidget* widget = new GraphWidget;
    widget->reset();

    using namespace graph_analysis;

    filters::VertexRegexFilter sourceNodeFilter(".*");
    filters::VertexRegexFilter targetNodeFilter(".*");
    filters::EdgeRegexFilter edgeFilter(".*");

    filters::EdgeContextFilter::Ptr filter(new filters::CombinedEdgeRegexFilter(sourceNodeFilter, edgeFilter, targetNodeFilter));
    std::vector< Filter< graph_analysis::Edge::Ptr >::Ptr > edgeFilters;
    edgeFilters.push_back(filter);


    int testNo = TESTNO;
    switch(testNo)
    {
        case 1:
            /*
             * Test 1.:
             * -------
             */
            Vertex::Ptr v0(new Vertex());
            Vertex::Ptr v1(new Vertex());
            Vertex::Ptr v2(new Vertex());
            Vertex::Ptr v3(new Vertex());
            Vertex::Ptr v4(new Vertex());
            
            Edge::Ptr edge0(new Edge());
            Edge::Ptr edge1(new Edge());
            Edge::Ptr edge2(new Edge());
            Edge::Ptr edge3(new Edge());
            Edge::Ptr edge4(new Edge());
            Edge::Ptr edge5(new Edge());
            Edge::Ptr edge6(new Edge());

            edge0->setSourceVertex(v0);
            edge0->setTargetVertex(v1);

            edge1->setSourceVertex(v0);
            edge1->setTargetVertex(v3);

            edge2->setSourceVertex(v1);
            edge2->setTargetVertex(v2);

            edge3->setSourceVertex(v1);
            edge3->setTargetVertex(v4);

            edge4->setSourceVertex(v2);
            edge4->setTargetVertex(v3);

            edge5->setSourceVertex(v3);
            edge5->setTargetVertex(v0);

            edge6->setSourceVertex(v3);
            edge6->setTargetVertex(v4);

            widget->addVertex(v0);
            widget->addVertex(v1);
            widget->addVertex(v2);
            widget->addVertex(v3);
            widget->addVertex(v4);

            widget->addEdge(edge0);
            widget->addEdge(edge1);
            widget->addEdge(edge2);
            widget->addEdge(edge3);
            widget->addEdge(edge4);
            widget->addEdge(edge5);
            widget->addEdge(edge6);
        break;
    }

    widget->setEdgeFilters(edgeFilters);
    widget->updateFromGraph();
    widget->setLayout(QString(LAYOUT));
#ifdef RENDER
    widget->toFile("graph.dot");
#endif
    QMainWindow mainWindow;
    mainWindow.setCentralWidget(widget);

    mainWindow.show();
    return app.exec();
}
