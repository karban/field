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

#include "scenebasic.h"

#include "scenenode.h"

#include "scene.h"

#include "field/problem.h"

SceneBasic::SceneBasic()
{
    setSelected(false);
    m_isHighlighted = false;
}

QVariant SceneBasic::variant()
{
    QVariant v;
    v.setValue(this);
    return v;
}

// *************************************************************************************************************************************

template <typename BasicType>
bool SceneBasicContainer<BasicType>::add(BasicType *item)
{
    //TODO add check
    data.append(item);

    return true;
}

template <typename BasicType>
bool SceneBasicContainer<BasicType>::remove(BasicType *item)
{
    return data.removeOne(item);
}

template <typename BasicType>
BasicType *SceneBasicContainer<BasicType>::at(int i)
{
    return data.at(i);
}

template <typename BasicType>
void SceneBasicContainer<BasicType>::clear()
{
    foreach (BasicType* item, data)
        delete item;

    data.clear();
}

template <typename BasicType>
void SceneBasicContainer<BasicType>::setSelected(bool value)
{
    foreach (BasicType* item, data)
        item->setSelected(value);
}

template <typename BasicType>
void SceneBasicContainer<BasicType>::setHighlighted(bool value)
{
    foreach (BasicType* item, data)
        item->setHighlighted(value);
}

template <typename BasicType>
void SceneBasicContainer<BasicType>::deleteWithUndo(QString message)
{
    foreach (BasicType *node, data)
    {
        Util::scene()->undoStack()->beginMacro(message);
        Util::scene()->undoStack()->push(node->getRemoveCommand());
        this->remove(node);
        Util::scene()->undoStack()->endMacro();
    }
}

template class SceneBasicContainer<SceneNode>;

// *************************************************************************************************************************************

SceneBasicDialog::SceneBasicDialog(QWidget *parent, bool isNew) : QDialog(parent)
{
    this->m_isNew = isNew;
    layout = new QVBoxLayout();
}

void SceneBasicDialog::createControls()
{
    // dialog buttons
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(doAccept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(doReject()));

    layout->addLayout(createContent());
    layout->addStretch();
    layout->addWidget(buttonBox);

    setLayout(layout);
}

void SceneBasicDialog::doAccept()
{
    if (save())
        accept();
}

void SceneBasicDialog::doReject()
{
    reject();
}

void SceneBasicDialog::evaluated(bool isError)
{
    buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!isError);
}


//**************************************************************************************************

