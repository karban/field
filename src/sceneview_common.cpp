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

#include "sceneview_common.h"
#include "sceneview_data.h"
#include "scene.h"

#include "scenebasic.h"
#include "scenenode.h"

#include "field/problem.h"

SceneViewWidget::SceneViewWidget(SceneViewCommon *widget, QWidget *parent) : QWidget(parent)
{
    createControls(widget);

    iconLeft(widget->iconView());
    labelLeft(widget->labelView());

    connect(widget, SIGNAL(labelCenter(QString)), this, SLOT(labelCenter(QString)));
    connect(widget, SIGNAL(labelRight(QString)), this, SLOT(labelRight(QString)));
}

SceneViewWidget::SceneViewWidget(QWidget *widget, QWidget *parent) : QWidget(parent)
{
    createControls(widget);

    iconLeft(icon("scene-info"));
    labelLeft(tr("Info"));
}

SceneViewWidget::~SceneViewWidget()
{
}

void SceneViewWidget::createControls(QWidget *widget)
{
    // label
    sceneViewLabelPixmap = new QLabel();
    sceneViewLabelLeft = new QLabel();
    sceneViewLabelLeft->setMinimumWidth(150);
    sceneViewLabelCenter = new QLabel();
    sceneViewLabelCenter->setMinimumWidth(150);
    sceneViewLabelRight = new QLabel();
    sceneViewLabelRight->setMinimumWidth(200);

    QHBoxLayout *sceneViewLabelLayout = new QHBoxLayout();
    sceneViewLabelLayout->addWidget(sceneViewLabelPixmap);
    sceneViewLabelLayout->addWidget(sceneViewLabelLeft);
    sceneViewLabelLayout->addStretch(0.5);
    sceneViewLabelLayout->addWidget(sceneViewLabelCenter);
    sceneViewLabelLayout->addStretch(0.5);
    sceneViewLabelLayout->addWidget(sceneViewLabelRight);

    // view
    QVBoxLayout *sceneViewLayout = new QVBoxLayout();
    sceneViewLayout->addLayout(sceneViewLabelLayout);
    sceneViewLayout->addWidget(widget);
    sceneViewLayout->setStretch(1, 1);

    setLayout(sceneViewLayout);
}

void SceneViewWidget::labelLeft(const QString &left)
{
    sceneViewLabelLeft->setText(left);
}

void SceneViewWidget::labelCenter(const QString &center)
{
    sceneViewLabelCenter->setText(center);
}

void SceneViewWidget::labelRight(const QString &right)
{
    sceneViewLabelRight->setText(right);
}

void SceneViewWidget::iconLeft(const QIcon &left)
{
    QPixmap pixmap = left.pixmap(QSize(16, 16));
    sceneViewLabelPixmap->setPixmap(pixmap);
}


// **********************************************************************************************

SceneViewCommon::SceneViewCommon(QWidget *parent) : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
    m_mainWindow = (QMainWindow *) parent;

    createActions();

    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
    setContextMenuPolicy(Qt::DefaultContextMenu);

    setMinimumSize(400, 400);
}

SceneViewCommon::~SceneViewCommon()
{
}

void SceneViewCommon::createActions()
{
}

void SceneViewCommon::initializeGL()
{
    glShadeModel(GL_SMOOTH);
    glEnable(GL_NORMALIZE);
}

void SceneViewCommon::resizeGL(int w, int h)
{
    setupViewport(w, h);
}

void SceneViewCommon::setupViewport(int w, int h)
{
    glViewport(0, 0, w, h);
}

ErrorResult SceneViewCommon::saveImageToFile(const QString &fileName, int w, int h)
{
    QPixmap pixmap = renderScenePixmap(w, h);
    if (pixmap.save(fileName, "PNG"))
        resizeGL(width(), height());
    else
        return ErrorResult(ErrorResultType_Critical, tr("Image cannot be saved to the file '%1'.").arg(fileName));

    return ErrorResult();
}

QPixmap SceneViewCommon::renderScenePixmap(int w, int h, bool useContext)
{
    QPixmap pixmap = renderPixmap((w == 0) ? width() : w,
                                  (h == 0) ? height() : h,
                                  useContext);
    resizeGL(width(), height());

    return pixmap;
}

void SceneViewCommon::loadProjectionViewPort()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrtho(-0.5, 0.5, -0.5, 0.5, -10.0, -10.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// events *****************************************************************************************************************************

void SceneViewCommon::closeEvent(QCloseEvent *event)
{
    event->ignore();
}

// slots *****************************************************************************************************************************

void SceneViewCommon::doZoomBestFit()
{
    CubePoint rect = Util::scene()->boundingBox();
}

void SceneViewCommon::doZoomIn()
{
    setZoom(1.2);
}

void SceneViewCommon::doZoomOut()
{
    setZoom(-1/1.2);
}

void SceneViewCommon::clear()
{
    refresh();
    doZoomBestFit();
}

void SceneViewCommon::refresh()
{
    // resize(((QWidget *) parent())->size());

    emit mousePressed();

    paintGL();
    updateGL();
}


void SceneViewCommon::drawBlend(Point start, Point end, double red, double green, double blue, double alpha) const
{
    // store color
    double color[4];
    glGetDoublev(GL_CURRENT_COLOR, color);

    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // blended rectangle
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(red, green, blue, alpha);

    glBegin(GL_QUADS);
    glVertex2d(start.x, start.y);
    glVertex2d(end.x, start.y);
    glVertex2d(end.x, end.y);
    glVertex2d(start.x, end.y);
    glEnd();

    glDisable(GL_BLEND);
    glDisable(GL_POLYGON_OFFSET_FILL);

    // retrieve color
    glColor4d(color[0], color[1], color[2], color[3]);
}
