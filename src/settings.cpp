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

#include "settings.h"
#include "scene.h"
#include "pythonlabagros.h"

#include "util/constants.h"
#include "gui/common.h"

const double minWidth = 110;

SettingsWidget::SettingsWidget(QWidget *parent) : QWidget(parent)
{
    setWindowIcon(icon("options"));
    setObjectName("Settings");

    createActions();
    createControls();

    load();

    connect(currentPythonEngineAgros(), SIGNAL(executedScript()), this, SLOT(updateControls()));
    connect(currentPythonEngineAgros(), SIGNAL(executedExpression()), this, SLOT(updateControls()));
}

void SettingsWidget::createActions()
{
    actSettings = new QAction(icon("options"), tr("Settings"), this);
    actSettings->setShortcut(QKeySequence("Alt+O"));
    actSettings->setCheckable(true);
}

void SettingsWidget::load()
{
    // workspace
    lblSceneFontExample->setFont(Util::config()->sceneFont);
    lblSceneFontExample->setText(QString("%1, %2").arg(lblSceneFontExample->font().family()).arg(lblSceneFontExample->font().pointSize()));

    chkShowAxes->setChecked(Util::config()->showAxes);

    chkZoomToMouse->setChecked(Util::config()->zoomToMouse);
    txtGeometryNodeSize->setValue(Util::config()->nodeSize);

    // 3d
    chkView3DLighting->setChecked(Util::config()->scalarView3DLighting);
    txtView3DAngle->setValue(Util::config()->scalarView3DAngle);
    chkView3DBackground->setChecked(Util::config()->scalarView3DBackground);

    // colors
    colorBackground->setColor(Util::config()->colorBackground);
    colorGrid->setColor(Util::config()->colorGrid);
    colorCross->setColor(Util::config()->colorCross);
    colorNodes->setColor(Util::config()->colorNodes);
    colorEdges->setColor(Util::config()->colorEdges);
    colorLabels->setColor(Util::config()->colorLabels);
    colorContours->setColor(Util::config()->colorContours);
    colorVectors->setColor(Util::config()->colorVectors);
    colorInitialMesh->setColor(Util::config()->colorInitialMesh);
    colorSolutionMesh->setColor(Util::config()->colorSolutionMesh);
    colorHighlighted->setColor(Util::config()->colorHighlighted);
    colorSelected->setColor(Util::config()->colorSelected);
}

void SettingsWidget::save()
{
    // workspace
    Util::config()->zoomToMouse = chkZoomToMouse->isChecked();

    Util::config()->sceneFont = lblSceneFontExample->font();

    Util::config()->showAxes = chkShowAxes->isChecked();

    Util::config()->nodeSize = txtGeometryNodeSize->value();

    // color
    Util::config()->colorBackground = colorBackground->color();
    Util::config()->colorGrid = colorGrid->color();
    Util::config()->colorCross = colorCross->color();
    Util::config()->colorNodes = colorNodes->color();
    Util::config()->colorEdges = colorEdges->color();
    Util::config()->colorLabels = colorLabels->color();
    Util::config()->colorContours = colorContours->color();
    Util::config()->colorVectors = colorVectors->color();
    Util::config()->colorInitialMesh = colorInitialMesh->color();
    Util::config()->colorSolutionMesh = colorSolutionMesh->color();
    Util::config()->colorHighlighted = colorHighlighted->color();
    Util::config()->colorSelected = colorSelected->color();

    // 3d
    Util::config()->scalarView3DLighting = chkView3DLighting->isChecked();
    Util::config()->scalarView3DAngle = txtView3DAngle->value();
    Util::config()->scalarView3DBackground = chkView3DBackground->isChecked();

    // save
    Util::config()->save();
}

void SettingsWidget::createControls()
{
    QWidget *workspace = controlsWorkspace();
    QWidget *colors = controlsColors();
    QWidget *advanced = controlsAdvanced();

    // tab widget
    QToolBox *tbxWorkspace = new QToolBox();
    tbxWorkspace->addItem(workspace, icon(""), tr("Workspace"));
    tbxWorkspace->addItem(colors, icon(""), tr("Colors"));
    tbxWorkspace->addItem(advanced, icon(""), tr("Advanced"));

    // layout workspace
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(tbxWorkspace);

    QWidget *widget = new QWidget(this);
    widget->setLayout(layout);

    // dialog buttons
    btnOK = new QPushButton(tr("Apply"));
    connect(btnOK, SIGNAL(clicked()), SLOT(doApply()));

    QHBoxLayout *layoutButtons = new QHBoxLayout();
    layoutButtons->addStretch();
    layoutButtons->addWidget(btnOK);

    QVBoxLayout *layoutMain = new QVBoxLayout();
    layoutMain->setContentsMargins(0, 5, 3, 5);
    layoutMain->addWidget(widget);
    layoutMain->addLayout(layoutButtons);

    setControls();

    setLayout(layoutMain);
}

QWidget *SettingsWidget::controlsWorkspace()
{
    // workspace
    chkZoomToMouse = new QCheckBox(tr("Zoom to mouse pointer"));

    QGridLayout *layoutGrid = new QGridLayout();
    layoutGrid->addWidget(chkZoomToMouse, 3, 0, 1, 2);

    QGroupBox *grpGrid = new QGroupBox(tr("Grid"));
    grpGrid->setLayout(layoutGrid);

    lblSceneFontExample = new QLabel(QString("%1, %2").arg(Util::config()->sceneFont.family()).arg(Util::config()->sceneFont.pointSize()));

    btnSceneFont = new QPushButton(tr("Set font"));
    connect(btnSceneFont, SIGNAL(clicked()), this, SLOT(doSceneFont()));

    QGridLayout *layoutFont = new QGridLayout();
    layoutFont->addWidget(lblSceneFontExample, 0, 1);
    layoutFont->addWidget(btnSceneFont, 0, 2);

    QGroupBox *grpFont = new QGroupBox(tr("Scene font"));
    grpFont->setLayout(layoutFont);

    // geometry
    txtGeometryNodeSize = new QSpinBox();
    txtGeometryNodeSize->setMinimum(1);
    txtGeometryNodeSize->setMaximum(20);

    QGridLayout *layoutGeometry = new QGridLayout();
    layoutGeometry->addWidget(new QLabel(tr("Node size:")), 0, 0);
    layoutGeometry->addWidget(txtGeometryNodeSize, 0, 1);

    QGroupBox *grpGeometry = new QGroupBox(tr("Geometry"));
    grpGeometry->setLayout(layoutGeometry);

    // other
    chkShowAxes = new QCheckBox(tr("Show axes"));

    QVBoxLayout *layoutOther = new QVBoxLayout();
    layoutOther->addWidget(chkShowAxes);

    QGroupBox *grpOther = new QGroupBox(tr("Other"));
    grpOther->setLayout(layoutOther);

    QPushButton *btnWorkspaceDefault = new QPushButton(tr("Default"));
    connect(btnWorkspaceDefault, SIGNAL(clicked()), this, SLOT(doWorkspaceDefault()));

    QVBoxLayout *layoutWorkspace = new QVBoxLayout();
    layoutWorkspace->addWidget(grpGrid);
    layoutWorkspace->addWidget(grpFont);
    layoutWorkspace->addWidget(grpGeometry);
    layoutWorkspace->addWidget(grpOther);
    layoutWorkspace->addStretch();
    layoutWorkspace->addWidget(btnWorkspaceDefault, 0, Qt::AlignLeft);

    QWidget *workspaceWidget = new QWidget();
    workspaceWidget->setLayout(layoutWorkspace);

    return workspaceWidget;
}

QWidget *SettingsWidget::controlsAdvanced()
{
    // layout 3d
    chkView3DLighting = new QCheckBox(tr("Ligthing"), this);
    txtView3DAngle = new QDoubleSpinBox(this);
    txtView3DAngle->setDecimals(1);
    txtView3DAngle->setSingleStep(1);
    txtView3DAngle->setMinimum(30);
    txtView3DAngle->setMaximum(360);
    chkView3DBackground = new QCheckBox(tr("Gradient background"), this);

    QGridLayout *layout3D = new QGridLayout();
    layout3D->addWidget(new QLabel(tr("Angle:")), 0, 1);
    layout3D->addWidget(txtView3DAngle, 0, 2);
    layout3D->addWidget(chkView3DLighting, 0, 3);
    layout3D->addWidget(new QLabel(tr("Height:")), 1, 1);
    layout3D->addWidget(chkView3DBackground, 1, 3);

    QGroupBox *grp3D = new QGroupBox(tr("3D view"));
    grp3D->setLayout(layout3D);

    QPushButton *btnAdvancedDefault = new QPushButton(tr("Default"));
    connect(btnAdvancedDefault, SIGNAL(clicked()), this, SLOT(doAdvancedDefault()));

    // layout postprocessor
    QVBoxLayout *layoutAdvanced = new QVBoxLayout();
    layoutAdvanced->addWidget(grp3D);
    layoutAdvanced->addStretch();
    layoutAdvanced->addWidget(btnAdvancedDefault, 0, Qt::AlignLeft);

    QWidget *advancedWidget = new QWidget(this);
    advancedWidget->setLayout(layoutAdvanced);

    return advancedWidget;
}

QWidget *SettingsWidget::controlsColors()
{
    QWidget *colorsWidget = new QWidget(this);

    // colors
    colorBackground = new ColorButton(this);
    colorGrid = new ColorButton(this);
    colorCross = new ColorButton(this);

    colorNodes = new ColorButton(this);
    colorEdges = new ColorButton(this);
    colorLabels = new ColorButton(this);
    colorContours = new ColorButton(this);
    colorVectors = new ColorButton(this);
    colorInitialMesh = new ColorButton(this);
    colorSolutionMesh = new ColorButton(this);

    colorHighlighted = new ColorButton(this);
    colorSelected = new ColorButton(this);

    QGridLayout *layoutColors = new QGridLayout();
    layoutColors->addWidget(new QLabel(tr("Background:")), 0, 0);
    layoutColors->addWidget(new QLabel(tr("Grid:")), 1, 0);
    layoutColors->addWidget(new QLabel(tr("Cross:")), 2, 0);
    layoutColors->addWidget(new QLabel(tr("Nodes:")), 3, 0);
    layoutColors->addWidget(new QLabel(tr("Edges:")), 4, 0);
    layoutColors->addWidget(new QLabel(tr("Labels:")), 5, 0);
    layoutColors->addWidget(new QLabel(tr("Contours:")), 6, 0);
    layoutColors->addWidget(new QLabel(tr("Vectors:")), 7, 0);
    layoutColors->addWidget(new QLabel(tr("Initial mesh:")), 8, 0);
    layoutColors->addWidget(new QLabel(tr("Solution mesh:")), 9, 0);
    layoutColors->addWidget(new QLabel(tr("Highlighted elements:")), 10, 0);
    layoutColors->addWidget(new QLabel(tr("Selected elements:")), 11, 0);

    layoutColors->addWidget(colorBackground, 0, 1);
    layoutColors->addWidget(colorGrid, 1, 1);
    layoutColors->addWidget(colorCross, 2, 1);
    layoutColors->addWidget(colorNodes, 3, 1);
    layoutColors->addWidget(colorEdges, 4, 1);
    layoutColors->addWidget(colorLabels, 5, 1);
    layoutColors->addWidget(colorContours, 6, 1);
    layoutColors->addWidget(colorVectors, 7, 1);
    layoutColors->addWidget(colorInitialMesh, 8, 1);
    layoutColors->addWidget(colorSolutionMesh, 9, 1);
    layoutColors->addWidget(colorHighlighted, 10, 1);
    layoutColors->addWidget(colorSelected, 11, 1);

    // default
    QPushButton *btnDefault = new QPushButton(tr("Default"));
    connect(btnDefault, SIGNAL(clicked()), this, SLOT(doColorsDefault()));

    QGroupBox *grpColor = new QGroupBox(tr("Colors"));
    grpColor->setLayout(layoutColors);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(grpColor);
    layout->addStretch();
    layout->addWidget(btnDefault, 0, Qt::AlignLeft);

    colorsWidget->setLayout(layout);

    return colorsWidget;
}

void SettingsWidget::setControls()
{

}

void SettingsWidget::updateControls()
{
    load();
}

void SettingsWidget::doApply()
{
    save();

    // time step
    QApplication::processEvents();

    emit apply();

    activateWindow();
}

void SettingsWidget::doWorkspaceDefault()
{
    lblSceneFontExample->setFont(FONT);
    lblSceneFontExample->setText(QString("%1, %2").arg(lblSceneFontExample->font().family()).arg(lblSceneFontExample->font().pointSize()));

    chkShowAxes->setChecked(SHOWAXES);

    chkZoomToMouse->setChecked(ZOOMTOMOUSE);
    txtGeometryNodeSize->setValue(GEOMETRYNODESIZE);
}

void SettingsWidget::doAdvancedDefault()
{
    chkView3DLighting->setChecked(VIEW3DLIGHTING);
    txtView3DAngle->setValue(VIEW3DANGLE);
    chkView3DBackground->setChecked(VIEW3DBACKGROUND);
}

void SettingsWidget::doColorsDefault()
{
    colorBackground->setColor(COLORBACKGROUND);
    colorGrid->setColor(COLORGRID);
    colorCross->setColor(COLORCROSS);
    colorNodes->setColor(COLORNODES);
    colorEdges->setColor(COLOREDGES);
    colorLabels->setColor(COLORLABELS);
    colorContours->setColor(COLORCONTOURS);
    colorVectors->setColor(COLORVECTORS);
    colorInitialMesh->setColor(COLORINITIALMESH);
    colorSolutionMesh->setColor(COLORSOLUTIONMESH);
    colorHighlighted->setColor(COLORHIGHLIGHTED);
    colorSelected->setColor(COLORSELECTED);
}

void SettingsWidget::doSceneFont()
{
    bool ok;
    QFont sceneFont = QFontDialog::getFont(&ok, lblSceneFontExample->font(), this);
    if (ok)
    {
        lblSceneFontExample->setFont(sceneFont);
        lblSceneFontExample->setText(QString("%1, %2").arg(lblSceneFontExample->font().family()).arg(lblSceneFontExample->font().pointSize()));
    }
}

// *******************************************************************************************************

ColorButton::ColorButton(QWidget *parent) : QPushButton(parent)
{
    setAutoFillBackground(false);
    setCursor(Qt::PointingHandCursor);
    connect(this, SIGNAL(clicked()), this, SLOT(doClicked()));
}

ColorButton::~ColorButton()
{
}

void ColorButton::setColor(const QColor &color)
{
    m_color = color;
    repaint();
}

void ColorButton::paintEvent(QPaintEvent *event)
{
    QPushButton::paintEvent(event);

    QPainter painter(this);
    painter.setPen(m_color);
    painter.setBrush(m_color);
    painter.drawRect(rect());
}

void ColorButton::doClicked()
{
    QColor color = QColorDialog::getColor(m_color);

    if (color.isValid())
    {
        setColor(color);
    }
}
