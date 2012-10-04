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

#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include "util.h"

struct SceneViewSettings;
class ScriptEditor;

class LineEditDouble;
class ValueLineEdit;

class ConfigDialog : public QDialog
{
    Q_OBJECT
public:
    ConfigDialog(QWidget *parent);

private slots:
    void doCurrentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

    void doAccept();
    void doReject();

private:
    QListWidget *lstView;
    QStackedWidget *pages;
    QWidget *panMain;
    QWidget *panSolver;
    QWidget *panGlobalScriptWidget;

    // main
    QComboBox *cmbGUIStyle;
    QComboBox *cmbLanguage;
    LineEditDouble *txtCollaborationServerURL;

    // show result in line edit value widget
    QCheckBox *chkLineEditValueShowResult;

    // save with solution
    QCheckBox *chkSaveWithSolution;

    // check version
    QCheckBox *chkCheckVersion;

    // log std out
    QCheckBox *chkLogStdOut;

    // scene font
    QLabel *lblSceneFontExample;
    QPushButton *btnSceneFont;

    // global script
    ScriptEditor *txtGlobalScript;

    void load();
    void save();

    void createControls();
    QWidget *createMainWidget();
    QWidget *createSolverWidget();
    QWidget *createGlobalScriptWidget();
};


#endif // OPTIONSDIALOG_H
