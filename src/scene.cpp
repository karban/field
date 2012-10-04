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

#include "scene.h"

#include "util/xml.h"

#include "util.h"
#include "logview.h"
#include "scenebasic.h"
#include "scenenode.h"

#include "field/problem.h"
#include "problemdialog.h"
#include "scenetransformdialog.h"

#include "pythonlabagros.h"

// ************************************************************************************************************************

// initialize pointer
Util *Util::m_singleton = NULL;

Util::Util()
{
    m_problem = new Problem();
    m_scene = new Scene();
    QObject::connect(m_scene, SIGNAL(invalidated()), m_problem, SLOT(clearSolution()));

    initLists();

    // config
    m_config = new Config();
    m_config->load();

    // log
    m_log = new Log();
}

Util::~Util()
{
    delete m_scene;
    delete m_config;
    delete m_problem;
    delete m_scriptEngineRemote;
    delete m_log;
}

void Util::createSingleton()
{
    m_singleton = new Util();
}

Util *Util::singleton()
{
    return m_singleton;
}

// ************************************************************************************************************************

Scene::Scene()
{
    createActions();

    m_undoStack = new QUndoStack(this);

    connect(this, SIGNAL(invalidated()), this, SLOT(doInvalidated()));

    nodes = new SceneNodeContainer();

    clear();
}

Scene::~Scene()
{
    clear();

    delete m_undoStack;

    delete nodes;
}

void Scene::createActions()
{
    // scene - add items
    actNewNode = new QAction(icon("scene-node"), tr("New &node..."), this);
    actNewNode->setShortcut(tr("Alt+N"));
    actNewNode->setStatusTip(tr("New node"));
    connect(actNewNode, SIGNAL(triggered()), this, SLOT(doNewNode()));

    actDeleteSelected = new QAction(icon("edit-delete"), tr("Delete selected objects"), this);
    actDeleteSelected->setStatusTip(tr("Delete selected objects"));
    connect(actDeleteSelected, SIGNAL(triggered()), this, SLOT(doDeleteSelected()));

    actTransform = new QAction(icon("scene-transform"), tr("&Transform"), this);
    actTransform->setStatusTip(tr("Transform"));
}

SceneNode *Scene::addNode(SceneNode *node)
{
    // check if node doesn't exists
    if (SceneNode* existing = nodes->get(node))
    {
        delete node;
        return existing;
    }

    nodes->add(node);
    if (!currentPythonEngine()->isRunning()) emit invalidated();

    return node;
}

void Scene::removeNode(SceneNode *node)
{
    nodes->remove(node);
    // delete node;

    nodes->items().removeOne(node);
    // delete node;

    emit invalidated();
}

SceneNode *Scene::getNode(const Point3 &point)
{
    return nodes->get(point);
}

void Scene::clear()
{
    blockSignals(true);

    m_undoStack->clear();

    // geometry
    nodes->clear();

    blockSignals(false);

    emit cleared();

    emit fileNameChanged(tr("unnamed"));
    emit invalidated();
}

CubePoint Scene::boundingBox() const
{
    if (nodes->isEmpty())
    {
        return CubePoint(Point3(-0.5, -0.5, -0.5), Point3(0.5, 0.5, 0.5));
    }
    else
    {
        // nodes bounding box
        CubePoint nodesBoundingBox = nodes->boundingBox();

        return nodesBoundingBox;
    }
}

void Scene::selectNone()
{
    nodes->setSelected(false);
}

void Scene::selectAll(SceneGeometryMode sceneMode)
{
    selectNone();

    switch (sceneMode)
    {
    case SceneGeometryMode_OperateOnNodes:
        nodes->setSelected();
        break;
    }
}

void Scene::deleteSelected()
{
    m_undoStack->beginMacro(tr("Delete selected"));

    nodes->selected().deleteWithUndo(tr("Remove node"));

    m_undoStack->endMacro();

    emit invalidated();
}

int Scene::selectedCount()
{
    return nodes->selected().length();
}

void Scene::highlightNone()
{
    nodes->setHighlighted(false);
}

int Scene::highlightedCount()
{
    return nodes->highlighted().length();
}


void Scene::transformTranslate(const Point3 &point, bool copy)
{
    m_undoStack->beginMacro(tr("Translation"));

    // TODO

    m_undoStack->endMacro();
    emit invalidated();
}

void Scene::transformRotateXY(const Point &point, double angle, bool copy)
{
    m_undoStack->beginMacro(tr("Rotation"));

    // TODO

    m_undoStack->endMacro();
    emit invalidated();
}

void Scene::transformScale(const Point3 &point, double scaleFactor, bool copy)
{
    m_undoStack->beginMacro(tr("Scale"));

    // TODO

    m_undoStack->endMacro();

    emit invalidated();
}

void Scene::doInvalidated()
{
}

void Scene::doNewNode(const Point3 &point)
{
    SceneNode *node = new SceneNode(point);
    if (node->showDialog(QApplication::activeWindow(), true) == QDialog::Accepted)
    {
        SceneNode *nodeAdded = addNode(node);
        if (nodeAdded == node) m_undoStack->push(new SceneNodeCommandAdd(node->point()));
    }
    else
        delete node;
}

void Scene::doDeleteSelected()
{
    deleteSelected();
}

ErrorResult Scene::readFromFile(const QString &fileName)
{
    QSettings settings;
    QFileInfo fileInfo(fileName);
    if (fileInfo.absoluteDir() != tempProblemDir())
        settings.setValue("General/LastProblemDir", fileInfo.absolutePath());

    QDomDocument doc;
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
        return ErrorResult(ErrorResultType_Critical, tr("File '%1' cannot be opened (%2).").
                           arg(fileName).
                           arg(file.errorString()));

    // save current locale
    char *plocale = setlocale (LC_NUMERIC, "");
    setlocale (LC_NUMERIC, "C");

    clear();

    Util::problem()->config()->setFileName(fileName);
    emit fileNameChanged(fileInfo.absoluteFilePath());

    blockSignals(true);

    if (!doc.setContent(&file))
    {
        file.close();
        return ErrorResult(ErrorResultType_Critical, tr("File '%1' is not valid Agros2D file.").arg(fileName));
    }
    file.close();

    // main document
    QDomElement eleDoc = doc.documentElement();
    QString version = eleDoc.attribute("version");

    // convert document
    if (version.isEmpty() || version == "2.0")
    {
        if (QMessageBox::question(QApplication::activeWindow(), tr("Convert file?"),
                                  tr("File %1 must be converted to the new version. Do you want to convert and replace current file?").arg(fileName),
                                  tr("&Yes"), tr("&No")) == 0)
        {
            QString out = transformXML(fileName, datadir() + "/resources/xslt/problem_fld_xml.xsl");

            doc.setContent(out);
            eleDoc = doc.documentElement();

            QFile file(fileName);
            if (!file.open(QIODevice::WriteOnly))
                return ErrorResult(ErrorResultType_Critical, tr("File '%1' cannot be saved (%2).").
                                   arg(fileName).
                                   arg(file.errorString()));

            QTextStream stream(&file);
            doc.save(stream, 4);

            file.waitForBytesWritten(0);
            file.close();
        }
        else
            return ErrorResult();
    }

    // validation
    /*
    ErrorResult error = validateXML(fileName, datadir() + "/resources/xsd/problem_fld_xml.xsd");
    if (error.isError())
    {
        //qDebug() << error.message();
        return error;
    }
    */

    // geometry ***************************************************************************************************************

    QDomNode eleGeometry = eleDoc.elementsByTagName("geometry").at(0);

    // nodes
    QDomNode eleNodes = eleGeometry.toElement().elementsByTagName("nodes").at(0);
    QDomNode nodeNode = eleNodes.firstChild();
    while (!nodeNode.isNull())
    {
        QDomElement element = nodeNode.toElement();

        Point3 point = Point3(element.attribute("x").toDouble(),
                              element.attribute("y").toDouble(),
                              element.attribute("z").toDouble());

        addNode(new SceneNode(point));
        nodeNode = nodeNode.nextSibling();
    }

    // problem info
    QDomNode eleProblemInfo = eleDoc.elementsByTagName("problem").at(0);

    // name
    Util::problem()->config()->setName(eleProblemInfo.toElement().attribute("name"));

    // startup script
    QDomNode eleScriptStartup = eleProblemInfo.toElement().elementsByTagName("startup_script").at(0);
    Util::problem()->config()->setStartupScript(eleScriptStartup.toElement().text());

    // FIX ME - EOL conversion
    QPlainTextEdit textEdit;
    textEdit.setPlainText(Util::problem()->config()->startupscript());
    Util::problem()->config()->setStartupScript(textEdit.toPlainText());

    // description
    QDomNode eleDescription = eleProblemInfo.toElement().elementsByTagName("description").at(0);
    Util::problem()->config()->setDescription(eleDescription.toElement().text());

    // read config
    QDomElement config = eleDoc.elementsByTagName("config").at(0).toElement();
    Util::config()->loadPostprocessor(&config);

    blockSignals(false);

    // default values
    emit invalidated();
    emit defaultValues();

    // mesh
    if (eleDoc.elementsByTagName("mesh").count() > 0)
    {
        QDomNode eleMesh = eleDoc.elementsByTagName("mesh").at(0);
        // TODO: Util::scene()->activeSolution()->loadMeshInitial(eleMesh.toElement());
    }
    /*
    // solutions
    if (eleDoc.elementsByTagName("solutions").count() > 0)
    {
        QDomNode eleSolutions = eleDoc.elementsByTagName("solutions").at(0);
        Util::scene()->sceneSolution()->loadSolution(eleSolutions.toElement());
        emit invalidated();
    }
    */

    // run script
    currentPythonEngineAgros()->runScript(Util::problem()->config()->startupscript());

    return ErrorResult();
}

ErrorResult Scene::writeToFile(const QString &fileName)
{
    QSettings settings;

    if (QFileInfo(tempProblemFileName()).baseName() != QFileInfo(fileName).baseName())
    {
        QFileInfo fileInfo(fileName);
        if (fileInfo.absoluteDir() != tempProblemDir())
        {
            settings.setValue("General/LastProblemDir", fileInfo.absoluteFilePath());
            Util::problem()->config()->setFileName(fileName);
        }
    }

    // save current locale
    char *plocale = setlocale (LC_NUMERIC, "");
    setlocale (LC_NUMERIC, "C");

    QDomDocument doc;

    // xml version
    QDomProcessingInstruction xmlVersion = doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"utf-8\"");
    doc.appendChild(xmlVersion);

    // main document
    QDomElement eleDoc = doc.createElement("document");
    doc.appendChild(eleDoc);
    eleDoc.setAttribute("version", "2.1");

    // geometry ***************************************************************************************************************

    QDomNode eleGeometry = doc.createElement("geometry");
    eleDoc.appendChild(eleGeometry);

    // nodes
    QDomNode eleNodes = doc.createElement("nodes");
    eleGeometry.appendChild(eleNodes);
    int inode = 0;
    foreach (SceneNode *node, nodes->items())
    {
        QDomElement eleNode = doc.createElement("node");

        eleNode.setAttribute("id", inode);
        eleNode.setAttribute("x", node->point().x);
        eleNode.setAttribute("y", node->point().y);
        eleNode.setAttribute("z", node->point().z);

        eleNodes.appendChild(eleNode);

        inode++;
    }

    // problem info
    QDomElement eleProblem = doc.createElement("problem");
    eleDoc.appendChild(eleProblem);

    // name
    eleProblem.setAttribute("name", Util::problem()->config()->name());

    // startup script
    QDomElement eleScriptStartup = doc.createElement("startup_script");
    eleScriptStartup.appendChild(doc.createTextNode(Util::problem()->config()->startupscript()));
    eleProblem.appendChild(eleScriptStartup);

    // description
    QDomElement eleDescription = doc.createElement("description");
    eleDescription.appendChild(doc.createTextNode(Util::problem()->config()->description()));
    eleProblem.appendChild(eleDescription);

    // save to file
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly))
        return ErrorResult(ErrorResultType_Critical, tr("File '%1' cannot be saved (%2).").
                           arg(fileName).
                           arg(file.errorString()));

    // save config
    QDomElement eleConfig = doc.createElement("config");
    eleDoc.appendChild(eleConfig);
    Util::config()->savePostprocessor(&eleConfig);

    QTextStream out(&file);
    doc.save(out, 4);

    file.waitForBytesWritten(0);
    file.close();

    if (QFileInfo(tempProblemFileName()).baseName() != QFileInfo(fileName).baseName())
        emit fileNameChanged(QFileInfo(fileName).absoluteFilePath());

    // set system locale
    setlocale(LC_NUMERIC, plocale);

    return ErrorResult();
}
