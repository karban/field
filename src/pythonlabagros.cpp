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

#include "pythonlabagros.h"

#include <Python.h>
#include "../resources_source/python/field.cpp"

#include "scene.h"
#include "sceneview_common.h"
#include "sceneview_geometry.h"
#include "sceneview_post3d.h"
#include "scenenode.h"

#include "field/problem.h"

#include "util/constants.h"

// current python engine agros
PythonEngineField *currentPythonEngineAgros()
{
    return static_cast<PythonEngineField *>(currentPythonEngine());
}

QString createPythonFromModel()
{
    QString str;

    return str;
}

// *******************************************************************************************

void PythonEngineField::addCustomExtensions()
{
    // init field cython extensions
    initfield();
}

void PythonEngineField::runPythonHeader()
{
    QString script;

    // global script
    if (!Util::config()->globalScript.isEmpty())
        script += Util::config()->globalScript + "\n";

    // startup script
    if (!Util::problem()->config()->startupscript().isEmpty())
        script += Util::problem()->config()->startupscript() + "\n";

    // run script
    if (!script.isEmpty())
        PyRun_String(script.toStdString().c_str(), Py_file_input, m_dict, m_dict);
}

PythonLabAgros::PythonLabAgros(PythonEngine *pythonEngine, QStringList args, QWidget *parent)
    : PythonEditorDialog(pythonEngine, args, parent)
{
    // add create from model
    actCreateFromModel = new QAction(icon("script-create"), tr("&Create script from model"), this);
    actCreateFromModel->setShortcut(QKeySequence(tr("Ctrl+M")));
    connect(actCreateFromModel, SIGNAL(triggered()), this, SLOT(doCreatePythonFromModel()));

    mnuTools->addSeparator();
    mnuTools->addAction(actCreateFromModel);

    tlbTools->addSeparator();
    tlbTools->addAction(actCreateFromModel);
}

void PythonLabAgros::doCreatePythonFromModel()
{
    txtEditor->setPlainText(createPythonFromModel());
}

// ***********************************************************

PyProblem::PyProblem(bool clearproblem)
{
    if (clearproblem)
        clear();
}

void PyProblem::clear()
{
    Util::problem()->clearFieldsAndConfig();
    Util::scene()->clear();
}

void PyProblem::refresh()
{
    Util::scene()->invalidate();

}

void PyProblem::solve()
{
    Util::scene()->invalidate();

    // trigger preprocessor
    currentPythonEngineAgros()->sceneViewPreprocessor()->actSceneModePreprocessor->trigger();

    Util::problem()->solve();
    if (Util::problem()->isSolved())
    {
        // trigger postprocessor
        currentPythonEngineAgros()->sceneViewPost3D()->actSceneModePost3D->trigger();
    }
}

// **************************************************************************************************

char *pyVersion()
{
    return const_cast<char*>(QApplication::applicationVersion().toStdString().c_str());
}

void pyQuit()
{
    // doesn't work without main event loop (run from script)
    // QApplication::exit(0);

    exit(0);
}

char *pyInput(char *str)
{
    QString text = QInputDialog::getText(QApplication::activeWindow(), QObject::tr("Script input"), QString(str));
    return const_cast<char*>(text.toStdString().c_str());
}

void pyMessage(char *str)
{
    QMessageBox::information(QApplication::activeWindow(), QObject::tr("Script message"), QString(str));
}

void pyOpenDocument(char *str)
{
    ErrorResult result = Util::scene()->readFromFile(QString(str));
    if (result.isError())
        throw invalid_argument(result.message().toStdString());
}

void pySaveDocument(char *str)
{
    ErrorResult result = Util::scene()->writeToFile(QString(str));
    if (result.isError())
        throw invalid_argument(result.message().toStdString());
}

void pyCloseDocument()
{
    Util::scene()->clear();
    // sceneView()->doDefaultValues();
    Util::scene()->invalidate();

    currentPythonEngineAgros()->sceneViewPreprocessor()->actSceneModePreprocessor->trigger();

    currentPythonEngineAgros()->sceneViewPreprocessor()->doZoomBestFit();
    currentPythonEngineAgros()->sceneViewPost3D()->doZoomBestFit();
}

void pySaveImage(char *str, int w, int h)
{
    // ErrorResult result = sceneView()->saveImageToFile(QString(str), w, h);
    // if (result.isError())
    //    throw invalid_argument(result.message().toStdString());
}
