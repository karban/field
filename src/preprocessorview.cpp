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

#include "preprocessorview.h"

#include "util/constants.h"

#include "scene.h"
#include "logview.h"
#include "scenebasic.h"
#include "scenenode.h"
#include "sceneview_geometry.h"
#include "problemdialog.h"
#include "pythonlabagros.h"
#include "field/problem.h"
#include "ctemplate/template.h"

PreprocessorWidget::PreprocessorWidget(SceneViewPreprocessor *sceneView, QWidget *parent): QWidget(parent)
{
    this->m_sceneViewGeometry = sceneView;

    setMinimumWidth(160);
    setObjectName("PreprocessorView");

    createActions();

    // context menu
    mnuPreprocessor = new QMenu(this);

    // boundary conditions, materials and geometry information
    createControls();

    connect(Util::scene(), SIGNAL(cleared()), this, SLOT(refresh()));

    connect(Util::scene(), SIGNAL(invalidated()), this, SLOT(refresh()));
    connect(Util::problem(), SIGNAL(solved()), this, SLOT(refresh()));
    connect(Util::problem(), SIGNAL(timeStepChanged()), this, SLOT(refresh()));
    connect(currentPythonEngineAgros(), SIGNAL(executedScript()), this, SLOT(refresh()));

    connect(trvWidget, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(doContextMenu(const QPoint &)));
    connect(trvWidget, SIGNAL(itemActivated(QTreeWidgetItem *, int)), this, SLOT(doItemSelected(QTreeWidgetItem *, int)));
    connect(trvWidget, SIGNAL(itemPressed(QTreeWidgetItem *, int)), this, SLOT(doItemSelected(QTreeWidgetItem *, int)));
    connect(trvWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)), this, SLOT(doItemDoubleClicked(QTreeWidgetItem *, int)));

    doItemSelected(NULL, Qt::UserRole);

    QSettings settings;
    splitter->restoreState(settings.value("PreprocessorView/SplitterState").toByteArray());
}

PreprocessorWidget::~PreprocessorWidget()
{
    QSettings settings;
    settings.setValue("PreprocessorView/SplitterState", splitter->saveState());
}

void PreprocessorWidget::createActions()
{
    actProperties = new QAction(icon("scene-properties"), tr("&Properties"), this);
    actProperties->setStatusTip(tr("Properties"));
    connect(actProperties, SIGNAL(triggered()), this, SLOT(doProperties()));

    actDelete = new QAction(icon("scene-delete"), tr("&Delete"), this);
    actDelete->setStatusTip(tr("Delete item"));
    connect(actDelete, SIGNAL(triggered()), this, SLOT(doDelete()));
}

void PreprocessorWidget::createMenu()
{
    mnuPreprocessor->clear();

    mnuPreprocessor->addAction(Util::scene()->actNewNode);
    mnuPreprocessor->addSeparator();
    mnuPreprocessor->addAction(actDelete);
    mnuPreprocessor->addSeparator();
    mnuPreprocessor->addAction(actProperties);
}

void PreprocessorWidget::createControls()
{
    webView = new QWebView(this);
    webView->setMinimumHeight(250);

    QHBoxLayout *layoutInfo = new QHBoxLayout();
    layoutInfo->setMargin(0);
    layoutInfo->addWidget(webView);

    QWidget *widgetInfo = new QWidget(this);
    widgetInfo->setLayout(layoutInfo);

    trvWidget = new QTreeWidget(this);
    trvWidget->setHeaderHidden(true);
    trvWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    trvWidget->setMouseTracking(true);
    trvWidget->setColumnCount(1);
    trvWidget->setColumnWidth(0, 150);
    trvWidget->setIndentation(12);

    splitter = new QSplitter(Qt::Vertical, this);
    splitter->addWidget(widgetInfo);
    splitter->addWidget(trvWidget);

    /*
    QVBoxLayout *layoutMain = new QVBoxLayout();
    layoutMain->addLayout(layoutInfo, 2);
    layoutMain->addWidget(trvWidget, 3);

    QWidget *main = new QWidget(this);
    main->setLayout(layoutMain);
    */

    QVBoxLayout *layoutMain = new QVBoxLayout();
    layoutMain->setContentsMargins(0, 5, 3, 5);
    layoutMain->addWidget(splitter);

    setLayout(layoutMain);
}

void PreprocessorWidget::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Delete:
        doDelete();
        break;
    default:
        QWidget::keyPressEvent(event);
    }
}

void PreprocessorWidget::refresh()
{
    // script speed improvement
    if (currentPythonEngine()->isRunning()) return;

    blockSignals(true);
    setUpdatesEnabled(false);

    trvWidget->clear();

    QFont fnt = trvWidget->font();
    fnt.setBold(true);

    // geometry
    QTreeWidgetItem *geometryNode = new QTreeWidgetItem(trvWidget);
    // geometryNode->setIcon(0, icon("geometry"));
    geometryNode->setText(0, tr("Geometry"));
    geometryNode->setFont(0, fnt);
    geometryNode->setExpanded(false);

    // nodes
    QTreeWidgetItem *nodesNode = new QTreeWidgetItem(geometryNode);
    nodesNode->setText(0, tr("Nodes"));
    nodesNode->setIcon(0, icon("scenenode"));
    // nodesNode->setForeground(0, QBrush(Qt::darkBlue));
    nodesNode->setFont(0, fnt);

    QList<QTreeWidgetItem *> listNodes;
    int inode = 0;
    foreach (SceneNode *node, Util::scene()->nodes->items())
    {
        QTreeWidgetItem *item = new QTreeWidgetItem();

        item->setText(0, QString("%1 - [%2; %3]").
                      arg(inode).
                      arg(node->point().x, 0, 'e', 2).
                      arg(node->point().y, 0, 'e', 2));
        item->setIcon(0, icon("scene-node"));
        item->setData(0, Qt::UserRole, node->variant());

        listNodes.append(item);

        inode++;
    }
    nodesNode->addChildren(listNodes);

    setUpdatesEnabled(true);
    blockSignals(false);

    QTimer::singleShot(0, this, SLOT(showInfo()));
}

void PreprocessorWidget::doContextMenu(const QPoint &pos)
{
    QTreeWidgetItem *item = trvWidget->itemAt(pos);
    doItemSelected(item, 0);

    trvWidget->setCurrentItem(item);
    mnuPreprocessor->exec(QCursor::pos());
}

void PreprocessorWidget::doItemSelected(QTreeWidgetItem *item, int role)
{
    createMenu();

    actProperties->setEnabled(false);
    actDelete->setEnabled(false);

    if (item != NULL)
    {
        Util::scene()->selectNone();
        Util::scene()->highlightNone();

        // geometry
        if (SceneBasic *objectBasic = trvWidget->currentItem()->data(0, Qt::UserRole).value<SceneBasic *>())
        {
            m_sceneViewGeometry->actSceneModePreprocessor->trigger();

            if (dynamic_cast<SceneNode *>(objectBasic))
                m_sceneViewGeometry->actOperateOnNodes->trigger();

            objectBasic->setSelected(true);
            m_sceneViewGeometry->refresh();
            m_sceneViewGeometry->setFocus();

            actProperties->setEnabled(true);
            actDelete->setEnabled(true);
        }
    }
}

void PreprocessorWidget::doItemDoubleClicked(QTreeWidgetItem *item, int role)
{
    doProperties();
}

void PreprocessorWidget::doProperties()
{
    if (trvWidget->currentItem())
    {
        // geometry
        if (SceneBasic *objectBasic = trvWidget->currentItem()->data(0, Qt::UserRole).value<SceneBasic *>())
        {
            if (objectBasic->showDialog(this) == QDialog::Accepted)
            {
                m_sceneViewGeometry->refresh();
                refresh();
            }
            return;
        }
    }
}

void PreprocessorWidget::doDelete()
{
    if (trvWidget->currentItem())
    {
        // scene objects
        if (SceneBasic *objectBasic = trvWidget->currentItem()->data(0, Qt::UserRole).value<SceneBasic*>())
        {
            if (SceneNode *node = dynamic_cast<SceneNode *>(objectBasic))
            {
                Util::scene()->nodes->remove(node);
            }
        }

        m_sceneViewGeometry->refresh();
    }
}

void PreprocessorWidget::showInfo()
{
    // stylesheet
    std::string style;
    ctemplate::TemplateDictionary stylesheet("style");
    stylesheet.SetValue("FONTFAMILY", QApplication::font().family().toStdString());
    stylesheet.SetValue("FONTSIZE", (QString("%1").arg(QApplication::font().pointSize()).toStdString()));

    ctemplate::ExpandTemplate(datadir().toStdString() + TEMPLATEROOT.toStdString() + "/panels/style.tpl", ctemplate::DO_NOT_STRIP, &stylesheet, &style);

    // template
    std::string info;
    ctemplate::TemplateDictionary problemInfo("info");

    problemInfo.SetValue("STYLESHEET", style);
    problemInfo.SetValue("BASIC_INFORMATION_LABEL", tr("Basic informations").toStdString());

    ctemplate::ExpandTemplate(datadir().toStdString() + TEMPLATEROOT.toStdString() + "/panels/preprocessor.tpl", ctemplate::DO_NOT_STRIP, &problemInfo, &info);
    webView->setHtml(QString::fromStdString(info));

    setFocus();
}
