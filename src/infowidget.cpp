// This file is part of Agros2D.
//
// Agros2D is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Agros2D is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Agros2D.  If not, see <http://www.gnu.org/licenses/>.
//
// hp-FEM group (http://hpfem.org/)
// University of Nevada, Reno (UNR) and University of West Bohemia, Pilsen
// Email: agros2d@googlegroups.com, home page: http://hpfem.org/agros2d/

#include "infowidget.h"

#include "scene.h"
#include "scenebasic.h"
#include "scenenode.h"
#include "sceneview_common.h"
#include "sceneview_geometry.h"
#include "pythonlabagros.h"

#include "util/constants.h"
#include "gui/chart.h"

#include "ctemplate/template.h"

InfoWidget::InfoWidget(SceneViewPreprocessor *sceneView, QWidget *parent): QWidget(parent)
{
    this->m_sceneViewGeometry = sceneView;

    // problem information
    webView = new QWebView(this);
    connect(webView, SIGNAL(loadFinished(bool)), SLOT(finishLoading(bool)));

    QVBoxLayout *layoutMain = new QVBoxLayout(this);
    // layoutMain->setContentsMargins(0, 5, 3, 5);
    layoutMain->addWidget(webView);

    setLayout(layoutMain);

    connect(Util::scene(), SIGNAL(cleared()), this, SLOT(refresh()));

    connect(Util::problem(), SIGNAL(timeStepChanged()), this, SLOT(refresh()));
    connect(Util::problem(), SIGNAL(meshed()), this, SLOT(refresh()));
    connect(Util::problem(), SIGNAL(couplingsChanged()), this, SLOT(refresh()));

    connect(currentPythonEngineAgros(), SIGNAL(executedScript()), this, SLOT(refresh()));
    connect(currentPythonEngineAgros(), SIGNAL(executedExpression()), this, SLOT(refresh()));

    refresh();
}

InfoWidget::~InfoWidget()
{
}

void InfoWidget::refresh()
{
    QTimer::singleShot(0, this, SLOT(showInfo()));
}

void InfoWidget::showInfo()
{
    // stylesheet
    std::string style;
    ctemplate::TemplateDictionary stylesheet("style");
    stylesheet.SetValue("FONTFAMILY", QApplication::font().family().toStdString());
    stylesheet.SetValue("FONTSIZE", (QString("%1").arg(QApplication::font().pointSize() - 1).toStdString()));

    ctemplate::ExpandTemplate(datadir().toStdString() + TEMPLATEROOT.toStdString() + "/panels/problem_style.tpl", ctemplate::DO_NOT_STRIP, &stylesheet, &style);

    // template
    std::string info;
    ctemplate::TemplateDictionary problemInfo("info");

    problemInfo.SetValue("STYLESHEET", style);
    problemInfo.SetValue("BASIC_INFORMATION_LABEL", tr("Basic informations").toStdString());

    problemInfo.SetValue("NAME_LABEL", tr("Name:").toStdString());
    problemInfo.SetValue("NAME", Util::problem()->config()->name().toStdString());

    problemInfo.SetValue("GEOMETRY_LABEL", tr("Geometry").toStdString());
    problemInfo.SetValue("GEOMETRY_NODES_LABEL", tr("Nodes:").toStdString());
    problemInfo.SetValue("GEOMETRY_NODES", QString::number(Util::scene()->nodes->count()).toStdString());

    if (Util::problem()->isSolved())
    {
        problemInfo.SetValue("SOLUTION_LABEL", tr("Solution").toStdString());
        problemInfo.SetValue("SOLUTION_ELAPSED_TIME_LABEL", tr("Total elapsed time:").toStdString());
        // problemInfo.SetValue("SOLUTION_ELAPSED_TIME", tr("%1 s").arg(Util::problem()->timeElapsed().toString("mm:ss.zzz")).toStdString());
        problemInfo.SetValue("NUM_THREADS_LABEL", tr("Number of threads:").toStdString());
        // problemInfo.SetValue("NUM_THREADS", tr("%1").arg(Hermes::Hermes2D::Hermes2DApi.get_param_value(Hermes::Hermes2D::numThreads)).toStdString());
        problemInfo.ShowSection("SOLUTION_PARAMETERS_SECTION");
    }

    ctemplate::ExpandTemplate(datadir().toStdString() + TEMPLATEROOT.toStdString() + "/panels/problem.tpl", ctemplate::DO_NOT_STRIP, &problemInfo, &info);

    // setHtml(...) doesn't work
    // webView->setHtml(QString::fromStdString(info));

    // load(...) works
    writeStringContent(tempProblemDir() + "/info.html", QString::fromStdString(info));
    webView->load(tempProblemDir() + "/info.html");
}


void InfoWidget::finishLoading(bool ok)
{
    // adaptive error
    if (Util::problem()->isSolved())
    {
        webView->page()->mainFrame()->evaluateJavaScript(readFileContent(datadir() + TEMPLATEROOT + "/panels/js/jquery.js"));
        webView->page()->mainFrame()->evaluateJavaScript(readFileContent(datadir() + TEMPLATEROOT + "/panels/js/jquery.flot.js"));

        /*
        foreach (FieldInfo *fieldInfo, Util::problem()->fieldInfos())
        {
            if (fieldInfo->adaptivityType() != AdaptivityType_None)
            {
                int timeStep = Util::solutionStore()->timeLevels(fieldInfo).count() - 1;
                int adaptiveSteps = Util::solutionStore()->lastAdaptiveStep(fieldInfo, SolutionMode_Normal) + 1;

                QString dataDOFs = "[";
                QString dataError = "[";
                for (int i = 0; i < adaptiveSteps; i++)
                {
                    MultiSolutionArray<double> msa = Util::solutionStore()->multiSolution(FieldSolutionID(fieldInfo, timeStep, i, SolutionMode_Normal));

                    dataDOFs += QString("[%1, %2], ").arg(i+1).arg(Hermes::Hermes2D::Space<double>::get_num_dofs(castConst(desmartize(msa.spaces()))));
                    // dataError += QString("[%1, %2], ").arg(i+1).arg(msa.adaptiveError());
                }
                dataDOFs += "]";
                dataError += "]";

                // error
                QString prescribedError = QString("[[1, %1], [%2, %3]]").
                        arg(fieldInfo->adaptivityTolerance()).
                        arg(adaptiveSteps).
                        arg(fieldInfo->adaptivityTolerance());

                // chart error vs. steps
                QString commandError = QString("$(function () { $.plot($(\"#chart_error_steps_%1\"), [ { data: %2, color: \"rgb(61, 61, 251)\", lines: { show: true }, points: { show: true } }, { data: %3, color: \"rgb(240, 0, 0)\" } ], { grid: { hoverable : true } });})").
                        arg(fieldInfo->fieldId()).
                        arg(dataError).
                        arg(prescribedError);

                // chart DOFs vs. steps
                QString commandDOFs = QString("$(function () { $.plot($(\"#chart_dofs_steps_%1\"), [ { data: %2, color: \"rgb(61, 61, 251)\", lines: { show: true }, points: { show: true } } ], { grid: { hoverable : true } });})").
                        arg(fieldInfo->fieldId()).
                        arg(dataDOFs);

                webView->page()->mainFrame()->evaluateJavaScript(commandError);
                webView->page()->mainFrame()->evaluateJavaScript(commandDOFs);
            }
        }
        */
    }
}

