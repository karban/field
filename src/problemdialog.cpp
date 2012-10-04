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

#include "problemdialog.h"

#include "scene.h"
#include "pythonlabagros.h"

#include "gui/lineeditdouble.h"
#include "gui/common.h"

const int minWidth = 130;

// ********************************************************************************************

ProblemWidget::ProblemWidget(QWidget *parent) : QWidget(parent)
{
    createActions();
    createControls();

    updateControls();

    // global signals
    connect(Util::scene(), SIGNAL(invalidated()), this, SLOT(updateControls()));
    connect(Util::problem(), SIGNAL(fieldsChanged()), this, SLOT(updateControls()));

    setMinimumSize(sizeHint());
}

void ProblemWidget::createActions()
{
    actProperties = new QAction(icon("document-properties"), tr("Properties"), this);
    actProperties->setShortcut(tr("F12"));
    actProperties->setStatusTip(tr("Problem properties"));
    actProperties->setCheckable(true);
}

void ProblemWidget::createControls()
{
    // tab
    QTabWidget *tabType = new QTabWidget();
    tabType->addTab(createControlsGeneral(), icon(""), tr("General"));
    tabType->addTab(createControlsScriptAndDescription(), icon(""), tr("Script and description"));

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(tabType);
    layout->addStretch();

    setLayout(layout);
}

QWidget *ProblemWidget::createControlsGeneral()
{
    // problem
    txtName = new QLineEdit("");

    // fill combobox
    fillComboBox();

    // general
    QGridLayout *layoutGeneral = new QGridLayout();
    layoutGeneral->setColumnMinimumWidth(0, minWidth);
    layoutGeneral->setColumnStretch(1, 1);
    layoutGeneral->addWidget(new QLabel(tr("Linear solver:")), 1, 0);

    QGroupBox *grpGeneral = new QGroupBox(tr("General"));
    grpGeneral->setLayout(layoutGeneral);

    // both
    QVBoxLayout *layoutPanel = new QVBoxLayout();
    layoutPanel->addWidget(grpGeneral);
    layoutPanel->addStretch();

    // name
    QGridLayout *layoutName = new QGridLayout();
    layoutName->addWidget(new QLabel(tr("Name:")), 0, 0);
    layoutName->addWidget(txtName, 0, 1);

    QVBoxLayout *layoutProblem = new QVBoxLayout();
    layoutProblem->addLayout(layoutName);
    layoutProblem->addLayout(layoutPanel);
    layoutProblem->addStretch();

    QWidget *widMain = new QWidget();
    widMain->setLayout(layoutProblem);

    return widMain;
}

QWidget *ProblemWidget::createControlsScriptAndDescription()
{
    // startup script
    txtStartupScript = new ScriptEditor(currentPythonEngine(), this);
    lblStartupScriptError = new QLabel();

    QPalette palette = lblStartupScriptError->palette();
    palette.setColor(QPalette::WindowText, QColor(Qt::red));
    lblStartupScriptError->setPalette(palette);

    QVBoxLayout *layoutStartup = new QVBoxLayout();
    layoutStartup->addWidget(txtStartupScript);
    layoutStartup->addWidget(lblStartupScriptError);

    QGroupBox *grpStartup = new QGroupBox(tr("Startup script"));
    grpStartup->setLayout(layoutStartup);

    // description
    txtDescription = new QTextEdit(this);
    txtDescription->setAcceptRichText(false);

    QVBoxLayout *layoutDescription = new QVBoxLayout();
    layoutDescription->addWidget(txtDescription);

    QGroupBox *grpDescription = new QGroupBox(tr("Description"));
    grpDescription->setLayout(layoutDescription);

    // layout
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(grpStartup, 2);
    layout->addWidget(grpDescription, 1);
    layout->addStretch();

    QWidget *widget = new QWidget();
    widget->setLayout(layout);

    return widget;
}

void ProblemWidget::fillComboBox()
{
    /*
    cmbCoordinateType->clear();
    cmbCoordinateType->addItem(coordinateTypeString(CoordinateType_Planar), CoordinateType_Planar);
    cmbCoordinateType->addItem(coordinateTypeString(CoordinateType_Axisymmetric), CoordinateType_Axisymmetric);
    */
}

void ProblemWidget::updateControls()
{   
    // disconnect signals
    // without clearing solution
    txtName->disconnect();
    txtDescription->disconnect();

    txtStartupScript->disconnect();

    // main
    txtName->setText(Util::problem()->config()->name());

    // startup
    txtStartupScript->setPlainText(Util::problem()->config()->startupscript());

    // description
    txtDescription->setPlainText(Util::problem()->config()->description());

    // connect signals
    // without clearing solution
    connect(txtName, SIGNAL(editingFinished()), this, SLOT(changedWithoutClear()));
    connect(txtDescription, SIGNAL(textChanged()), this, SLOT(changedWithoutClear()));

    connect(txtStartupScript, SIGNAL(textChanged()), this, SLOT(startupScriptChanged()));
    connect(txtStartupScript, SIGNAL(textChanged()), this, SLOT(changedWithClear()));
}

void ProblemWidget::changedWithoutClear()
{
    Util::problem()->config()->setName(txtName->text());
    Util::problem()->config()->setDescription(txtDescription->toPlainText());
}

void ProblemWidget::changedWithClear()
{
    // run and check startup script
    if (!txtStartupScript->toPlainText().isEmpty())
    {
        ScriptResult scriptResult = currentPythonEngineAgros()->runScript(txtStartupScript->toPlainText());
        if (scriptResult.isError)
            return;
    }

    // save properties
    Util::problem()->config()->blockSignals(true);

    // script
    Util::problem()->config()->setStartupScript(txtStartupScript->toPlainText());

    Util::problem()->config()->blockSignals(false);
    Util::problem()->config()->refresh();

    emit changed();
}

void ProblemWidget::startupScriptChanged()
{
    lblStartupScriptError->clear();

    // run and check startup script
    if (!txtStartupScript->toPlainText().isEmpty())
    {
        ScriptResult scriptResult = currentPythonEngineAgros()->runScript(txtStartupScript->toPlainText());
        if (scriptResult.isError)
            lblStartupScriptError->setText(QObject::tr("Error: %1").arg(scriptResult.text));
    }
}
