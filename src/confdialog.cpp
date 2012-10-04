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

#include "confdialog.h"

#include "util/constants.h"
#include "gui/lineeditdouble.h"
#include "gui/systemoutput.h"

#include "scene.h"
#include "sceneview_common.h"
#include "pythonlabagros.h"

ConfigDialog::ConfigDialog(QWidget *parent) : QDialog(parent)
{
    setWindowIcon(icon("options"));
    setWindowTitle(tr("Options"));

    createControls();

    load();

    setMinimumSize(sizeHint());
    setMaximumSize(sizeHint());
}

void ConfigDialog::load()
{
    // gui style
    cmbGUIStyle->setCurrentIndex(cmbGUIStyle->findText(Util::config()->guiStyle));
    if (cmbGUIStyle->currentIndex() == -1 && cmbGUIStyle->count() > 0) cmbGUIStyle->setCurrentIndex(0);

    // language
    cmbLanguage->setCurrentIndex(cmbLanguage->findText(Util::config()->language));
    if (cmbLanguage->currentIndex() == -1 && cmbLanguage->count() > 0) cmbLanguage->setCurrentIndex(0);

    // check version
    chkCheckVersion->setChecked(Util::config()->checkVersion);

    // show result in line edit value widget
    chkLineEditValueShowResult->setChecked(Util::config()->lineEditValueShowResult);

    chkSaveWithSolution->setChecked(Util::config()->saveProblemWithSolution);

    // global script
    txtGlobalScript->setPlainText(Util::config()->globalScript);
}

void ConfigDialog::save()
{
    // gui style
    Util::config()->guiStyle = cmbGUIStyle->currentText();
    setGUIStyle(cmbGUIStyle->currentText());

    // language
    if (Util::config()->language != cmbLanguage->currentText())
        QMessageBox::warning(QApplication::activeWindow(),
                             tr("Language change"),
                             tr("Interface language has been changed. You must restart the application."));
    Util::config()->language = cmbLanguage->currentText();

    // check version
    Util::config()->checkVersion = chkCheckVersion->isChecked();

    // show result in line edit value widget
    Util::config()->lineEditValueShowResult = chkLineEditValueShowResult->isChecked();

    Util::config()->saveProblemWithSolution = chkSaveWithSolution->isChecked();

    // global script
    Util::config()->globalScript = txtGlobalScript->toPlainText();

    // save
    Util::config()->save();
}

void ConfigDialog::createControls()
{
    lstView = new QListWidget(this);
    pages = new QStackedWidget(this);

    panMain = createMainWidget();
    panSolver = createSolverWidget();
    panGlobalScriptWidget = createGlobalScriptWidget();

    // List View
    lstView->setCurrentRow(0);
    lstView->setViewMode(QListView::IconMode);
    lstView->setResizeMode(QListView::Adjust);
    lstView->setMovement(QListView::Static);
    lstView->setFlow(QListView::TopToBottom);
    lstView->setIconSize(QSize(60, 60));
    lstView->setMinimumWidth(135);
    lstView->setMaximumWidth(135);
    lstView->setMinimumHeight((45+fontMetrics().height()*4)*5);
    connect(lstView, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)),
            this, SLOT(doCurrentItemChanged(QListWidgetItem *, QListWidgetItem *)));

    QSize sizeItem(131, 85);

    // listView items
    QListWidgetItem *itemMain = new QListWidgetItem(icon("options-main"), tr("Main"), lstView);
    itemMain->setTextAlignment(Qt::AlignHCenter);
    itemMain->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    itemMain->setSizeHint(sizeItem);

    QListWidgetItem *itemSolver = new QListWidgetItem(icon("options-solver"), tr("Solver"), lstView);
    itemSolver->setTextAlignment(Qt::AlignHCenter);
    itemSolver->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    itemSolver->setSizeHint(sizeItem);

    QListWidgetItem *itemGlobalScript = new QListWidgetItem(icon("options-python"), tr("Python"), lstView);
    itemGlobalScript->setTextAlignment(Qt::AlignHCenter);
    itemGlobalScript->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    itemGlobalScript->setSizeHint(sizeItem);

    pages->addWidget(panMain);
    pages->addWidget(panSolver);
    pages->addWidget(panGlobalScriptWidget);

    QHBoxLayout *layoutHorizontal = new QHBoxLayout();
    layoutHorizontal->addWidget(lstView);
    layoutHorizontal->addWidget(pages);

    // dialog buttons
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(doAccept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(doReject()));

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addLayout(layoutHorizontal);
    // layout->addStretch();
    layout->addWidget(buttonBox);

    setLayout(layout);
}

QWidget *ConfigDialog::createMainWidget()
{
    QWidget *mainWidget = new QWidget(this);

    // general
    cmbGUIStyle = new QComboBox(mainWidget);
    cmbGUIStyle->addItems(QStyleFactory::keys());
    cmbGUIStyle->addItem("Manhattan");

    cmbLanguage = new QComboBox(mainWidget);
    cmbLanguage->addItems(availableLanguages());

    QGridLayout *layoutGeneral = new QGridLayout();
    layoutGeneral->addWidget(new QLabel(tr("UI:")), 0, 0);
    layoutGeneral->addWidget(cmbGUIStyle, 0, 1);
    layoutGeneral->addWidget(new QLabel(tr("Language:")), 1, 0);
    layoutGeneral->addWidget(cmbLanguage, 1, 1);

    QGroupBox *grpGeneral = new QGroupBox(tr("General"));
    grpGeneral->setLayout(layoutGeneral);

    // collaboration
    txtCollaborationServerURL = new LineEditDouble();

    QVBoxLayout *layoutCollaboration = new QVBoxLayout();
    layoutCollaboration->addWidget(new QLabel(tr("Collaboration server URL:")));
    layoutCollaboration->addWidget(txtCollaborationServerURL);

    QGroupBox *grpCollaboration = new QGroupBox(tr("Collaboration"));
    grpCollaboration->setLayout(layoutCollaboration);

    // other
    chkLineEditValueShowResult = new QCheckBox(tr("Show value result in line edit input"));
    chkCheckVersion = new QCheckBox(tr("Check new version during startup"));

    chkLogStdOut = new QCheckBox(tr("Print application log to stdout."));

    QVBoxLayout *layoutOther = new QVBoxLayout();
    layoutOther->addWidget(chkLineEditValueShowResult);
    layoutOther->addWidget(chkCheckVersion);
    layoutOther->addWidget(chkLogStdOut);

    QGroupBox *grpOther = new QGroupBox(tr("Other"));
    grpOther->setLayout(layoutOther);

    // layout
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(grpGeneral);
    layout->addWidget(grpCollaboration);
    layout->addWidget(grpOther);
    layout->addStretch();

    mainWidget->setLayout(layout);

    return mainWidget;
}

QWidget *ConfigDialog::createSolverWidget()
{
    // general
    chkSaveWithSolution = new QCheckBox(tr("Save problem with solution"));

    QGridLayout *layoutSolver = new QGridLayout();
    layoutSolver->addWidget(chkSaveWithSolution);

    QGroupBox *grpSolver = new QGroupBox(tr("Solver"));
    grpSolver->setLayout(layoutSolver);

    QVBoxLayout *layoutGeneral = new QVBoxLayout();
    layoutGeneral->addWidget(grpSolver);
    layoutGeneral->addStretch();

    QWidget *solverGeneralWidget = new QWidget(this);
    solverGeneralWidget->setLayout(layoutGeneral);

    // default
    QPushButton *btnCommandsDefault = new QPushButton(tr("Default"));
    connect(btnCommandsDefault, SIGNAL(clicked()), this, SLOT(doCommandsDefault()));

    QTabWidget *solverWidget = new QTabWidget(this);
    solverWidget->addTab(solverGeneralWidget, icon(""), tr("General"));

    return solverWidget;
}

QWidget *ConfigDialog::createGlobalScriptWidget()
{
    QWidget *viewWidget = new QWidget(this);

    txtGlobalScript = new ScriptEditor(currentPythonEngine(), this);

    // layout
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(txtGlobalScript);

    viewWidget->setLayout(layout);

    return viewWidget;
}

void ConfigDialog::doCurrentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    pages->setCurrentIndex(lstView->row(current));
}

void ConfigDialog::doAccept()
{
    save();
    accept();
}

void ConfigDialog::doReject()
{
    reject();
}
