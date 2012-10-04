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

#include "sceneview_geometry.h"
#include "util.h"
#include "scene.h"
#include "field/problem.h"

#include "scenebasic.h"
#include "scenenode.h"

#include "gl2ps/gl2ps.h"

SceneViewPreprocessor::SceneViewPreprocessor(QWidget *parent)
    : SceneViewCommon3D(NULL, parent)
{
    createActionsGeometry();
    createMenuGeometry();
}

SceneViewPreprocessor::~SceneViewPreprocessor()
{
}

void SceneViewPreprocessor::createActionsGeometry()
{
    actSceneModePreprocessor = new QAction(iconView(), tr("Preprocessor"), this);
    actSceneModePreprocessor->setShortcut(Qt::Key_F5);
    actSceneModePreprocessor->setStatusTip(tr("Preprocessor"));
    actSceneModePreprocessor->setCheckable(true);

    // scene - operate on items
    actOperateOnNodes = new QAction(icon("scene-node"), tr("Operate on &nodes"), this);
    actOperateOnNodes->setShortcut(Qt::Key_F2);
    actOperateOnNodes->setStatusTip(tr("Operate on nodes"));
    actOperateOnNodes->setCheckable(true);
    actOperateOnNodes->setChecked(true);

    actOperateGroup = new QActionGroup(this);
    actOperateGroup->setExclusive(true);
    actOperateGroup->addAction(actOperateOnNodes);
    connect(actOperateGroup, SIGNAL(triggered(QAction *)), this, SLOT(doSceneGeometryModeSet(QAction *)));

    // select region
    actSceneViewSelectRegion = new QAction(icon("scene-select-region"), tr("&Select region"), this);
    actSceneViewSelectRegion->setStatusTip(tr("Select region"));
    actSceneViewSelectRegion->setCheckable(true);

    // object properties
    actSceneObjectProperties = new QAction(icon("scene-properties"), tr("Object properties"), this);
    actSceneObjectProperties->setShortcut(Qt::Key_Space);
    connect(actSceneObjectProperties, SIGNAL(triggered()), this, SLOT(doSceneObjectProperties()));
}

void SceneViewPreprocessor::createMenuGeometry()
{
    mnuScene = new QMenu(this);

    mnuScene->addAction(Util::scene()->actNewNode);
    mnuScene->addSeparator();
    mnuScene->addAction(actSceneViewSelectRegion);
    mnuScene->addAction(Util::scene()->actTransform);
    mnuScene->addSeparator();
    mnuScene->addAction(actSceneObjectProperties);
}

void SceneViewPreprocessor::doSceneObjectProperties()
{
    if (m_sceneMode == SceneGeometryMode_OperateOnNodes)
    {
        if (Util::scene()->selectedCount() == 1)
        {
            for (int i = 0; i < Util::scene()->nodes->length(); i++)
            {
                if (Util::scene()->nodes->at(i)->isSelected())
                    Util::scene()->nodes->at(i)->showDialog(this);
            }
        }
    }

    Util::scene()->selectNone();
}

void SceneViewPreprocessor::refresh()
{
    // actions
    actSceneViewSelectRegion->setEnabled(actSceneModePreprocessor->isChecked());
    actOperateOnNodes->setEnabled(actSceneModePreprocessor->isChecked());

    SceneViewCommon::refresh();
}

void SceneViewPreprocessor::clear()
{
    SceneViewCommon3D::clear();

    deleteTexture(m_backgroundTexture);
    m_backgroundTexture = -1;

    m_sceneMode = SceneGeometryMode_OperateOnNodes;
}

void SceneViewPreprocessor::doSceneGeometryModeSet(QAction *action)
{
    if (actOperateOnNodes->isChecked())
        m_sceneMode = SceneGeometryMode_OperateOnNodes;
    else
    {
        // set default
        actOperateOnNodes->setChecked(true);
        m_sceneMode = SceneGeometryMode_OperateOnNodes;
    }

    switch (m_sceneMode)
    {
    case SceneGeometryMode_OperateOnNodes:
        emit labelCenter(tr("Operate on nodes"));
        break;
    }

    Util::scene()->highlightNone();
    Util::scene()->selectNone();
    m_nodeLast = NULL;

    refresh();

    emit sceneGeometryModeChanged(m_sceneMode);
}

void SceneViewPreprocessor::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - m_lastPos.x();
    int dy = event->y() - m_lastPos.y();

    SceneViewCommon3D::mouseMoveEvent(event);

    m_lastPos = event->pos();

    setToolTip("");
}

void SceneViewPreprocessor::mousePressEvent(QMouseEvent *event)
{

    SceneViewCommon3D::mousePressEvent(event);
}

void SceneViewPreprocessor::mouseReleaseEvent(QMouseEvent *event)
{

    SceneViewCommon3D::mouseReleaseEvent(event);
}

void SceneViewPreprocessor::mouseDoubleClickEvent(QMouseEvent *event)
{

    SceneViewCommon3D::mouseDoubleClickEvent(event);
}

void SceneViewPreprocessor::keyPressEvent(QKeyEvent *event)
{
    SceneViewCommon3D::keyPressEvent(event);

    switch (event->key())
    {
    case Qt::Key_Delete:
    {
        Util::scene()->deleteSelected();
    }
        break;
    case Qt::Key_Space:
    {
        doSceneObjectProperties();
    }
        break;
    case Qt::Key_A:
    {
        // select all
        if (event->modifiers() & Qt::ControlModifier)
        {
            Util::scene()->selectAll(m_sceneMode);

            refresh();
        }
    }
        break;
    default:
        QGLWidget::keyPressEvent(event);
    }
}

void SceneViewPreprocessor::keyReleaseEvent(QKeyEvent *event)
{    
    updateGL();

    SceneViewCommon3D::keyReleaseEvent(event);
}

void SceneViewPreprocessor::contextMenuEvent(QContextMenuEvent *event)
{
    actSceneObjectProperties->setEnabled(false);

    // set node context menu
    if (m_sceneMode == SceneGeometryMode_OperateOnNodes)
        actSceneObjectProperties->setEnabled(Util::scene()->selectedCount() == 1);

    if (mnuScene)
        delete mnuScene;
    createMenuGeometry();

    mnuScene->exec(event->globalPos());
}

void SceneViewPreprocessor::loadBackgroundImage(const QString &fileName, double x, double y, double w, double h)
{
    // delete texture
    if (m_backgroundTexture != -1)
    {
        deleteTexture(m_backgroundTexture);
        m_backgroundTexture = -1;
    }

    if (QFile::exists(fileName))
    {
        m_backgroundImage.load(fileName);
        m_backgroundTexture = bindTexture(m_backgroundImage, GL_TEXTURE_2D, GL_RGBA);
        m_backgroundPosition = QRectF(x, y, w, h);
    }
}

void SceneViewPreprocessor::paintGL()
{
    if (!isVisible()) return;
    makeCurrent();

    glClearColor(Util::config()->colorBackground.redF(),
                 Util::config()->colorBackground.greenF(),
                 Util::config()->colorBackground.blueF(), 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_DEPTH_TEST);

    // geometry
    paintGeometry();

    // axes
    if (Util::config()->showAxes) paintAxes();
}

void SceneViewPreprocessor::paintGeometry()
{
    loadProjection3d(true);

    // nodes
    foreach (SceneNode *node, Util::scene()->nodes->items())
    {
        glColor3d(Util::config()->colorNodes.redF(),
                  Util::config()->colorNodes.greenF(),
                  Util::config()->colorNodes.blueF());
        glPointSize(Util::config()->nodeSize);

        glBegin(GL_POINTS);
        glVertex3d(node->point().x, node->point().y, node->point().z);
        glEnd();

        glColor3d(Util::config()->colorBackground.redF(),
                  Util::config()->colorBackground.greenF(),
                  Util::config()->colorBackground.blueF());
        glPointSize(Util::config()->nodeSize - 2.0);

        glBegin(GL_POINTS);
        glVertex3d(node->point().x, node->point().y, node->point().z);
        glEnd();

        if ((node->isSelected()) || (node->isHighlighted()) || (node->isError()) )
        {
            glPointSize(Util::config()->nodeSize - 2.0);

            if (node->isError())
                glColor3d(Util::config()->colorCrossed.redF(),
                          Util::config()->colorCrossed.greenF(),
                          Util::config()->colorCrossed.blueF());

            if (node->isHighlighted())
                glColor3d(Util::config()->colorHighlighted.redF(),
                          Util::config()->colorHighlighted.greenF(),
                          Util::config()->colorHighlighted.blueF());
            if (node->isSelected())
                glColor3d(Util::config()->colorSelected.redF(),
                          Util::config()->colorSelected.greenF(),
                          Util::config()->colorSelected.blueF());

            glBegin(GL_POINTS);
            glVertex3d(node->point().x, node->point().y, node->point().z);
            glEnd();
        }
    }
}


ErrorResult SceneViewPreprocessor::saveGeometryToFile(const QString &fileName, int format)
{
    // store old value
    SceneGeometryMode sceneMode = m_sceneMode;
    m_sceneMode == SceneGeometryMode_OperateOnNodes;
    actOperateOnNodes->trigger();

    makeCurrent();
    int state = GL2PS_OVERFLOW;
    int buffsize = 0;
    GLint options = GL2PS_DRAW_BACKGROUND | GL2PS_USE_CURRENT_VIEWPORT;

    FILE *fp = fopen(fileName.toStdString().c_str(), "wb");
    while (state == GL2PS_OVERFLOW)
    {
        buffsize += 1024*1024;
        gl2psBeginPage("Agros2D", "Agros2D - export", NULL, format,
                       GL2PS_BSP_SORT, options,
                       GL_RGBA, 0, NULL, 0, 0, 0, buffsize, fp, "xxx.pdf");

        glClearColor(Util::config()->colorBackground.redF(),
                     Util::config()->colorBackground.greenF(),
                     Util::config()->colorBackground.blueF(), 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // paintSolutionMesh();
        // paintInitialMesh();

        paintGeometry();

        state = gl2psEndPage();
    }
    fclose(fp);

    // restore viewport
    m_sceneMode = sceneMode;
    if (m_sceneMode == SceneGeometryMode_OperateOnNodes) actOperateOnNodes->trigger();

    return ErrorResult();
}
