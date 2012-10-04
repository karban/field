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

#ifndef PROBLEMDIALOG_H
#define PROBLEMDIALOG_H

#include "util.h"

class ProblemConfig;
class ScriptEditor;

class LineEditDouble;
class ValueLineEdit;

class ProblemWidget;

class ProblemWidget: public QWidget
{
    Q_OBJECT
public:
    ProblemWidget(QWidget *parent = 0);

    QAction *actProperties;

signals:
    void changed();

public slots:
    void updateControls();

private:
    QDialogButtonBox *buttonBox;

    QLineEdit *txtName;

    // startup script
    ScriptEditor *txtStartupScript;
    QLabel *lblStartupScriptError;

    // description
    QTextEdit *txtDescription;

    void createActions();
    void createControls();
    QWidget *createControlsGeneral();
    QWidget *createControlsScriptAndDescription();

    void fillComboBox();

private slots:
    void startupScriptChanged();

    void changedWithClear();
    void changedWithoutClear();
};

#endif // PROBLEMDIALOG_H
