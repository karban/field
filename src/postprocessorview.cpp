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

#include "postprocessorview.h"

#include "gui/lineeditdouble.h"
#include "gui/groupbox.h"
#include "gui/common.h"

#include "scene.h"
#include "sceneview_geometry.h"
#include "sceneview_post3d.h"
#include "pythonlabagros.h"

#include "field/problem.h"

#include "util/constants.h"

const double minWidth = 110;

PostprocessorWidget::PostprocessorWidget(SceneViewPreprocessor *sceneGeometry,
                                         SceneViewPost3D *scenePost3D,
                                         QWidget *parent) : QWidget(parent)
{
    m_sceneGeometry = sceneGeometry;
    m_scenePost3D = scenePost3D;

    setWindowIcon(icon("scene-properties"));
    setObjectName("PostprocessorView");

    createControls();

    loadBasic();


    connect(currentPythonEngineAgros(), SIGNAL(executedScript()), this, SLOT(updateControls()));
    connect(currentPythonEngineAgros(), SIGNAL(executedExpression()), this, SLOT(updateControls()));
}

void PostprocessorWidget::loadBasic()
{

    radPost3DNone->setChecked(Util::config()->showPost3D == SceneViewPost3DMode_None);
    radPost3DScalarField3D->setChecked(Util::config()->showPost3D == SceneViewPost3DMode_ScalarView3D);
    radPost3DModel->setChecked(Util::config()->showPost3D == SceneViewPost3DMode_Model);

    // scalar field
    chkShowScalarColorBar->setChecked(Util::config()->showScalarColorBar);
    cmbPalette->setCurrentIndex(cmbPalette->findData(Util::config()->paletteType));
    chkPaletteFilter->setChecked(Util::config()->paletteFilter);
    doPaletteFilter(chkPaletteFilter->checkState());
    txtPaletteSteps->setValue(Util::config()->paletteSteps);

    refresh();
}

void PostprocessorWidget::saveBasic()
{
    // active field

    // show
    if (radPost3DNone->isChecked()) Util::config()->showPost3D = SceneViewPost3DMode_None;
    if (radPost3DScalarField3D->isChecked()) Util::config()->showPost3D = SceneViewPost3DMode_ScalarView3D;
    if (radPost3DModel->isChecked()) Util::config()->showPost3D = SceneViewPost3DMode_Model;

    // scalar field
    Util::config()->showScalarColorBar = chkShowScalarColorBar->isChecked();
    Util::config()->paletteType = (PaletteType) cmbPalette->itemData(cmbPalette->currentIndex()).toInt();
    Util::config()->paletteFilter = chkPaletteFilter->isChecked();
    Util::config()->paletteSteps = txtPaletteSteps->value();

    // save
    Util::config()->save();
}

void PostprocessorWidget::createControls()
{    
    // dialog buttons
    btnOK = new QPushButton(tr("Apply"));
    connect(btnOK, SIGNAL(clicked()), SLOT(doApply()));

    QHBoxLayout *layoutButtons = new QHBoxLayout();
    layoutButtons->addStretch(1);
    layoutButtons->addWidget(btnOK);

    basic = controlsBasic();
    groupPostScalar = postScalarWidget();

    QVBoxLayout *layoutMain = new QVBoxLayout();
    layoutMain->setContentsMargins(0, 5, 3, 5);
    layoutMain->addWidget(basic);
    layoutMain->addWidget(groupPostScalar);
    layoutMain->addWidget(btnOK, 0, Qt::AlignRight);

    refresh();

    setLayout(layoutMain);
}

QWidget *PostprocessorWidget::post3DWidget()
{
    // layout post3d
    radPost3DNone = new QRadioButton(tr("None"), this);
    radPost3DScalarField3D = new QRadioButton(tr("Scalar view"), this);
    radPost3DModel = new QRadioButton("Model", this);

    butPost3DGroup = new QButtonGroup(this);
    butPost3DGroup->addButton(radPost3DNone);
    butPost3DGroup->addButton(radPost3DScalarField3D);
    butPost3DGroup->addButton(radPost3DModel);
    connect(butPost3DGroup, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(doPostprocessorGroupClicked(QAbstractButton*)));

    QGridLayout *layoutPost3D = new QGridLayout();
    layoutPost3D->addWidget(radPost3DNone, 0, 0);
    layoutPost3D->addWidget(radPost3DScalarField3D, 1, 0);
    layoutPost3D->addWidget(radPost3DModel, 1, 1);

    QHBoxLayout *layoutShowPost3D = new QHBoxLayout();
    layoutShowPost3D->addLayout(layoutPost3D);
    layoutShowPost3D->addStretch(1);

    QGroupBox *grpShowPost3D = new QGroupBox(tr("Postprocessor 3D"));
    grpShowPost3D->setLayout(layoutShowPost3D);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->setMargin(0);
    layout->addWidget(grpShowPost3D);
    layout->addStretch(1);

    QWidget *widget = new QWidget(this);
    widget->setLayout(layout);

    return widget;
}

QWidget *PostprocessorWidget::controlsBasic()
{
    groupPost3d = post3DWidget();

    widgetsLayout = new QStackedLayout();
    widgetsLayout->addWidget(groupPost3d);

    QVBoxLayout *layoutBasic = new QVBoxLayout();
    layoutBasic->addLayout(widgetsLayout);
    layoutBasic->addStretch(1);

    QWidget *widget = new QWidget(this);
    widget->setLayout(layoutBasic);

    return widget;
}

CollapsableGroupBoxButton *PostprocessorWidget::postScalarWidget()
{
    // layout scalar field
    cmbPostScalarFieldVariable = new QComboBox();
    // connect(cmbPostScalarFieldVariable, SIGNAL(currentIndexChanged(int)), this, SLOT(doScalarFieldVariable(int)));

    chkScalarFieldRangeAuto = new QCheckBox(tr("Auto range"));
    connect(chkScalarFieldRangeAuto, SIGNAL(stateChanged(int)), this, SLOT(doScalarFieldRangeAuto(int)));

    groupPostScalarAdvanced = postScalarAdvancedWidget();

    QGridLayout *layoutScalarField = new QGridLayout();
    layoutScalarField->setColumnMinimumWidth(0, minWidth);
    layoutScalarField->setColumnStretch(1, 1);
    layoutScalarField->addWidget(new QLabel(tr("Variable:")), 0, 0);
    layoutScalarField->addWidget(cmbPostScalarFieldVariable, 0, 1);
    layoutScalarField->addWidget(groupPostScalarAdvanced, 2, 0, 1, 2);

    CollapsableGroupBoxButton *grpScalarField = new CollapsableGroupBoxButton(tr("Scalar field"));
    connect(grpScalarField, SIGNAL(collapseEvent(bool)), this, SLOT(doScalarFieldExpandCollapse(bool)));
    grpScalarField->setCollapsed(true);
    grpScalarField->setLayout(layoutScalarField);

    return grpScalarField;
}

QWidget *PostprocessorWidget::postScalarAdvancedWidget()
{
    // scalar field
    // palette
    cmbPalette = new QComboBox();
    foreach (QString key, paletteTypeStringKeys())
        cmbPalette->addItem(paletteTypeString(paletteTypeFromStringKey(key)), paletteTypeFromStringKey(key));

    chkPaletteFilter = new QCheckBox(tr("Filter"));
    connect(chkPaletteFilter, SIGNAL(stateChanged(int)), this, SLOT(doPaletteFilter(int)));

    // steps
    txtPaletteSteps = new QSpinBox(this);
    txtPaletteSteps->setMinimum(PALETTESTEPSMIN);
    txtPaletteSteps->setMaximum(PALETTESTEPSMAX);

    // log scale
    chkScalarFieldRangeLog = new QCheckBox(tr("Log. scale"));
    txtScalarFieldRangeBase = new LineEditDouble(SCALARFIELDRANGEBASE);
    connect(chkScalarFieldRangeLog, SIGNAL(stateChanged(int)), this, SLOT(doScalarFieldLog(int)));

    QPalette palette;
    palette.setColor(QPalette::WindowText, Qt::red);

    txtScalarFieldRangeMin = new LineEditDouble(0.1, true);
    connect(txtScalarFieldRangeMin, SIGNAL(textChanged(QString)), this, SLOT(doScalarFieldRangeMinChanged()));
    lblScalarFieldRangeMinError = new QLabel("");
    lblScalarFieldRangeMinError->setPalette(palette);
    lblScalarFieldRangeMinError->setVisible(false);
    txtScalarFieldRangeMax = new LineEditDouble(0.1, true);
    connect(txtScalarFieldRangeMax, SIGNAL(textChanged(QString)), this, SLOT(doScalarFieldRangeMaxChanged()));
    lblScalarFieldRangeMaxError = new QLabel("");
    lblScalarFieldRangeMaxError->setPalette(palette);
    lblScalarFieldRangeMaxError->setVisible(false);

    QGridLayout *layoutScalarFieldRange = new QGridLayout();
    lblScalarFieldRangeMin = new QLabel(tr("Minimum:"));
    layoutScalarFieldRange->addWidget(lblScalarFieldRangeMin, 0, 0);
    layoutScalarFieldRange->addWidget(txtScalarFieldRangeMin, 0, 1);
    layoutScalarFieldRange->addWidget(lblScalarFieldRangeMinError, 0, 2);
    layoutScalarFieldRange->addWidget(chkScalarFieldRangeAuto, 0, 3);
    lblScalarFieldRangeMax = new QLabel(tr("Maximum:"));
    layoutScalarFieldRange->addWidget(lblScalarFieldRangeMax, 1, 0);
    layoutScalarFieldRange->addWidget(txtScalarFieldRangeMax, 1, 1);
    layoutScalarFieldRange->addWidget(lblScalarFieldRangeMaxError, 1, 2);

    QGroupBox *grpScalarFieldRange = new QGroupBox(tr("Range"));
    grpScalarFieldRange->setLayout(layoutScalarFieldRange);

    QGridLayout *gridLayoutScalarFieldPalette = new QGridLayout();
    gridLayoutScalarFieldPalette->setColumnMinimumWidth(0, minWidth);
    gridLayoutScalarFieldPalette->setColumnStretch(1, 1);
    gridLayoutScalarFieldPalette->addWidget(new QLabel(tr("Palette:")), 0, 0);
    gridLayoutScalarFieldPalette->addWidget(cmbPalette, 0, 1, 1, 2);
    gridLayoutScalarFieldPalette->addWidget(new QLabel(tr("Steps:")), 2, 0);
    gridLayoutScalarFieldPalette->addWidget(txtPaletteSteps, 2, 1);
    gridLayoutScalarFieldPalette->addWidget(chkPaletteFilter, 2, 2);
    gridLayoutScalarFieldPalette->addWidget(new QLabel(tr("Base:")), 3, 0);
    gridLayoutScalarFieldPalette->addWidget(txtScalarFieldRangeBase, 3, 1);
    gridLayoutScalarFieldPalette->addWidget(chkScalarFieldRangeLog, 3, 2);

    QGroupBox *grpScalarFieldPalette = new QGroupBox(tr("Palette"));
    grpScalarFieldPalette->setLayout(gridLayoutScalarFieldPalette);

    // decimal places
    txtScalarDecimalPlace = new QSpinBox(this);
    txtScalarDecimalPlace->setMinimum(SCALARDECIMALPLACEMIN);
    txtScalarDecimalPlace->setMaximum(SCALARDECIMALPLACEMAX);

    // color bar
    chkShowScalarColorBar = new QCheckBox(tr("Show colorbar"), this);

    QGridLayout *gridLayoutScalarFieldColorbar = new QGridLayout();
    gridLayoutScalarFieldColorbar->addWidget(new QLabel(tr("Decimal places:")), 0, 0);
    gridLayoutScalarFieldColorbar->addWidget(txtScalarDecimalPlace, 0, 1);
    gridLayoutScalarFieldColorbar->addWidget(chkShowScalarColorBar, 0, 2);

    QGroupBox *grpScalarFieldColorbar = new QGroupBox(tr("Colorbar"));
    grpScalarFieldColorbar->setLayout(gridLayoutScalarFieldColorbar);

    QVBoxLayout *layoutScalarFieldAdvanced = new QVBoxLayout();
    layoutScalarFieldAdvanced->setMargin(0);
    layoutScalarFieldAdvanced->addWidget(grpScalarFieldPalette);
    layoutScalarFieldAdvanced->addWidget(grpScalarFieldColorbar);
    layoutScalarFieldAdvanced->addWidget(grpScalarFieldRange);

    QWidget *scalarWidget = new QWidget();
    scalarWidget->setLayout(layoutScalarFieldAdvanced);

    return scalarWidget;
}

void PostprocessorWidget::updateControls()
{
    loadBasic();
}

void PostprocessorWidget::doPaletteFilter(int state)
{
    txtPaletteSteps->setEnabled(!chkPaletteFilter->isChecked());
}

void PostprocessorWidget::refresh()
{
    if (m_scenePost3D->actSceneModePost3D->isChecked())
    {
        // widgetsLayout->setCurrentWidget(groupPost3d);

        // scalar view 3d
        radPost3DNone->setEnabled(Util::problem()->isSolved());
        radPost3DScalarField3D->setEnabled(Util::problem()->isSolved());
        radPost3DModel->setEnabled(Util::problem()->isSolved());

        // scalar
        groupPostScalar->setVisible(radPost3DScalarField3D->isEnabled() && radPost3DScalarField3D->isChecked());
        groupPostScalarAdvanced->setVisible((radPost3DScalarField3D->isEnabled() && radPost3DScalarField3D->isChecked())
                                            && !groupPostScalar->isCollapsed());
    }
}

void PostprocessorWidget::doApply()
{
    saveBasic();

    // time step
    QApplication::processEvents();

    emit apply();

    activateWindow();
}


void PostprocessorWidget::doScalarFieldRangeAuto(int state)
{
    txtScalarFieldRangeMin->setEnabled(!chkScalarFieldRangeAuto->isChecked());
    txtScalarFieldRangeMax->setEnabled(!chkScalarFieldRangeAuto->isChecked());
}

void PostprocessorWidget::doScalarFieldRangeMinChanged()
{
    lblScalarFieldRangeMinError->clear();
    lblScalarFieldRangeMinError->setVisible(false);
    lblScalarFieldRangeMaxError->clear();
    lblScalarFieldRangeMaxError->setVisible(false);
    btnOK->setEnabled(true);

    if (txtScalarFieldRangeMin->value() > txtScalarFieldRangeMax->value())
    {
        lblScalarFieldRangeMinError->setText(QString("> %1").arg(txtScalarFieldRangeMax->value()));
        lblScalarFieldRangeMinError->setVisible(true);
        //btnOK->setDisabled(true);
    }
    /*
    else if (txtScalarFieldRangeMin->value() == txtScalarFieldRangeMax->value())
    {
        lblScalarFieldRangeMinError->setText(QString("= %1").arg(txtScalarFieldRangeMax->value()));
        btnOK->setDisabled(true);
    }
    */
}

void PostprocessorWidget::doScalarFieldRangeMaxChanged()
{
    lblScalarFieldRangeMaxError->clear();
    lblScalarFieldRangeMinError->clear();
    btnOK->setEnabled(true);

    if (txtScalarFieldRangeMax->value() < txtScalarFieldRangeMin->value())
    {
        lblScalarFieldRangeMaxError->setText(QString("< %1").arg(txtScalarFieldRangeMin->value()));
        //btnOK->setDisabled(true);
    }
    /*
    else if (txtScalarFieldRangeMax->value() == txtScalarFieldRangeMin->value())
    {
        lblScalarFieldRangeMaxError->setText(QString("= %1").arg(txtScalarFieldRangeMin->value()));
        btnOK->setDisabled(true);
    }
    */
}

void PostprocessorWidget::doPostprocessorGroupClicked(QAbstractButton *button)
{
    refresh();
}

void PostprocessorWidget::doScalarFieldLog(int state)
{
    txtScalarFieldRangeBase->setEnabled(chkScalarFieldRangeLog->isChecked());
}

void PostprocessorWidget::doScalarFieldExpandCollapse(bool collapsed)
{
    groupPostScalarAdvanced->setVisible(!collapsed);
}
