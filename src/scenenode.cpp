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

#include "util.h"
#include "scene.h"
#include "scenenode.h"
#include "field/problem.h"

SceneNode::SceneNode(const Point3 &point) : SceneBasic(), m_point(point)
{
}

double SceneNode::distance(const Point3 &point) const
{
    return (this->point() - point).magnitude();
}

int SceneNode::showDialog(QWidget *parent, bool isNew)
{
    DSceneNode *dialog = new DSceneNode(this, parent, isNew);
    return dialog->exec();
}

SceneNodeCommandRemove* SceneNode::getRemoveCommand()
{
    return new SceneNodeCommandRemove(this->point());
}

// *************************************************************************************************************************************

SceneNode* SceneNodeContainer::get(SceneNode *node) const
{
    foreach (SceneNode *nodeCheck, data)
    {
        if (nodeCheck->point() == node->point())
        {
            return nodeCheck;
        }
    }

    return NULL;
}

SceneNode* SceneNodeContainer::get(const Point3 &point) const
{
    foreach (SceneNode *nodeCheck, data)
    {
        if (nodeCheck->point() == point)
            return nodeCheck;
    }

    return NULL;
}

bool SceneNodeContainer::remove(SceneNode *item)
{
    return SceneBasicContainer<SceneNode>::remove(item);
}

CubePoint SceneNodeContainer::boundingBox() const
{
    Point3 min( numeric_limits<double>::max(),  numeric_limits<double>::max(),  numeric_limits<double>::max());
    Point3 max(-numeric_limits<double>::max(), -numeric_limits<double>::max(), -numeric_limits<double>::max());

    foreach (SceneNode *node, data)
    {
        min.x = qMin(min.x, node->point().x);
        max.x = qMax(max.x, node->point().x);
        min.y = qMin(min.y, node->point().y);
        max.y = qMax(max.y, node->point().y);
        min.y = qMin(min.z, node->point().z);
        max.y = qMax(max.z, node->point().z);
    }

    return CubePoint(min, max);
}

SceneNodeContainer SceneNodeContainer::selected()
{
    SceneNodeContainer list;
    foreach (SceneNode* item, this->data)
    {
        if (item->isSelected())
            list.data.push_back(item);
    }

    return list;
}

SceneNodeContainer SceneNodeContainer::highlighted()
{
    SceneNodeContainer list;
    foreach (SceneNode* item, this->data)
    {
        if (item->isHighlighted())
            list.data.push_back(item);
    }

    return list;
}

// *************************************************************************************************************************************

DSceneNode::DSceneNode(SceneNode *node, QWidget *parent, bool isNew) : SceneBasicDialog(parent, isNew)
{
    m_object = node;

    setWindowIcon(icon("scene-node"));
    setWindowTitle(tr("Node"));

    createControls();

    load();

    setMinimumSize(sizeHint());
    // setMaximumSize(sizeHint());
}

QLayout* DSceneNode::createContent()
{
    txtPointX = new ValueLineEdit();
    txtPointY = new ValueLineEdit();
    txtPointZ = new ValueLineEdit();
    connect(txtPointX, SIGNAL(editingFinished()), this, SLOT(doEditingFinished()));
    connect(txtPointY, SIGNAL(editingFinished()), this, SLOT(doEditingFinished()));
    connect(txtPointZ, SIGNAL(editingFinished()), this, SLOT(doEditingFinished()));
    connect(txtPointX, SIGNAL(evaluated(bool)), this, SLOT(evaluated(bool)));
    connect(txtPointY, SIGNAL(evaluated(bool)), this, SLOT(evaluated(bool)));
    connect(txtPointZ, SIGNAL(evaluated(bool)), this, SLOT(evaluated(bool)));
    lblDistance = new QLabel();
    lblAnglePhi = new QLabel();
    lblAnglePsi = new QLabel();

    QFormLayout *layout = new QFormLayout();
    layout->addRow(Util::problem()->config()->labelX() + " (m):", txtPointX);
    layout->addRow(Util::problem()->config()->labelY() + " (m):", txtPointY);
    layout->addRow(Util::problem()->config()->labelZ() + " (m):", txtPointZ);
    layout->addRow(tr("Distance:"), lblDistance);
    layout->addRow(tr("Angle 1:"), lblAnglePhi);
    layout->addRow(tr("Angle 2:"), lblAnglePsi);

    return layout;
}

bool DSceneNode::load()
{
    SceneNode *sceneNode = dynamic_cast<SceneNode *>(m_object);

    txtPointX->setNumber(sceneNode->point().x);
    txtPointY->setNumber(sceneNode->point().y);
    txtPointZ->setNumber(sceneNode->point().z);

    doEditingFinished();

    return true;
}

bool DSceneNode::save()
{
    if (!txtPointX->evaluate(false)) return false;
    if (!txtPointY->evaluate(false)) return false;
    if (!txtPointZ->evaluate(false)) return false;

    SceneNode *sceneNode = dynamic_cast<SceneNode *>(m_object);

    Point3 point(txtPointX->number(), txtPointY->number(), txtPointZ->number());

    // check if node doesn't exists
    if (Util::scene()->getNode(point) && ((sceneNode->point() != point) || m_isNew))
    {
        QMessageBox::warning(this, tr("Node"), tr("Node already exists."));
        return false;
    }

    if (!m_isNew)
    {
        if (sceneNode->point() != point)
        {
            Util::scene()->undoStack()->push(new SceneNodeCommandEdit(sceneNode->point(), point));
        }
    }

    sceneNode->setPoint(point);

    return true;
}

void DSceneNode::doEditingFinished()
{
    double mag = sqrt((txtPointX->number() * txtPointX->number()) + (txtPointY->number() * txtPointY->number()) + (txtPointZ->number() * txtPointZ->number()));
    lblDistance->setText(QString("%1 m").arg(mag));
    lblAnglePhi->setText(QString("%1 deg.").arg(
                             (mag > EPS_ZERO)
                             ? atan2(txtPointY->number(), txtPointX->number()) / M_PI * 180.0 : 0.0));
    lblAnglePsi->setText(QString("%1 deg.").arg(
                             (mag > EPS_ZERO)
                             ? atan2(txtPointZ->number(), txtPointX->number()) / M_PI * 180.0 : 0.0));
}


// undo framework *******************************************************************************************************************

SceneNodeCommandAdd::SceneNodeCommandAdd(const Point3 &point, QUndoCommand *parent) : QUndoCommand(parent)
{
    m_point = point;
}

void SceneNodeCommandAdd::undo()
{
    SceneNode *node = Util::scene()->getNode(m_point);
    if (node)
    {
        Util::scene()->nodes->remove(node);
        Util::scene()->invalidate();
    }
}

void SceneNodeCommandAdd::redo()
{
    Util::scene()->addNode(new SceneNode(m_point));
    Util::scene()->invalidate();
}

SceneNodeCommandRemove::SceneNodeCommandRemove(const Point3 &point, QUndoCommand *parent) : QUndoCommand(parent)
{
    m_point = point;
}

void SceneNodeCommandRemove::undo()
{
    Util::scene()->addNode(new SceneNode(m_point));
    Util::scene()->invalidate();
}

void SceneNodeCommandRemove::redo()
{
    SceneNode *node = Util::scene()->getNode(m_point);
    if (node)
    {
        Util::scene()->nodes->remove(node);
        Util::scene()->invalidate();
    }
}

SceneNodeCommandEdit::SceneNodeCommandEdit(const Point3 &point, const Point3 &pointNew, QUndoCommand *parent) : QUndoCommand(parent)
{
    m_point = point;
    m_pointNew = pointNew;
}

void SceneNodeCommandEdit::undo()
{
    SceneNode *node = Util::scene()->getNode(m_pointNew);
    if (node)
    {
        node->setPoint(m_point);
        Util::scene()->invalidate();
    }
}

void SceneNodeCommandEdit::redo()
{
    SceneNode *node = Util::scene()->getNode(m_point);
    if (node)
    {
        node->setPoint(m_pointNew);
        Util::scene()->invalidate();
    }
}

bool SceneNode::isError()
{
    return false; // TODO
}
