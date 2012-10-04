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

#include "common.h"

#include "scene.h"
#include "util.h"

#include "field/problem.h"

void readPixmap(QLabel *lblEquation, const QString &name)
{
    QPixmap pixmap;
    pixmap.load(name);
    lblEquation->setPixmap(pixmap);
    lblEquation->setMaximumSize(pixmap.size());
    lblEquation->setMinimumSize(pixmap.size());
}

QLabel *createLabel(const QString &label, const QString &toolTip)
{
    QLabel *lblEquation = new QLabel(label + ":");
    lblEquation->setToolTip(toolTip);
    lblEquation->setMinimumWidth(100);
    return lblEquation;
}

void addTreeWidgetItemValue(QTreeWidgetItem *parent, const QString &name, const QString &text, const QString &unit)
{
    QTreeWidgetItem *item = new QTreeWidgetItem(parent);
    item->setText(0, name);
    item->setText(1, text);
    item->setTextAlignment(1, Qt::AlignRight);
    item->setText(2, QString("%1 ").arg(unit));
    item->setTextAlignment(2, Qt::AlignLeft);
}

void fillComboBoxScalarVariable(QComboBox *cmbFieldVariable)
{
    if (!Util::problem()->isSolved())
        return;

    // store variable
    QString physicFieldVariable = cmbFieldVariable->itemData(cmbFieldVariable->currentIndex()).toString();

    // clear combo
    cmbFieldVariable->blockSignals(true);
    cmbFieldVariable->clear();

    cmbFieldVariable->setCurrentIndex(cmbFieldVariable->findData(physicFieldVariable));
    if (cmbFieldVariable->currentIndex() == -1)
        cmbFieldVariable->setCurrentIndex(0);
    cmbFieldVariable->blockSignals(false);
}
