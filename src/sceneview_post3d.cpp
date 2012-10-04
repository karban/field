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

#include "sceneview_post3d.h"
#include "util.h"
#include "scene.h"
#include "field/problem.h"
#include "logview.h"

#include "scenebasic.h"
#include "scenenode.h"

static void computeNormal(double p0x, double p0y, double p0z,
                          double p1x, double p1y, double p1z,
                          double p2x, double p2y, double p2z,
                          double* normal)
{
    double ax = (p1x - p0x);
    double ay = (p1y - p0y);
    double az = (p1z - p0z);

    double bx = (p2x - p0x);
    double by = (p2y - p0y);
    double bz = (p2z - p0z);

    normal[0] = ay * bz - az * by;
    normal[1] = az * bx - ax * bz;
    normal[2] = ax * by - ay * bx;

    // normalize
    // double l = 1.0 / sqrt(sqr(nx) + sqr(ny) + sqr(nz));
    // double p[3] = { nx*l, ny*l, nz*l };
}

SceneViewPost3D::SceneViewPost3D(PostView *postHermes, QWidget *parent)
    : SceneViewCommon3D(postHermes, parent),
    m_listScalarField3D(-1),
    m_listParticleTracing(-1),
    m_listModel(-1)
{
    createActionsPost3D();

    connect(Util::scene(), SIGNAL(defaultValues()), this, SLOT(clear()));
    connect(Util::scene(), SIGNAL(cleared()), this, SLOT(clear()));

    connect(m_postHermes, SIGNAL(processed()), this, SLOT(refresh()));
}

SceneViewPost3D::~SceneViewPost3D()
{
}

void SceneViewPost3D::createActionsPost3D()
{
    actSceneModePost3D = new QAction(iconView(), tr("Post 3D"), this);
    actSceneModePost3D->setShortcut(Qt::Key_F8);
    actSceneModePost3D->setStatusTip(tr("Postprocessor 3D"));
    actSceneModePost3D->setCheckable(true);
}

void SceneViewPost3D::mousePressEvent(QMouseEvent *event)
{
    SceneViewCommon3D::mousePressEvent(event);
}

void SceneViewPost3D::paintGL()
{
    if (!isVisible()) return;
    makeCurrent();

    glClearColor(Util::config()->colorBackground.redF(),
                 Util::config()->colorBackground.greenF(),
                 Util::config()->colorBackground.blueF(), 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    if (Util::problem()->isSolved())
    {
        if (Util::config()->showPost3D == SceneViewPost3DMode_ScalarView3D) paintScalarField3D();

        // bars
        if (Util::config()->showPost3D == SceneViewPost3DMode_ScalarView3D)
            paintScalarFieldColorBar(-1, 1);
    }

    switch (Util::config()->showPost3D)
    {
    case SceneViewPost3DMode_ScalarView3D:
    {
        if (Util::problem()->isSolved())
        {
            emit labelCenter("TODO");
        }
    }
        break;
    case SceneViewPost3DMode_Model:
        emit labelCenter(tr("Model"));
        break;
    case SceneViewPost3DMode_ParticleTracing:
        emit labelCenter(tr("Particle tracing"));
        break;
    default:
        emit labelCenter(tr("Postprocessor 3D"));
    }

    if (Util::config()->showAxes) paintAxes();
}

void SceneViewPost3D::resizeGL(int w, int h)
{
    if (Util::problem()->isSolved())
    {
        paletteFilter(textureScalar());
        paletteCreate(textureScalar());
    }

    SceneViewCommon::resizeGL(w, h);
}

void SceneViewPost3D::paintScalarField3D()
{
    if (!Util::problem()->isSolved()) return;

    loadProjection3d(true);

    if (m_listScalarField3D == -1)
    {
        paletteFilter(textureScalar());
        paletteCreate(textureScalar());

        m_listScalarField3D = glGenLists(1);
        glNewList(m_listScalarField3D, GL_COMPILE);

        // gradient background
        paintBackground();

        glEnable(GL_DEPTH_TEST);

        glEndList();

        glCallList(m_listScalarField3D);
    }
    else
    {
        glCallList(m_listScalarField3D);
    }
}

void SceneViewPost3D::refresh()
{
    if (m_listScalarField3D != -1) glDeleteLists(m_listScalarField3D, 1);
    if (m_listModel != -1) glDeleteLists(m_listModel, 1);
    if (m_listParticleTracing != -1) glDeleteLists(m_listParticleTracing, 1);

    m_listScalarField3D = -1;
    m_listModel = -1;
    m_listParticleTracing = -1;

    // actions
    actSceneModePost3D->setEnabled(Util::problem()->isSolved());
    actSetProjectionXY->setEnabled(Util::problem()->isSolved());
    actSetProjectionXZ->setEnabled(Util::problem()->isSolved());
    actSetProjectionYZ->setEnabled(Util::problem()->isSolved());

    SceneViewCommon::refresh();
}

void SceneViewPost3D::clear()
{
    SceneViewCommon3D::clear();
}
