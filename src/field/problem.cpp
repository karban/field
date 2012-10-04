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

#include "problem.h"

#include "scene.h"
#include "scenebasic.h"
#include "scenenode.h"
#include "logview.h"

ProblemConfig::ProblemConfig(QWidget *parent) : QObject(parent)
{
    clear();
}

void ProblemConfig::clear()
{
    m_name = QObject::tr("unnamed");
    m_fileName = "";
    m_startupscript = "";
    m_description = "";

    // harmonic
    m_frequency = 0.0;
}


Problem::Problem()
{
    m_timeStep = 0;
    m_timeElapsed = QTime(0, 0);
    m_isSolved = false;
    m_isSolving = false;

    m_config = new ProblemConfig();

    connect(m_config, SIGNAL(changed()), this, SLOT(clearSolution()));

    actClearSolutions = new QAction(icon(""), tr("Clear solutions"), this);
    actClearSolutions->setStatusTip(tr("Clear solutions"));
    connect(actClearSolutions, SIGNAL(triggered()), this, SLOT(clearSolution()));       
}

Problem::~Problem()
{
    clearSolution();
    clearFieldsAndConfig();

    delete m_config;
}

void Problem::clearSolution()
{
    m_isSolved = false;
    m_isSolving = false;
    m_timeStep = 0;
    m_timeElapsed = QTime(0, 0);
}

void Problem::clearFieldsAndConfig()
{
    clearSolution();

    // clear config
    m_config->clear();
}

void Problem::solve()
{
    if (isSolving())
        return;

    // start
    QTime elapsedTime;
    elapsedTime.start();

    //setActualTime(0.);

    // TODO: solve

    // delete temp file
    if (config()->fileName() == tempProblemFileName() + ".a2d")
    {
        QFile::remove(config()->fileName());
        config()->setFileName("");
    }

    m_isSolving = false;

    m_timeElapsed = milisecondsToTime(elapsedTime.elapsed());

    // close indicator progress
    Indicator::closeProgress();
}
